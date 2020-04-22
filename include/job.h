#ifndef JOB_H
#define JOB_H

typedef enum job_status {
	RUNNING,
	STOPPED,
	ABORTED,
	DONE
} JOB_STATUS;

typedef struct job {
	PIPELINE *pipeline;
	int jobid;
	JOB_STATUS status;
	pid_t pid;
	int exit_code;
	int status_updated;
#ifdef EXTRA_CREDIT
	struct timeval starting_time;
	struct rusage starting_usage;
#endif
} JOB;

void start_pipeline(PIPELINE *pipeline, char *envp[]);

void free_job(JOB *job);

#endif /* JOB_H */
