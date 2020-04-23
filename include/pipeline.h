#ifndef PIPELINE_H
#define PIPELINE_H

/* Linked list of tasks */
typedef struct pipeline_list {
	TASK *task;					/* Current task */
	struct pipeline_list *next; /* Pointer to next pipeline_list which has next task. NULL if end */
} PIPELINE_LIST;

typedef struct pipeline {
	PIPELINE_LIST *list; /* List of tasks */
	char *full_command;  /* String of entered string */
	size_t n_pipelines;  /* How many elements are in the list */
	int fg;				 /* 0 if background job, foreground job otherwise */
} PIPELINE;

/* Represents a pipeline that was never allocated properly */
#define PIPELINE_FAILED ((PIPELINE *) 0)
/* Represents a pipeline that had no tasks */
#define PIPELINE_EMPTY ((PIPELINE *) 1)

/**
 * Frees specified pipeline. Pipeline is useless after this function call
 *
 * @param pipeline	Pipeline to be freed
 */
void free_pipeline(PIPELINE *pipeline);

/**
 * Gets the task at the index (index is [0, pipeline->n_pipelines)
 *
 * @param pipeline	Pipeline to be used
 * @param index		Index of the task desired (0 - pipeline->n_pipelines)
 *
 * @return			Task at specified index
 */
TASK *pipeline_get_task(PIPELINE *pipeline, int index);

#endif /* PIPELINE_H */
