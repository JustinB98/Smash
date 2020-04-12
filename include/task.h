#ifndef TASK_H
#define TASK_H 
#include <stdlib.h>

typedef struct word_list {
	char *word;
	struct word_list *next;
} WORD_LIST;

typedef struct task {
	char *full_command;
	WORD_LIST *word_list;
	size_t n_words;
	char *stdin_path;
	char *stdout_path;
	char *stderr_path;
	int fg;
} TASK;

#define TASK_FAILED ((TASK *) 0)
#define TASK_EMPTY ((TASK *) 1)

#ifdef EXTRA_CREDIT

typedef struct pipeline_list {
	TASK *task;
	struct pipeline_list *next;
} PIPELINE_LIST;

typedef struct pipeline {
	PIPELINE_LIST *list;
	char *full_command;
} PIPELINE;

PIPELINE *parse_pipeline(char *string_to_parse);

void free_pipeline(PIPELINE *pipeline);

#define PIPELINE_FAILED ((PIPELINE *) 0)
#define PIPELINE_EMPTY ((PIPELINE *) 1)

#endif

char *task_get_word(TASK *task, int index);

char *task_get_command(TASK *task);

void free_task(TASK *task);

TASK *parse_task(char *string_to_parse);

#endif /* TASK_H */
