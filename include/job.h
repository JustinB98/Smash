#ifndef JOB_H
#define JOB_H

typedef enum job_status {
	RUNNING, /* Process is running */
	STOPPED, /* Process is stopped/sleeping/suspended */
	ABORTED, /* Process was signaled */
	DONE	 /* Process was exited */
} JOB_STATUS;

typedef struct job {
	PIPELINE *pipeline; /* Commands to run */
	int jobid;			/* Job id for the job table */
	JOB_STATUS status;  /* Current status for the process */
	pid_t pid;			/* Process id for the child process */
	int exit_code;		/* Exit code for the process */
	int status_updated;	/* If the status was updated before displaying */
#ifdef EXTRA_CREDIT
	int time_failed;	/* If gettimeofday(2) or getrusage(2) fails */
	struct timeval starting_time, final_time; /* Value of out arg for gettimeofday(2) before and after process lifetime */
	struct rusage starting_usage, final_usage; /* Value of out arg for getrusage(2) before and after process lifetime */
#endif
} JOB;

/**
 * Starts running the pipeline in a new process via fork(2)
 *
 * @param pipeline	Pipeline to run
 * @param envp		Environment from main
 */
void start_pipeline(PIPELINE *pipeline, char *envp[]);

/**
 * Frees specified job. 
 * The job should not be used after a call to this function
 *
 * @param job	Job to be freed
 */
void free_job(JOB *job);

#endif /* JOB_H */
