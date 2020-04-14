#include <stdlib.h>
#include <stdio.h>

#include "linked_list.h"
#include "hashtable.h"
#include "queue.h"
#include "task.h"
#include "pipeline.h"
#include "job.h"
#include "debug.h"

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
	/* TODO what if job is NULL */
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

static JOB *job_table_change_status_of_job(pid_t pid, int status) {
	JOB *job = hashtable_find(pid_table, pid);
	print_debug_message("[%d] %d %s status %s -> %s",
			job->jobid, pid,
			job->pipeline->full_command,
			job_status_names[job->status],
			job_status_names[status]);
	job->status = status;
	return job;
}

static void job_table_mark_as_finished(pid_t pid, int status, int exit_code) {
	JOB *job = job_table_change_status_of_job(pid, status);
	job->exit_code = exit_code;
}

void job_table_mark_as_done(pid_t pid, int exit_code) {
	job_table_mark_as_finished(pid, DONE, exit_code);
}

void job_table_mark_as_aborted(pid_t pid, int exit_code) {
	job_table_mark_as_finished(pid, ABORTED, exit_code);
}

void job_table_change_status(pid_t pid, int status) {
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

static void print_and_remove_finished_job(JOB *job) {
	if (job->status != DONE && job->status != ABORTED) return;
	printf("[%d] %d %s \'%s\' -------- Exit code: %d\n",
			job->jobid, job->pid,
			job_status_names[job->status],
			job->pipeline->full_command,
			job->exit_code);
	job_table_remove(job);
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

