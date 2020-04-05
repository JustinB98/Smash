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

static int allocate_task_variables(TASK **task, char **string_to_parse, WORD_LIST **word_list) {
	*task = malloc(sizeof(TASK));
	if (*task == NULL) goto free_task;
	*string_to_parse = strdup(*string_to_parse);
	if (string_to_parse == NULL) goto free_string_to_parse;
	*word_list = malloc(sizeof(WORD_LIST));
	if (word_list == NULL) goto free_word_list;
	return 0;
free_word_list:
	free(*word_list);
free_string_to_parse:
	free(*string_to_parse);
free_task:
	free(*task);
	return -1;
}

static char *get_exit_code() {
	char *exit_code_str = malloc(5);
	if (exit_code_str == NULL) goto get_exit_code_finish;
	if (sprintf(exit_code_str, "%d", 44) < 0) goto get_exit_code_failed;
	goto get_exit_code_finish;
get_exit_code_failed:
	free(exit_code_str);
get_exit_code_finish:
	return exit_code_str;
}

static char *process_word(char *word) {
	if (*word == '$') {
		if (!strcmp(word, "$?")) return get_exit_code();
		char *var = getenv(word + 1);
		var = var == NULL ? "" : var;
		return strdup(var);
	} else {
		return strdup(word);
	}
}

TASK *parse_task(char *string_to_parse) {
	TASK *task;
	WORD_LIST *word_list;
	if (allocate_task_variables(&task, &string_to_parse, &word_list)) return TASK_FAILED;
	task->full_command = string_to_parse;
	char *s = string_to_parse;
	word_list->next = NULL;
	word_list->word = NULL;
	WORD_LIST *prev = NULL;
	task->word_list = word_list;
	size_t n_words = 0;
	char *token = NULL;
	while ((token = strtok_r(s, " ", &s))) {
		if (*token == '\n') continue;
		word_list->word = process_word(token);
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
