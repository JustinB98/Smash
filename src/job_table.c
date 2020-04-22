#include <stdlib.h>
#include <stdio.h>

#ifdef EXTRA_CREDIT
#include <sys/time.h>
#include <sys/times.h>
#include <sys/resource.h>
#include "args.h"
#endif

#include "linked_list.h"
#include "hashtable.h"
#include "queue.h"
#include "task.h"
#include "pipeline.h"
#include "job.h"
#include "debug.h"
#include "job_table.h"

static HASHTABLE *pid_table, *job_id_table;
static QUEUE *job_id_queue;

static char *job_status_names[] = { "RUNNING", "STOPPED", "ABORTED", "DONE" };

#define HASHTABLE_SIZE 53

void job_table_init() {
	pid_table = hashtable_init(HASHTABLE_SIZE);
	if (pid_table == NULL) exit(EXIT_FAILURE);
	job_id_table = hashtable_init(HASHTABLE_SIZE);
	if (job_id_table == NULL) exit(EXIT_FAILURE);
	job_id_queue = queue_init();
	if (job_id_queue == NULL) exit(EXIT_FAILURE);
}

int job_table_insert(JOB *job) {
	int new_jobid = queue_peek(job_id_queue) + 1;
	job->jobid = new_jobid;
	hashtable_insert(pid_table, job->pid, job);
	hashtable_insert(job_id_table, new_jobid, job);
	queue_insert(job_id_queue, new_jobid);
	return new_jobid;
}

void job_table_remove(JOB *job) {
	if (job == NULL) return;
	print_debug_message("Removing job %d (pid=%d) \'%s\' from the job table",
			job->jobid, job->pid, job->pipeline->full_command);
	hashtable_remove(job_id_table, job->jobid, NULL);
	hashtable_remove(pid_table, job->pid, NULL);
	queue_remove(job_id_queue, job->jobid);
	free_job(job);

}

void job_table_remove_by_pid(pid_t pid) {
	JOB *job = hashtable_find(pid_table, pid);
	job_table_remove(job);
}

JOB *job_table_find(int jobid) {
	return hashtable_find(job_id_table, jobid);
}

static JOB *job_table_change_status_of_job(pid_t pid, JOB_STATUS status) {
	JOB *job = hashtable_find(pid_table, pid);
	if (job == NULL) {
		print_debug_message("We've reaped a child that wasn't in the job table [%d]", pid);
		return NULL;
	}
	print_debug_message("[%d] %d %s status %s -> %s",
			job->jobid, pid,
			job->pipeline->full_command,
			job_status_names[job->status],
			job_status_names[status]);
	job->status = status;
	job->status_updated = 1;
	return job;
}

static JOB *job_table_mark_as_finished(pid_t pid, JOB_STATUS status, int exit_code) {
	JOB *job = job_table_change_status_of_job(pid, status);
	if (job != NULL) {
		job->exit_code = exit_code;
#ifdef EXTRA_CREDIT
		finalize_times(job);
#endif
	}
	return job;
}

void job_table_mark_as_done(pid_t pid, int exit_code) {
	job_table_mark_as_finished(pid, DONE, exit_code);
}

void job_table_mark_as_aborted(pid_t pid, int exit_code) {
	job_table_mark_as_finished(pid, ABORTED, exit_code);
}

void job_table_change_status(pid_t pid, JOB_STATUS status) {
	job_table_change_status_of_job(pid, status);
}

static void for_each_job(void (*job_consumer)(JOB *)) {
	int max_job_id = queue_peek(job_id_queue);
	for (int i = 1; i <= max_job_id; ++i) {
		void *data = hashtable_find(job_id_table, i);
		if (data != NULL) {
			job_consumer(data);
		}
	}
}

#ifdef EXTRA_CREDIT
void finalize_times(JOB *job) {
	if (!has_t_flag()) return;
	if (job->time_failed) {
		fprintf(stderr, "Since getting time for job already failed, the time for this job cannot be displayed\n");
		return;
	}
	if (gettimeofday(&job->final_time, NULL) < 0) {
		perror("gettimeofday failed... cannot get times");
		job->time_failed = 1;
		return;
	}
	if (getrusage(RUSAGE_CHILDREN, &job->final_usage) < 0) {
		perror("getrusage failed... cannot get times");
		return;
	}
}

void print_times(JOB *job) {
	if (!has_t_flag()) return;
	if (job->time_failed) {
		fprintf(stderr, "Since getting time for job already failed, the time for this job cannot be displayed\n");
		return;
	}
	struct timeval final_time;
	struct timeval final_utime, final_stime;
	timersub(&job->final_time, &job->starting_time, &final_time);
	timersub(&job->final_usage.ru_utime, &job->starting_usage.ru_utime, &final_utime);
	timersub(&job->final_usage.ru_stime, &job->starting_usage.ru_stime, &final_stime);
	printf("TIMES: real: %ld.%ld user: %ld.%ld sys: %ld.%ld\n",
			final_time.tv_sec, final_time.tv_usec,
			final_utime.tv_sec, final_utime.tv_usec,
			final_stime.tv_sec, final_stime.tv_usec
		  );
}
#endif

static void print_and_remove_finished_job(JOB *job) {
	if (!job->status_updated) return;
	job->status_updated = 0;
	printf("[%d] %d %s \'%s\'",
			job->jobid, job->pid,
			job_status_names[job->status],
			job->pipeline->full_command);
	if (job->status != DONE && job->status != ABORTED) goto print_and_remove_finished_job_finish;
	printf(" -------- Exit code: %d\n", job->exit_code);
#ifdef EXTRA_CREDIT
	print_times(job);
#endif
	job_table_remove(job);
	return;
print_and_remove_finished_job_finish:
	printf("\n");
}

static void print_single_job(JOB *job) {
	if (job->status == DONE || job->status == ABORTED) {
		print_and_remove_finished_job(job);
	} else {
		printf("[%d] %d %s \'%s\'\n",
				job->jobid, job->pid,
				job_status_names[job->status],
				job->pipeline->full_command);
	}
}

void print_all_jobs() {
	for_each_job(print_single_job);
}

void print_and_remove_finished_jobs() {
	for_each_job(print_and_remove_finished_job);
}

static void job_freer(void *data) {
	JOB *job = data;
	free_job(job);
}

void job_table_fini() {
	hashtable_fini(pid_table, NULL);
	hashtable_fini(job_id_table, job_freer);
	queue_fini(job_id_queue);
}

