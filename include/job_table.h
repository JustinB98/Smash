#ifndef JOB_TABLE_H
#define JOB_TABLE_H

void job_table_init();

int job_table_insert(JOB *job);

void job_table_remove(JOB *job);

void job_table_remove_by_pid(pid_t pid);

JOB *job_table_find(int jobid);

void job_table_change_status(pid_t pid, JOB_STATUS status);

void job_table_mark_as_done(pid_t pid, int exit_code);

void job_table_mark_as_aborted(pid_t pid, int exit_code);

void print_all_jobs();

void print_and_remove_finished_jobs();

void job_table_fini();

#endif /* JOB_TABLE_H */
