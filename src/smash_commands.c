#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <pwd.h>
#include <signal.h>

#include "task.h"
#include "exit_code.h"
#include "job_table.h"
#include "foreground_job.h"

static int smash_exit(TASK *task) {
	if (task->n_words != 1) { 
		fprintf(stderr, "smash: Too many arguments for exit\nUSAGE: exit\n");
		set_exit_code_failure();
		return -1;
	} else return 1;
}

static int smash_cd(TASK *task) {
	if (task->n_words > 2) {
		fprintf(stderr, "smash: Too many arguments for cd\nUSAGE: cd [PATH]\n");
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
		set_exit_code_failure();
		return -1;
	}
	set_exit_code_success();
	return 1;
}

static int smash_pwd(TASK *task) {
	if (task->n_words > 1) {
		fprintf(stderr, "smash: Too many arguments for pwd\nUSAGE: pwd\n");
		set_exit_code_failure();
		return -1;
	}
	char wd[PATH_MAX];
	char *result = getcwd(wd, PATH_MAX);
	if (result == NULL) {
		fprintf(stderr, "smash: Could not get current working directory\n");
		set_exit_code_failure();
		return -1;
	}
	puts(wd);
	set_exit_code_success();
	return 1;
}

static int smash_jobs(TASK *task) {
	if (task->n_words > 1) {
		fprintf(stderr, "smash: Too many arguments for jobs\n");
		return -1;
	}
	print_all_jobs();
	return 1;
}

static JOB *job_control_prereq(TASK *task) {
	int jobid;
	char *cmd = task_get_command(task);
	if (task->n_words != 2) {
		fprintf(stderr, "smash: Incorrect usage of %1$s\nUSAGE: %1$s <jobid>\n", cmd);
		return NULL;
	}
	char *jobid_str = task_get_word(task, 1);
	if (sscanf(jobid_str, "%d", &jobid) < 0) {
		fprintf(stderr, "smash: Job id must be a number\n");
		return NULL;
	}
	JOB *job = job_table_find(jobid);
	if (job == NULL) {
		fprintf(stderr, "smash: Could not find job number%d\n", jobid);
	}
	return job;
}

static int smash_fg(TASK *task) {
	JOB *job = job_control_prereq(task);
	if (job == NULL) return -1;
	sigset_t set, oset;
	sigfillset(&set);
	sigprocmask(SIG_SETMASK, &set, &oset);
	kill(job->pid, SIGCONT);
	int result = wait_for_process(job, &oset, NULL);
	sigprocmask(SIG_SETMASK, &oset, NULL);
	if (result == 0) {
		job_table_remove(job->pid);
	}
	return 1;
}

static int smash_bg(TASK *task) {
	JOB *job = job_control_prereq(task);
	if (job == NULL) return -1;
	kill(job->pid, SIGCONT);
	return 1;
}

int execute_smash_command(TASK *task) {
	char *cmd = task_get_command(task);
	if (!strcmp(cmd, "cd")) return smash_cd(task);
	else if (!strcmp(cmd, "pwd")) return smash_pwd(task);
	else if (!strcmp(cmd, "jobs")) return smash_jobs(task);
	else if (!strcmp(cmd, "fg")) return smash_fg(task);
	else if (!strcmp(cmd, "bg")) return smash_bg(task);
	return 0;
}

int should_exit(TASK *task) {
	char *cmd = task_get_command(task);
	if (!strcmp(cmd, "exit")) {
		return smash_exit(task);
	} else return 0;
}
