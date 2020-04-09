#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "input.h"
#include "task.h"
#include "job.h"
#include "exit_code.h"
#include "signal_handlers.h"
#include "smash_commands.h"
#include "args.h"
#include "job_table.h"

void print_task_info(TASK *task) {
	WORD_LIST *list = task->word_list;
	while (list) {
		printf("%s ", list->word);
		list = list->next;
	}
	printf("\n");
	printf("stdin: %s\n", task->stdin_path);
	printf("stdout: %s\n", task->stdout_path);
	printf("stderr: %s\n", task->stderr_path);
}

static FILE *open_file_input() {
	FILE *file = fopen(get_file_input(), "r");
	if (file == NULL) {
		fprintf(stderr, "Could not open %s\n", get_file_input());
		exit(EXIT_FAILURE);
	}
	return file;
}

int main(int argc, char *argv[], char *env[]) {
	process_args(argc, argv);
	job_table_init();
	size_t n = getpagesize();
	char *buf = malloc(n);
	int result = 0;
	signal_handlers_init();
	TASK *task = NULL;
	FILE *file_input = has_file_input() ? open_file_input() : stdin;
	while (1) {
		if (file_input == stdin) {
			printf("smash> ");
			fflush(stdout);
		}
		result = get_input(file_input, &buf, &n, child_reaper);
		fflush(file_input);
		if (result < 0) break;
		task = parse_task(buf);
		if (task == TASK_EMPTY) continue;
		else if (task == TASK_FAILED) break;
		int exit_smash = should_exit(task);
		if (exit_smash > 0) {
			free_task(task);
			break;
		}
		else if (exit_smash == 0) {
			start_task(task, env);
		}
	}
	job_table_fini();
	if (file_input != stdin) fclose(file_input);
	free(buf);
	if (result < 0 && file_input == stdin) puts("");
	return EXIT_SUCCESS;
}
