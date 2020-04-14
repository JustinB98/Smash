#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#include "input.h"
#include "task.h"
#include "pipeline.h"
#include "job.h"
#include "exit_code.h"
#include "signal_handlers.h"
#include "smash_commands.h"
#include "args.h"
#include "job_table.h"
#include "debug.h"
#include "parser.h"
#include "metadata.h"

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
	metadata_init();
	job_table_init();
	size_t n = getpagesize();
	char *buf = malloc(n);
	int result = 0;
	signal_handlers_init();
	PIPELINE *pipeline = NULL;
	FILE *file_input = has_file_input() ? open_file_input() : stdin;
	while (1) {
		if (file_input == stdin) {
			printf("smash> ");
			fflush(stdout);
		}
		result = get_input(file_input, &buf, &n, child_reaper);
		/* File input needs to be flushed because appearently the new line from it */
		/* messes with reading it */
		fflush(file_input);
		if (result < 0) break;
		pipeline = parse_pipeline(buf);
		if (pipeline == PIPELINE_EMPTY) goto end_of_main_loop;
		else if (pipeline == PIPELINE_FAILED) break;
		int exit_smash = 0;
		if (pipeline->n_pipelines == 1) {
			exit_smash = should_exit(pipeline_get_task(pipeline, 0));
		}
		if (exit_smash > 0) {
			free_pipeline(pipeline);
			break;
		}
		else if (exit_smash == 0) {
			start_pipeline(pipeline, env);
		}
end_of_main_loop:
		print_and_remove_finished_jobs();
	}
	job_table_fini();
	if (file_input != stdin) fclose(file_input);
	free(buf);
	if (result < 0 && file_input == stdin) puts("exit");
	return EXIT_SUCCESS;
}
