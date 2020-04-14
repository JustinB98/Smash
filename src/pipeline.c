#include <stdlib.h>
#include <stdio.h>

#include "task.h"
#include "pipeline.h"

void free_pipeline(PIPELINE *pipeline) {
	free(pipeline->full_command);
	PIPELINE_LIST *current = pipeline->list;
	while (current) {
		PIPELINE_LIST *prev = current;
		free_task(current->task);
		current = current->next;
		free(prev);
	}
	free(pipeline);
}

TASK *pipeline_get_task(PIPELINE *pipeline, int index) {
	PIPELINE_LIST *current = pipeline->list;
	for (int i = 0; i < index; ++i) {
		current = current->next;
	}
	return current->task;
}
