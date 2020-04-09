#ifndef JOB_H
#define JOB_H

#include "task.h"

typedef enum job_status {
	RUNNING,
	STOPPED,
	DONE
} JOB_STATUS;

typedef struct job {
	TASK *task;
	JOB_STATUS status;
	pid_t pid;
} JOB;

void start_task(TASK *task, char *envp[]);

void free_job(JOB *job);

#endif /* JOB_H */
