#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "task.h"
#include "exit_code.h"

#ifdef EXTRA_CREDIT
#include <dirent.h>
#include <sys/types.h>
#include <limits.h>
#include <pwd.h>
#include <unistd.h>
#endif

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

static int allocate_task_variables(TASK **task, char **string_to_parse) {
	*task = malloc(sizeof(TASK));
	if (*task == NULL) goto free_task;
	TASK *task_ptr = *task;
	task_ptr->stdin_path = NULL;
	task_ptr->stdout_path = NULL;
	task_ptr->stderr_path = NULL;
	*string_to_parse = strdup(*string_to_parse);
	if (string_to_parse == NULL) goto free_string_to_parse;
	return 0;
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

static char *get_env_var(char *word) {
	if (!strcmp(word, "$?")) return get_exit_code_str();
	char *var = getenv(word + 1);
	var = var == NULL ? "" : var;
	return strdup(var);
}

#ifdef EXTRA_CREDIT
static WORD_LIST **fill_glob(WORD_LIST **word_list, char *word, size_t *n_words) {
	size_t original_n_words = *n_words;
	char *extension = word + 1;
	char wd[PATH_MAX];
	char *current_directory_str = getcwd(wd, PATH_MAX);
	if (current_directory_str == NULL) {
		perror("Could not open current directory");
		return word_list;
	}
	DIR *current_directory = opendir(current_directory_str);
	if (current_directory == NULL) {
		perror("Could not open current directly");
		return word_list;
	}
	while (1) {
		errno = 0;
		struct dirent *entry = readdir(current_directory);
		if (entry == NULL) {
			if (errno != 0) perror("Could not open entry in current directory");
			break;
		}
		if (strstr(entry->d_name, extension) == NULL) continue;
		*word_list = malloc(sizeof(WORD_LIST));
		if (*word_list == NULL) {
			fprintf(stderr, "Could not malloc a word list\n");
			return NULL;
		}
		(*word_list)->next = NULL;
		(*word_list)->word = strdup(entry->d_name);
		word_list = &((*word_list)->next);
		++(*n_words);
	}
	closedir(current_directory);
	if (original_n_words == *n_words) {
		*word_list = malloc(sizeof(WORD_LIST));
		if (*word_list == NULL) {
			fprintf(stderr, "Could not malloc a word list\n");
			return NULL;
		}
		(*word_list)->next = NULL;
		(*word_list)->word = strdup(word);
		word_list = &((*word_list)->next);
		++(*n_words);
	}
	return word_list;
}

static int handle_glob(WORD_LIST ***word_list, char *word, size_t *n_words) {
	if (word[0] == '*' && word[1] == '.') {
		*word_list = fill_glob(*word_list, word, n_words);
		return 1;
	}
	return 0;
}

void free_pipeline(PIPELINE *pipeline) {
	free(pipeline->full_command);
	PIPELINE_LIST *current = pipeline->list;
	while (current) {
		PIPELINE *prev = pipeline;
		free_task(current->task);
		current = current->next;
		free(prev);
	}
}

PIPELINE *parse_pipeline(char *string_to_parse) {
	char *s = string_to_parse;
	char *token = NULL;
	PIPELINE *pipeline = malloc(sizeof(PIPELINE));
	pipeline->list = NULL;
	PIPELINE_LIST **list = &pipeline->list;
	if (pipeline == NULL) goto parse_pipeline_finish;
	pipeline->full_command = strdup(string_to_parse);
	if (pipeline->full_command == NULL) goto parse_pipeline_failed;
	while ((token = strtok_r(s, "|", &s))) {
		TASK *task = parse_task(token);
		if (task == TASK_EMPTY) continue;
		else if (task == TASK_FAILED) goto parse_pipeline_failed;
		*list = malloc(sizeof(PIPELINE_LIST));
		if (*list == NULL) goto parse_pipeline_failed;
		(*list)->next = NULL;
		(*list)->task = task;
		list = &((*list)->next);
	}
	if (pipeline->list == NULL) {
		free_pipeline(pipeline);
		return PIPELINE_EMPTY;
	}
	goto parse_pipeline_finish;
parse_pipeline_failed:
		free_pipeline(pipeline);
		pipeline = PIPELINE_FAILED;
parse_pipeline_finish:
	return pipeline;
}

#endif

static char *process_word(char *word) {
	if (*word == '$') {
		return get_env_var(word);
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
	WORD_LIST **word_list;
	char *s = string_to_parse;
	if (allocate_task_variables(&task, &string_to_parse)) return TASK_FAILED;
	task->full_command = string_to_parse;
	word_list = &(task->word_list);
	*word_list = NULL;
	size_t n_words = 0;
	char *token = NULL;
	char *last_token = NULL;
	while ((token = strtok_r(s, " ", &s))) {
		if (*token == '\n') continue;
		last_token = token;
		if (*word_list == NULL && token[0] == '#') break;
		if (handle_redirection(task, token) > 0) continue;
#ifdef EXTRA_CREDIT
		if (handle_glob(&word_list, token, &n_words)) {
			if (word_list == NULL) goto parse_task_failed;
			continue;
		}
#endif
		*word_list = malloc(sizeof(WORD_LIST));
		if (*word_list == NULL) goto parse_task_failed;
		char *word = process_word(token);
		if (word == NULL) goto parse_task_failed;
		(*word_list)->word = word;
		(*word_list)->next = NULL;
		word_list = &((*word_list)->next);
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
	if (task->word_list == NULL) {
		/* nothing entered */
		free_task(task);
		return TASK_EMPTY;
	} else {
		return task;
	}
parse_task_failed:
	free_task(task);
	return TASK_FAILED;
}
