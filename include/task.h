#ifndef TASK_H
#define TASK_H 

/* Linked list of words */
typedef struct word_list {
	char *word;				/* Word that was delimited by a space */
	struct word_list *next; /* Next word (NULL if end) */
} WORD_LIST;

typedef struct task {
	char *full_command; 	/* String of the full task */
	WORD_LIST *word_list; 	/* list of words */
	size_t n_words; 		/* How many words */
	char *stdin_path; 		/* Redirection for stdin */
	char *stdout_path; 		/* Redirection for stdout */
	char *stderr_path; 		/* Redirection for stderr */
	int fg;					/* 0 for background, else foreground task */
} TASK;

/* Represents a task that could not be malloced */
#define TASK_FAILED ((TASK *) 0)
/* Represents a task that is empty */
#define TASK_EMPTY ((TASK *) 1)

/**
 * Used to traverse through the word list and grab a word at an index
 * Index should be in range [0, task->n_words)
 *
 * @param task	Task to use
 * @param index	Index of word
 *
 * @return word located at index "index" in the task's word list
 */
char *task_get_word(TASK *task, int index);

/**
 * Returns the first word of the specified task's word list,
 * which will be the command to execute
 *
 * @param task	Task to get the command of
 *
 * @return task_get_word(task, 0)
 */
char *task_get_command(TASK *task);

/**
 * Frees task and all memory associated with it
 * Task is useless after this function call
 *
 * @param task	Task to free
 */
void free_task(TASK *task);

/**
 * Prints all the arguments in the task (task_get_word(task, 1-n))
 *
 * @param task	Task to use
 */
void task_print_all_args(TASK *task);

#endif /* TASK_H */
