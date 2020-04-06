#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "input.h"
#include "task.h"
#include "job.h"
#include "exit_code.h"

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

int main(int argc, const char *argv[], char *env[]) {
	size_t n = getpagesize();
	char *buf = malloc(n);
	int result = 0;
	exit_code = 0;
	while (1) {
		printf("smash> ");
		fflush(stdout);
		result = get_input(stdin, &buf, &n, NULL);
		if (result < 0) break;
		TASK *task = parse_task(buf);
		if (task == TASK_EMPTY) continue;
		else if (task == TASK_FAILED) break;
		start_task(task, env);
		free_task(task);
	}
	free(buf);
	if (result < 0) puts("");
	return result < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
