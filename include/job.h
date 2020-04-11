#ifndef JOB_H
#define JOB_H

#include "task.h"

typedef enum job_status {
	RUNNING,
	STOPPED,
	ABORTED,
	DONE
} JOB_STATUS;

typedef struct job {
	TASK *task;
	int jobid;
	JOB_STATUS status;
	pid_t pid;
	int exit_code;
} JOB;

void start_task(TASK *task, char *envp[]);

void free_job(JOB *job);

#endif /* JOB_H */
