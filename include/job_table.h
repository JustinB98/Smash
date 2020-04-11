#ifndef JOB_TABLE_H
#define JOB_TABLE_H
#include "job.h"

void job_table_init();

int job_table_insert(JOB *job);

void job_table_remove(JOB *job);

void job_table_remove_by_pid(pid_t pid);

JOB *job_table_find(int jobid);

void job_table_change_status(pid_t pid, int status);

void print_all_jobs();

void print_and_remove_finished_jobs();

void job_table_fini();

#endif /* JOB_TABLE_H */
