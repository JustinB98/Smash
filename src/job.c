#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "task.h"
#include "job.h"
#include "exit_code.h"

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

static void fill_argv(WORD_LIST *list, size_t n_words, char **argv) {
	WORD_LIST *current = list;
	for (size_t i = 0; i < n_words; ++i) {
		*argv = current->word;
		++argv;
		current = current->next;
	}
	*argv = NULL;
}

static void child_process_start_job(TASK *task, char *envp[]) {
	int stdin_fd, stdout_fd, stderr_fd;
	fill_redirection_fds(task, &stdin_fd, &stdout_fd, &stderr_fd);
	dup_fds(stdin_fd, stdout_fd, stderr_fd);
	char *argv[task->n_words];
	fill_argv(task->word_list, task->n_words, argv);
	if (execvp(argv[0], argv) < 0) {
		fprintf(stderr, "Could not find program: %s\n", argv[0]);
		exit(127);
	}
}

void start_task(TASK *task, char *envp[]) {
	pid_t pid = fork();
	if (pid < 0) {
		fprintf(stderr, "Could not spawn process\n");
		return;
	} else if (pid == 0) {
		child_process_start_job(task, envp);
	}
	if (task->fg) {
		int exit_status;
		waitpid(pid, &exit_status, 0);
		exit_code = WEXITSTATUS(exit_status);
	}
}

