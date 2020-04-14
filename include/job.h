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
} JOB;

void start_pipeline(PIPELINE *pipeline, char *envp[]);

void free_job(JOB *job);

#endif /* JOB_H */
