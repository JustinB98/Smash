#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "input.h"
#include "task.h"

int main(int argc, const char *argv[], const char *env[]) {
	size_t n = getpagesize();
	char *buf = malloc(n);
	int result = 0;
	while (1) {
		printf("smash> ");
		fflush(stdout);
		result = get_input(stdin, &buf, &n, NULL);
		if (result < 0) break;
		TASK *task = parse_task(buf);
		if (task == TASK_EMPTY) continue;
		else if (task == TASK_FAILED) break;
		printf("Successful: %p\n", task);
		printf("Input: %s\n", buf);
		WORD_LIST *list = task->word_list;
		while (list) {
			printf("%s ", list->word);
			list = list->next;
		}
		printf("\n");
		printf("stdin: %s\n", task->stdin_path);
		printf("stdout: %s\n", task->stdout_path);
		printf("stderr: %s\n", task->stderr_path);
		free_task(task);
	}
	free(buf);
	if (result < 0) puts("");
	return result < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
