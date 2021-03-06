#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <pwd.h>
#include <signal.h>
#ifdef EXTRA_CREDIT
#include <sys/resource.h>
#endif

#include "task.h"
#include "pipeline.h"
#include "job.h"
#include "job_table.h"
#include "foreground_job.h"
#include "metadata.h"
#include "debug.h"
#include "exit_code.h"

#define print_smash_error(cmd, msg) fprintf(stderr, "-%s: %s: %s\n", PROGRAM_NAME, cmd, msg)
#define print_smash_error_with_extra(cmd, extra, msg) fprintf(stderr, "-%s: %s: %s: %s\n", PROGRAM_NAME, cmd, extra, msg)
#define print_smash_error_with_errno_and_extra(cmd, extra) print_smash_error_with_extra(cmd, extra, strerror(errno))
#define print_smash_error_with_errno(cmd) print_smash_error(cmd, strerror(errno))
#define print_smash_error_too_many_args(cmd) print_smash_error(cmd, "Too many arguments")
#define print_smash_error_no_jobid(cmd, jobid) print_smash_error_with_extra(cmd, jobid, "no such job")
#define print_smash_error_with_usage(cmd, usage) print_smash_error(cmd, usage)
#define print_smash_error_no_job_control(cmd) print_smash_error(cmd, "no job control")

#define TRY_COMMAND_AND_HANDLE_ERROR(to_run, func_name, op_name, after) \
	if ((to_run) < 0) { \
		fprintf(stderr, "%s failed... aborting %s: %s\n", (func_name), (op_name), strerror(errno)); \
		after; \
	}

static int smash_error_check(TASK *task) {
	if (task->n_words > 2) { 
		print_smash_error_too_many_args("exit");
		return -1;
	} else return 1;
}

static int smash_cd(TASK *task) {
	if (task->n_words > 2) {
		print_smash_error_too_many_args("cd");
		return -1;
	}
	char *path = NULL;
	if (task->n_words == 2) {
		path = task_get_word(task, 1);
	} else if (task->n_words == 1) {
		path = getenv("HOME");
		if (path == NULL) {
			fprintf(stderr, "No home path found\n");
			return -1;
		}
	}
	int result = chdir(path);
	if (result < 0) {
		print_smash_error_with_errno_and_extra("cd", path);
		return -1;
	}
	return 1;
}

static int smash_pwd(TASK *task) {
	if (get_smash_pid() == getpid()) return 0;
	if (task->n_words > 1) {
		print_smash_error_too_many_args("pwd");
		return -1;
	}
	char wd[PATH_MAX];
	char *result = getcwd(wd, PATH_MAX);
	if (result == NULL) {
		print_smash_error_with_errno("pwd");
		return -1;
	}
	puts(wd);
	return 1;
}

static int smash_jobs(TASK *task) {
	if (task->n_words > 1) {
		print_smash_error_too_many_args("jobs");
		return -1;
	} else if (get_smash_pid() != getpid()) {
		print_smash_error_no_job_control("jobs");
		return -1;
	}
	print_all_jobs();
	return 1;
}

static JOB *get_job_from_table(char *jobid_str, char *cmd) {
	int jobid;
	if (sscanf(jobid_str, "%d", &jobid) < 0) {
		print_smash_error_no_jobid(cmd, jobid_str);
		return NULL;
	}
	JOB *job = job_table_find(jobid);
	if (job == NULL) {
		print_smash_error_no_jobid(cmd, jobid_str);
	}
	return job;

}

static JOB *job_control_prereq(TASK *task) {
	char *cmd = task_get_command(task);
	if (task->n_words != 2) {
		print_smash_error(cmd, "Only one arg <jobid>");
		return NULL;
	}
	char *jobid_str = task_get_word(task, 1);
	return get_job_from_table(jobid_str, cmd);
}

static void change_job_status_to_stopped(JOB *job) {
	job_table_change_status(job->pid, STOPPED);
}

static int smash_fg(TASK *task) {
	int result = -1;
	JOB *job = job_control_prereq(task);
	if (get_smash_pid() != getpid()) {
		print_smash_error_no_job_control("fg");
		return -1;
	} else if (job == NULL) {
		return -1;
	} else if (job->status == DONE || job->status == ABORTED) {
		print_smash_error("fg", "job has been terminated");
		return -1;
	}
	sigset_t set, oset;
	TRY_COMMAND_AND_HANDLE_ERROR(sigfillset(&set), "sigfillset", "fg", return -1);
	TRY_COMMAND_AND_HANDLE_ERROR(sigprocmask(SIG_SETMASK, &set, &oset), "sigfillset", "fg", return -1);
	puts(job->pipeline->full_command);
	if (is_interactive()) {
		TRY_COMMAND_AND_HANDLE_ERROR(tcsetpgrp(STDIN_FILENO, job->pid), "tcsetpgrp", "fg", goto smash_fg_finish);
	}
	TRY_COMMAND_AND_HANDLE_ERROR(kill(-job->pid, SIGCONT), "kill", "fg", goto smash_fg_finish);
	result = wait_for_process(job, &oset, change_job_status_to_stopped);
smash_fg_finish:
	TRY_COMMAND_AND_HANDLE_ERROR(sigprocmask(SIG_SETMASK, &oset, NULL), "sigprocmask", "fg", return -1);
	if (result == 0) {
		job_table_remove(job);
	}
	return 1;
}

static int smash_bg(TASK *task) {
	JOB *job = job_control_prereq(task);
	if (get_smash_pid() != getpid()) {
		print_smash_error_no_job_control("bg");
		return -1;
	} else if (job == NULL) {
		return -1;
	} else if (job->status == DONE || job->status == ABORTED) {
		print_smash_error("bg", "job has been terminated");
		return -1;
	} else if (job->status == RUNNING) {
		print_smash_error("bg", "job is already running in the background");
		/* Bash doesn't mark this as an error */
		return 1;
	}
	TRY_COMMAND_AND_HANDLE_ERROR(kill(-job->pid, SIGCONT), "kill", "bg", return -1);
	return 1;
}

static int smash_kill(TASK *task) {
	char *cmd = task_get_command(task);
	if (task->n_words != 3) {
		print_smash_error_with_usage("kill", "USAGE: kill -<SIGNUM> <jobid>");
		return -1;
	}
	char *signum_str = task_get_word(task, 1);
	int signum;
	if (sscanf(signum_str, "%d", &signum) < 0 || (signum > 0)) {
		print_smash_error_with_usage("kill", "USAGE: kill -<SIGNUM> <jobid>");
		return -1;
	}
	signum = -signum;
	/* Signal number without dash was already checked above */
	if (signum > NSIG) {
		print_smash_error_with_extra("kill", signum_str + 1, "Invalid signal specification");
		return -1;
	}
	char *jobstr_id = task_get_word(task, 2);
	JOB *job = get_job_from_table(jobstr_id, cmd);
	if (job == NULL) return -1;
	TRY_COMMAND_AND_HANDLE_ERROR(kill(-job->pid, signum), "kill", "kill", return -1);
	return 1;
}

static int smash_echo(TASK *task) {
	if (get_smash_pid() == getpid()) return 0;
	print_debug_message("Executing smash echo");
	task_print_all_args(task);
	printf("\n");
	return 1;
}

int execute_smash_command(TASK *task) {
	char *cmd = task_get_command(task);
	int result = 0;
	int fg_op = 0;
	if (!strcmp(cmd, "cd")) result = smash_cd(task);
	else if (!strcmp(cmd, "pwd")) result = smash_pwd(task);
	else if (!strcmp(cmd, "jobs")) result = smash_jobs(task);
	else if (!strcmp(cmd, "fg")) { fg_op = 1; result = smash_fg(task); }
	else if (!strcmp(cmd, "bg")) result = smash_bg(task);
	else if (!strcmp(cmd, "kill")) result = smash_kill(task);
	else if (!strcmp(cmd, "echo")) result = smash_echo(task);
	else if (!strcmp(cmd, "exit")) result = 1; /* Make sure it's a no op during a pipeline */
	if (result == 0) return 0;
	if (result < 0) {
		set_exit_code_failure();
	} else if (!fg_op) {
		set_exit_code_success();
	}
	return 1;
}

int should_exit(TASK *task, int *exit_code) {
	char *cmd = task_get_command(task);
	if (strcmp(cmd, "exit")) return 0;
	int invalid_exit_args = smash_error_check(task);
	if (invalid_exit_args < 0) return -1;
	if (task->n_words == 1) {
		*exit_code = get_exit_code();
		return 1;
	}
	char *exit_arg = task_get_word(task, 1);
	if (sscanf(exit_arg, "%d", exit_code) < 0) {
		print_smash_error_with_extra("exit", exit_arg, "numeric argument required");
		return -1;
	}
	return 1;
}
