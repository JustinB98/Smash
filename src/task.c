#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "task.h"

static void free_word_list(WORD_LIST *word_list) {
	WORD_LIST *current = NULL;
	while (word_list != NULL) {
		current = word_list;
		word_list = word_list->next;
		free(current);
	}
}

void free_task(TASK *task) {
	free(task->full_command);
	WORD_LIST *list = task->word_list;
	free_word_list(list);
	free(task);
}

TASK *parse_task(char *string_to_parse) {
	TASK *task = malloc(sizeof(TASK));
	if (task == NULL) return TASK_FAILED;
	string_to_parse = strdup(string_to_parse);
	if (string_to_parse == NULL) {
		free(task);
		return TASK_FAILED;
	}
	if (string_to_parse == NULL) return TASK_FAILED;
	task->full_command = string_to_parse;
	char *s = string_to_parse;
	char *token = NULL;
	WORD_LIST *word_list = malloc(sizeof(WORD_LIST));
	if (word_list == NULL) {
		free(task);
		free(string_to_parse);
		return TASK_FAILED;
	}
	word_list->next = NULL;
	WORD_LIST *prev = NULL;
	task->word_list = word_list;
	size_t n_words = 0;
	while ((token = strtok_r(s, " ", &s))) {
		if (*token == '\n') continue;
		word_list->word = token;
		WORD_LIST *next_list = malloc(sizeof(WORD_LIST));
		if (next_list == NULL) {
			free_task(task);
			return TASK_FAILED;
		}
		word_list->next = next_list;
		prev = word_list;
		word_list = next_list;
		word_list->next = NULL;
		++n_words;
	}
	task->n_words = n_words;
	if (prev == NULL) {
		/* nothing entered, return 0 */
		free_task(task);
		return TASK_EMPTY;
	} else {
		prev->next = NULL;
		free(word_list);
		return task;
	}
}
