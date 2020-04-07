#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "task.h"
#include "exit_code.h"

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
	TASK *task_ptr = *task;
	task_ptr->stdin_path = NULL;
	task_ptr->stdout_path = NULL;
	task_ptr->stderr_path = NULL;
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

static char *get_exit_code_str() {
	char *exit_code_str = malloc(10);
	if (exit_code_str == NULL) goto get_exit_code_finish;
	if (sprintf(exit_code_str, "%d", get_exit_code()) < 0) goto get_exit_code_failed;
	goto get_exit_code_finish;
get_exit_code_failed:
	free(exit_code_str);
get_exit_code_finish:
	return exit_code_str;
}

static char *process_word(char *word) {
	if (*word == '$') {
		if (!strcmp(word, "$?")) return get_exit_code_str();
		char *var = getenv(word + 1);
		var = var == NULL ? "" : var;
		return strdup(var);
	} else {
		return strdup(word);
	}
}

/* Returns 1 if token was a redirection. 0 if not */
static int handle_redirection(TASK *task, char *token) {
	if (token[0] == '2' && token[1] == '>') {
		task->stderr_path = token + 2;
		return 1;
	} else if (token[0] == '>') {
		task->stdout_path = token + 1;
		return 1;
	} else if (token[0] == '<') {
		task->stdin_path = token + 1;
		return 1;
	}
	return 0;
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
	char *last_token = NULL;
	while ((token = strtok_r(s, " ", &s))) {
		if (*token == '\n') continue;
		last_token = token;
		if (handle_redirection(task, token) > 0) continue;
		word_list->word = process_word(token);
		if (word_list->word == NULL) goto parse_task_failed;
		WORD_LIST *next_list = malloc(sizeof(WORD_LIST));
		if (next_list == NULL) goto parse_task_failed;
		word_list->next = next_list;
		prev = word_list;
		word_list = next_list;
		word_list->next = NULL;
		++n_words;
	}
	int fg = 1;
	if (last_token != NULL) {
		if (!strcmp(last_token, "&")) {
			--n_words;
			fg = 0;
		}
	}
	task->fg = fg;
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
parse_task_failed:
	free_task(task);
	return TASK_FAILED;
}
