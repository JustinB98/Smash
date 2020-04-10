#include <stdlib.h>
#include <stdio.h>

#include "linked_list.h"
#include "hashtable.h"
#include "queue.h"
#include "task.h"
#include "job.h"

static HASHTABLE *pid_table, *job_id_table;
static QUEUE *job_id_queue;

static char *job_status_names[] = { "RUNNING", "STOPPED", "ABORTED", "DONE" };

void job_table_init() {
	pid_table = hashtable_init(53);
	if (pid_table == NULL) exit(EXIT_FAILURE);
	job_id_table = hashtable_init(53);
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

void job_table_remove(pid_t pid) {
	JOB *job = hashtable_find(pid_table, pid);
	/* TODO what if job is NULL */
	hashtable_remove(job_id_table, job->jobid, NULL);
	hashtable_remove(pid_table, job->pid, NULL);
	queue_remove(job_id_queue, job->jobid);
	free_job(job);
}

JOB *job_table_find(int jobid) {
	return hashtable_find(job_id_table, jobid);
}

void job_table_change_status(pid_t pid, int status) {
	JOB *job = hashtable_find(pid_table, pid);
	job->status = status;
}

void print_all_jobs() {
	int max_job_id = queue_peek(job_id_queue);
	for (int i = 1; i <= max_job_id; ++i) {
		void *data = hashtable_find(job_id_table, i);
		if (data != NULL) {
			JOB *job = data;
			printf("[%d] %d %s %s\n", i, job->pid, job_status_names[job->status], job->task->full_command);
		}
	}
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

