#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#ifdef EXTRA_CREDIT
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include "metadata.h"
#include "task.h"
#include "pipeline.h"
#include "job.h"
#include "exit_code.h"
#include "smash_commands.h"
#include "signal_handlers.h"
#include "job_table.h"
#include "foreground_job.h"
#include "debug.h"
#include "smash_runner.h"

void free_job(JOB *job) {
	free_pipeline(job->pipeline);
	free(job);
}

static void print_new_background_job(JOB *job) {
	print_debug_message("Inserting foreground job into background");
	job_table_insert(job);
	job_table_change_status(job->pid, STOPPED);
}

/* Return 0 if smash command, -1 if not */
static int try_smash_command(PIPELINE *pipeline) {
	if (pipeline->n_pipelines == 1) {
		int smash_command = execute_smash_command(pipeline_get_task(pipeline, 0));
		if (smash_command != 0) {
			free_pipeline(pipeline);
			return 0;
		}
	}
	return -1;
}

static void reset_signal_handlers() {
	install_signal_handler(SIGTTIN, SIG_DFL);
	install_signal_handler(SIGTTOU, SIG_DFL);
	install_signal_handler(SIGINT, SIG_DFL);
	install_signal_handler(SIGTSTP, SIG_DFL);
}

#ifdef EXTRA_CREDIT
void fill_time_fields(JOB *job) {
	if (gettimeofday(&job->starting_time, NULL) < 0) {
		perror("Could not get current time");
		memset(&job->starting_time, 0, sizeof(struct timeval));
		job->time_failed = 1;
	} else if (getrusage(RUSAGE_CHILDREN, &job->starting_usage) < 0) {
		perror("Could not get current program stats");
		memset(&job->starting_usage, 0, sizeof(struct rusage));
		job->time_failed = 1;
	} else {
		job->time_failed = 0;
	}
}
#endif

void start_pipeline(PIPELINE *pipeline, char *envp[]) {
	if (try_smash_command(pipeline) == 0) return;
	JOB *job = malloc(sizeof(JOB));
	if (job == NULL) {
		perror("Could not malloc for job");
		return;
	}
	sigset_t set, oset;
	sigfillset(&set);
	if (sigprocmask(SIG_SETMASK, &set, &oset) < 0) {
		perror("sigprocmask: Could not create child process");
		return;
	}
	sigstop_flag = 0;
	pid_t pid = fork();
	if (pid < 0) {
		perror("Could not spawn process");
		if (sigprocmask(SIG_SETMASK, &oset, NULL) < 0) {
			perror("sigprocmask failed... Program will not function normally");
		}
		return;
	} else if (pid == 0) {
		reset_signal_handlers();
		pid = getpid();
		if (setpgid(pid, pid) < 0) {
			perror("Could not set pgid of child process");
			abort();
		}
#ifdef EXTRA_CREDIT
		child_process_start_job(pipeline);
#else
		child_process_start_job(pipeline_get_task(pipeline, 0));
#endif
	}
#ifdef EXTRA_CREDIT
	fill_time_fields(job);
#endif
	if (setpgid(pid, pid) < 0) {
		/* Possible that child cannot set it either, in which case the child will abort itself */
		perror("Could not set forked child's pgid");
	}
	print_debug_message("RUNNING: [%d] %s", pid, pipeline->full_command);
	/* Fill job fields */
	job->status_updated = 0;
	job->pipeline = pipeline;
	job->status = RUNNING;
	job->pid = pid;
	if (pipeline->fg) {
		int result = wait_for_process(job, &oset, print_new_background_job);
		if (result == 0) free_job(job);
	} else {
		int jobid = job_table_insert(job);
		printf("[%d] %d STARTED \'%s\'\n", jobid, pid, pipeline->full_command);
	}
	if (sigprocmask(SIG_SETMASK, &oset, NULL) < 0) {
		perror("sigprocmask failed: Program will not behave normally");
	}
	sigstop_flag = 0;
}

