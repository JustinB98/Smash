#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <pwd.h>

#include "task.h"
#include "exit_code.h"

static int smash_exit(TASK *task) {
	if (task->n_words != 1) { 
		fprintf(stderr, "smash: Too many arguments for exit\nUSAGE: exit\n");
		set_exit_code_failure();
		return -1;
	} else return 1;
}

static int smash_cd(TASK *task) {
	if (task->n_words > 2) {
		fprintf(stderr, "smash: Too many arguments for cd\nUSAGE: cd [PATH]\n");
		set_exit_code_failure();
		return -1;
	}
	char *path = NULL;
	if (task->n_words == 2) {
		path = task->word_list->next->word;
	} else if (task->n_words == 1) {
		path = getenv("HOME");
	}
	int result = chdir(path);
	if (result < 0) {
		fprintf(stderr, "Could not find %s\n", path);
		set_exit_code_failure();
		return -1;
	}
	set_exit_code_success();
	return 1;
}

static int smash_pwd(TASK *task) {
	if (task->n_words > 1) {
		fprintf(stderr, "smash: Too many arguments for pwd\nUSAGE: pwd\n");
		set_exit_code_failure();
		return -1;
	}
	char wd[PATH_MAX];
	char *result = getcwd(wd, PATH_MAX);
	if (result == NULL) {
		fprintf(stderr, "smash: Could not get current working directory\n");
		set_exit_code_failure();
		return -1;
	}
	puts(wd);
	set_exit_code_success();
	return 1;
}

int execute_smash_command(TASK *task) {
	char *cmd = task->word_list->word;
	if (!strcmp(cmd, "cd")) return smash_cd(task);
	else if (!strcmp(cmd, "pwd")) return smash_pwd(task);
	return 0;
}

int should_exit(TASK *task) {
	char *cmd = task->word_list->word;
	if (!strcmp(cmd, "exit")) {
		return smash_exit(task);
	} else return 0;
}
