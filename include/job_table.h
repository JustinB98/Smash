#ifndef JOB_TABLE_H
#define JOB_TABLE_H
#include "job.h"

void job_table_init();

int job_table_insert(JOB *job);

void job_table_remove(int jobid);

JOB *job_table_find(int jobid);

void job_table_change_status(pid_t pid, int status);

void print_all_jobs();

void job_table_fini();

#endif /* JOB_TABLE_H */
