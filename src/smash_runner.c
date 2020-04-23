#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#ifdef EXTRA_CREDIT
#include <sys/resource.h>
#endif

#include "task.h"
#include "pipeline.h"
#include "job.h"
#include "exit_code.h"
#include "smash_commands.h"
#include "signal_handlers.h"
#include "job_table.h"
#include "foreground_job.h"
#include "debug.h"

#define ON_ERROR_EXIT(to_run, perror_msg) \
	if ((to_run) < 0) { \
		perror(perror_msg); \
		exit(EXIT_FAILURE); \
	}


static void safe_open_file_for_reading(char *path, int *fd, int default_value) {
	if (path == NULL) {
		*fd = default_value;
		return;
	}
	int possible_fd = open(path, O_WRONLY | O_TRUNC | O_CREAT, 0666);
	if (possible_fd < 0) {
		fprintf(stderr, "Could not open %s: %s\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}
	*fd = possible_fd;
}

static void open_stdin_path(char *stdin_path, int *stdin_fd) {
	if (stdin_path == NULL) {
		*stdin_fd = STDIN_FILENO;
		return;
	}
	int fd = open(stdin_path, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open %s: %s\n", stdin_path, strerror(errno));
		exit(EXIT_FAILURE);
	}
	*stdin_fd = fd;
}

static void fill_redirection_fds(TASK *task, int *stdin_fd, int *stdout_fd, int *stderr_fd) {
	safe_open_file_for_reading(task->stdout_path, stdout_fd, STDOUT_FILENO);
	safe_open_file_for_reading(task->stderr_path, stderr_fd, STDERR_FILENO);
	open_stdin_path(task->stdin_path, stdin_fd);
}

#ifndef EXTRA_CREDIT
static void dup_fd(int fd, int default_value, char *type) {
	if (fd != default_value) {
		int result = dup2(fd, default_value);
		if (result < 0) {
			fprintf(stderr, "Could not dup2 %s: %s\n", type, strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
}

static void dup_fds(int stdin_fd, int stdout_fd, int stderr_fd) {
	dup_fd(stdin_fd, STDIN_FILENO, "stdin");
	dup_fd(stdout_fd, STDOUT_FILENO, "stdout");
	dup_fd(stderr_fd, STDERR_FILENO, "stderr");
}
#endif
static void fill_argv(WORD_LIST *list, size_t n_words, char **argv) {
	WORD_LIST *current = list;
	for (size_t i = 0; i < n_words; ++i) {
		*argv = current->word;
		++argv;
		current = current->next;
	}
	*argv = NULL;
}

static void start_exec(TASK *task){
	int smash_command = execute_smash_command(task);
	if (smash_command != 0) {
		exit(get_exit_code());
	}
	char *argv[task->n_words + 1];
	fill_argv(task->word_list, task->n_words, argv);
	sigset_t set;
	sigemptyset(&set);
	sigprocmask(SIG_SETMASK, &set, NULL);
	if (execvp(argv[0], argv) < 0) {
		fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
		/* Standard terminal error for commands not found */
		exit(127);
	}
}

#ifndef EXTRA_CREDIT
void child_process_start_job(TASK *task) {
	int stdin_fd, stdout_fd, stderr_fd;
	fill_redirection_fds(task, &stdin_fd, &stdout_fd, &stderr_fd);
	dup_fds(stdin_fd, stdout_fd, stderr_fd);
	start_exec(task);
}
#endif

#ifdef EXTRA_CREDIT
/* Runs a command and will use the ifd and ofd to replace stdin and stdout */
static pid_t exe_command(TASK *task, int ifd, int ofd) {
	pid_t cpid;
	if ((cpid = fork()) == 0) {
		/* Child */
		/* Replace stdin with ifd and stdout with ofd */
		int in_fd, out_fd, error_fd;
		fill_redirection_fds(task, &in_fd, &out_fd, &error_fd);
		// fprintf(stderr, "in_fd: %d out_fd: %d error_fd: %d ifd: %d ofd: %d\n", in_fd, out_fd, error_fd, ifd, ofd);
		if (in_fd != STDIN_FILENO) {
			close(ifd);
			ifd = in_fd;
		}
		if (out_fd != STDOUT_FILENO) {
			close(ofd);
			ofd = out_fd;
		}
		ON_ERROR_EXIT(dup2(ifd, STDIN_FILENO), "dup2");
		ON_ERROR_EXIT(dup2(ofd, STDOUT_FILENO), "dup2");
		ON_ERROR_EXIT(dup2(error_fd, STDERR_FILENO), "dup2");
		/* Doesn't return */
		start_exec(task);
	} else if (cpid < 0) {
		perror("Something went went wrong when forking, aborting");
		abort();
	} else {
		int result = setpgid(cpid, getpgid(getpid()));
		if (result < 0) {
			perror("Could not set process ground for child");
			abort();
		}
	}
	return cpid;
}

/* populates arguments with read and write fds from the pipe system call. Will abort if pipe() fails */
static void get_pipes(int *read_fd, int *write_fd) {
	int pfds[2];
	if (pipe(pfds) < 0) abort(); /* pipe failed */
	*read_fd = pfds[0];
	*write_fd = pfds[1];
	/* Set the pipes to close on an exec function */
	// fcntl(*read_fd, F_SETFD, FD_CLOEXEC);
	// fcntl(*write_fd, F_SETFD, FD_CLOEXEC);
}

static void wait_for_pipeline_to_end(pid_t cpids[], size_t n_pipelines) {
	sigset_t empty_set;
	sigemptyset(&empty_set);
	int exit_status = 0;
	pid_t last_pid = cpids[n_pipelines - 1];
	while (n_pipelines > 0) {
		sigsuspend(&empty_set);
		if (sigchld_flag) {
			pid_t rpid;
			int wstatus;
			while ((rpid = waitpid(-1, &wstatus, WNOHANG)) > 0) {
				if (rpid == last_pid) {
					exit_status = WEXITSTATUS(wstatus);
				}
				--n_pipelines;
			}
		}
		sigchld_flag = 0;
	}
	exit(exit_status);
}

/* Takes a pipeline and runs all commands concurrently */
void child_process_start_job(PIPELINE *pipeline) {
	sigint_flag = 0; sigstop_flag = 0; sigchld_flag = 0;
	sigset_t full_signal_set;
	sigfillset(&full_signal_set);
	sigprocmask(SIG_SETMASK, &full_signal_set, NULL);
	int ifd = STDIN_FILENO;
	int ending_ofd = STDOUT_FILENO;
	PIPELINE_LIST *pl = pipeline->list;
	/* if (ifd != STDIN_FILENO) fcntl(ifd, F_SETFD, FD_CLOEXEC); */
	/* added for readability */
	int read_fd = -1, write_fd = -1;
	pid_t cpids[pipeline->n_pipelines];
	int i = 0;
	while (pl != NULL) {
		if (pl->next != NULL) {
			get_pipes(&read_fd, &write_fd);
		} else {
			write_fd = ending_ofd;
		}
		/* Can't use read_fd yet. Need to start off with our initial fd (ifd) */
		pid_t pid = exe_command(pl->task, ifd, write_fd);
		cpids[i++] = pid;
		/* No need to keep the write fd open in this process any longer */
		if (write_fd != STDOUT_FILENO) close(write_fd);
		/* Child process will use pfds[1] to write and pfds[0] will get this input. */
		/* Need to redirect this fd to be use as stdin for the next program */
		/* current path will write to this. Now the next program needs to read from it */
		if (ifd != STDIN_FILENO) close(ifd);
		/* The process we just started will write to this read end of the pipe */
		/* We need to mask stdin (using dup2) with read_fd in order to get piping */
		ifd = read_fd;
		pl = pl->next;
	}
	wait_for_pipeline_to_end(cpids, pipeline->n_pipelines);
}

#endif

