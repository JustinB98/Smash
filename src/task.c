#include <stdlib.h>
#include <stdio.h>

#include "task.h"

static void free_word_list(WORD_LIST *word_list) {
	WORD_LIST *current = NULL;
	while (word_list != NULL) {
		current = word_list;
		word_list = word_list->next;
		if (current->word != NULL) free(current->word);
		free(current);
	}
}

void free_task(TASK *task) {
	free(task->full_command);
	WORD_LIST *list = task->word_list;
	free_word_list(list);
	free(task);
}

char *task_get_word(TASK *task, int index) {
	if (index > task->n_words) return NULL;
	WORD_LIST *current = task->word_list;
	for (int i = 0; i < index; ++i) {
		current = current->next;
	}
	return current->word;
}

char *task_get_command(TASK *task) {
	return task->word_list->word;
}

