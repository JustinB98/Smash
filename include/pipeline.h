#ifndef PIPELINE_H
#define PIPELINE_H

typedef struct pipeline_list {
	TASK *task;
	struct pipeline_list *next;
} PIPELINE_LIST;

typedef struct pipeline {
	PIPELINE_LIST *list;
	char *full_command;
	size_t n_pipelines;
	int fg;
} PIPELINE;

#define PIPELINE_FAILED ((PIPELINE *) 0)
#define PIPELINE_EMPTY ((PIPELINE *) 1)

void free_pipeline(PIPELINE *pipeline);

TASK *pipeline_get_task(PIPELINE *pipeline, int index);

#endif /* PIPELINE_H */
