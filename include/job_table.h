#ifndef JOB_TABLE_H
#define JOB_TABLE_H

/**
 * Initalizes the job table
 * Creates the two hashtables for querying jobs
 * and the priority queue for handling job ids
 */
void job_table_init();

/**
 * Inserts the job into the job table
 * Uses the pid of the job to insert
 * Updates the jobid field in job
 *
 * @param job	Job to insert
 *
 * @return		jobid generated by the priority queue
 */
int job_table_insert(JOB *job);

/**
 * Removes job from table. Uses the job's pid
 * Frees the job
 *
 * @param job	Job to remove
 */
void job_table_remove(JOB *job);

/**
 * Removes a job using the specified pid.
 * If the job is not found, a message will be printed to stderr
 *
 * @param pid	Pid got from waitpid(2) or some variant
 */
void job_table_remove_by_pid(pid_t pid);

/**
 * Finds a job using the jobid
 *
 * @param jobid	Job id to look for
 *
 * @return		Job associated with jobid
 *				NULL if no job was found
 */
JOB *job_table_find(int jobid);

/**
 * Changes the status of the job with the specified pid
 *
 * @param pid		Pid of the job to change status
 * @param status	New status
 */
void job_table_change_status(pid_t pid, JOB_STATUS status);

/**
 * Marks a job as done (DONE from the JOB_STATUS enum)
 *
 * @param pid		Pid of job in job table
 * @param exit_code	Exit code as gotten from WEXITSTATUS
 */
void job_table_mark_as_done(pid_t pid, int exit_code);

/**
 * Marks a job as aborted (ABORTED from the JOB_STATUS enum)
 *
 * @param pid		Pid of the job in the job table
 * @param exit_code Exit code for the aborted process
 *					Usually 128 + WTERMSIG
 */
void job_table_mark_as_aborted(pid_t pid, int exit_code);

/**
 * Prints all jobs in the job table to stdout
 * Will print the exit code of the job is DONE or ABORTED
 */
void print_all_jobs();

/**
 * Prints all the jobs to stdout that have been finished (DONE or ABORTED)
 * Removes the finished job from the job table
 * Should be called often enough to keep the job table clean
 * and to free job memory
 * But should be called when the will be little conflict with other
 * programs' stdout
 */
void print_and_remove_finished_jobs();

/**
 * Frees the job table and all memory associated with it
 * This includes all the jobs
 */
void job_table_fini();

#ifdef EXTRA_CREDIT
/**
 * Fills in the final_time and final_usage for the specified job
 * Will not update fields if getting the the starting time or usage
 * failed
 *
 * @param job	Job to fill fields in
 */
void finalize_times(JOB *job);

/**
 * Performs the subtraction on the final and starting times and
 * then prints the results to stdout
 *
 * @param job	Job whose times to print
 */
void print_times(JOB *job);
#endif

#endif /* JOB_TABLE_H */
