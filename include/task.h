#ifndef TASK_H
#define TASK_H 

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

void free_task(TASK *task);

TASK *parse_task(char *string_to_parse);

#endif /* TASK_H */
