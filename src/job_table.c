#include <stdlib.h>
#include <stdio.h>

#include "linked_list.h"
#include "hashtable.h"
#include "queue.h"
#include "task.h"
#include "job.h"

static HASHTABLE *pid_table, *job_id_table;
static QUEUE *job_id_queue;

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
	hashtable_insert(pid_table, job->pid, job);
	hashtable_insert(job_id_table, new_jobid, job);
	queue_insert(job_id_queue, new_jobid);
	return new_jobid;
}

void job_table_remove(int jobid) {
	JOB *job = hashtable_find(job_id_table, jobid);
	hashtable_remove(job_id_table, jobid, NULL);
	hashtable_remove(pid_table, job->pid, NULL);
	queue_remove(job_id_queue, jobid);
	free_job(job);
}

void print_all_jobs() {
	int max_job_id = queue_peek(job_id_queue);
	for (int i = 1; i <= max_job_id; ++i) {
		void *data = hashtable_find(job_id_table, i);
		printf("Data found: %p\n", data);
		if (data != NULL) {
			JOB *job = data;
			printf("[%d] %d %d %s\n", i, job->pid, job->status, job->task->full_command);
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

