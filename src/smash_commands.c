#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <pwd.h>
#include <signal.h>

#include "task.h"
#include "pipeline.h"
#include "exit_code.h"
#include "job.h"
#include "job_table.h"
#include "foreground_job.h"
#include "metadata.h"

#define print_smash_error(cmd, msg) fprintf(stderr, "-%s: %s: %s\n", PROGRAM_NAME, cmd, msg)
#define print_smash_error_with_extra(cmd, extra, msg) fprintf(stderr, "-%s: %s: %s: %s\n", PROGRAM_NAME, cmd, extra, msg)
#define print_smash_error_with_errno_and_extra(cmd, extra) print_smash_error_with_extra(cmd, extra, strerror(errno))
#define print_smash_error_with_errno(cmd) print_smash_error(cmd, strerror(errno))
#define print_smash_error_too_many_args(cmd) print_smash_error(cmd, "Too many arguments")
#define print_smash_error_no_jobid(cmd, jobid) print_smash_error_with_extra(cmd, jobid, "no such job")
#define print_smash_error_with_usage(cmd, usage) print_smash_error(cmd, usage)

static int smash_exit(TASK *task) {
	if (task->n_words != 1) { 
		print_smash_error_with_errno("exit");
		set_exit_code_failure();
		return -1;
	} else return 1;
}

static int smash_cd(TASK *task) {
	if (task->n_words > 2) {
		print_smash_error_too_many_args("cd");
		set_exit_code_failure();
		return -1;
	}
	char *path = NULL;
	if (task->n_words == 2) {
		path = task_get_word(task, 1);
	} else if (task->n_words == 1) {
		path = getenv("HOME");
	}
	int result = chdir(path);
	if (result < 0) {
		fprintf(stderr, "Could not find %s\n", path);
		print_smash_error_with_errno_and_extra("cd", path);
		set_exit_code_failure();
		return -1;
	}
	set_exit_code_success();
	return 1;
}

static int smash_pwd(TASK *task) {
	if (task->n_words > 1) {
		print_smash_error_too_many_args("pwd");
		set_exit_code_failure();
		return -1;
	}
	char wd[PATH_MAX];
	char *result = getcwd(wd, PATH_MAX);
	if (result == NULL) {
		print_smash_error_with_errno("pwd");
		set_exit_code_failure();
		return -1;
	}
	puts(wd);
	set_exit_code_success();
	return 1;
}

static int smash_jobs(TASK *task) {
	if (task->n_words > 1) {
		print_smash_error_too_many_args("jobs");
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

static int smash_fg(TASK *task) {
	JOB *job = job_control_prereq(task);
	if (job == NULL) return -1;
	sigset_t set, oset;
	sigfillset(&set);
	sigprocmask(SIG_SETMASK, &set, &oset);
	killpg(job->pid, SIGCONT);
	int result = wait_for_process(job, &oset, NULL);
	sigprocmask(SIG_SETMASK, &oset, NULL);
	if (result == 0) {
		job_table_remove(job);
	}
	return 1;
}

static int smash_bg(TASK *task) {
	JOB *job = job_control_prereq(task);
	if (job == NULL) return -1;
	killpg(job->pid, SIGCONT);
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
	killpg(job->pid, signum);
	return 1;
}

int execute_smash_command(TASK *task) {
	char *cmd = task_get_command(task);
	if (!strcmp(cmd, "cd")) return smash_cd(task);
	else if (!strcmp(cmd, "pwd")) return smash_pwd(task);
	else if (!strcmp(cmd, "jobs")) return smash_jobs(task);
	else if (!strcmp(cmd, "fg")) return smash_fg(task);
	else if (!strcmp(cmd, "bg")) return smash_bg(task);
	else if (!strcmp(cmd, "kill")) return smash_kill(task);
	return 0;
}

int should_exit(TASK *task) {
	char *cmd = task_get_command(task);
	if (!strcmp(cmd, "exit")) {
		return smash_exit(task);
	} else return 0;
}
