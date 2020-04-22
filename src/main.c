#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#ifdef EXTRA_CREDIT
#include <sys/resource.h>
#endif

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

#if 0
/* Only kept for debugging */
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
#endif

static FILE *open_file_input() {
	FILE *file = fopen(get_file_input(), "r");
	if (file == NULL) {
		fprintf(stderr, "Could not open %s\n", get_file_input());
		exit(EXIT_FAILURE);
	}
	return file;
}

static void init(size_t *n, char **buf) {
	if (!is_interactive()) goto init_data;
	/* Take care of process stuff */
	if (setsid() < 0 && errno != EPERM) {
		/* Not a big deal, but should be noted to user */
		perror("Could not set smash as session leader");
	}
	if (setpgid(getpid(), getpid()) < 0) {
		/* Again, not a big deal. Some features might now work */
		perror("Could not move smash into its own process group");
	}
	if (tcsetpgrp(STDIN_FILENO, getpid()) < 0) {
		perror("Could not give smash terminal access");
	}

init_data:
	/* Init our data structures */
	job_table_init();
	signal_handlers_init();
	*n = getpagesize();
	if (*n < 0) {
		fprintf(stderr, "Could not grab system page size, exiting...");
		exit(EXIT_FAILURE);
	}
	*buf = malloc(*n);
	if (*buf == NULL) {
		fprintf(stderr, "Could not malloc a buffer for input. Exiting...\n");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[], char *env[]) {
	int result = 0;
	process_args(argc, argv);
	FILE *file_input = has_file_input() ? open_file_input() : stdin;
	metadata_init(file_input);
	size_t n;
	char *buf;
	init(&n, &buf);
	PIPELINE *pipeline = NULL;
	int exit_code = 0;
	int exit_smash = 0;
	while (1) {
		if (is_interactive()) {
			printf("smash> ");
			if (fflush(stdout) < 0) {
				/* Worst case, user won't see prompt */
				perror("fflush");
			}
		}
		errno = 0;
		result = get_input(file_input, &buf, &n, child_reaper);
		if (result < 0) {
			exit_code = errno == 0 ? get_exit_code() : EXIT_FAILURE;
			break;
		}
		/* File input needs to be flushed because appearently the new line from it */
		/* messes with reading it */
		if (fflush(file_input) < 0) {
			perror("Could not flush input file");
		}
		pipeline = parse_pipeline(buf);
		if (pipeline == PIPELINE_EMPTY) goto end_of_main_loop;
		else if (pipeline == PIPELINE_FAILED) {
			exit_code = EXIT_FAILURE;
			break;
		}
		if (pipeline->n_pipelines == 1) {
			exit_smash = should_exit(pipeline_get_task(pipeline, 0), &exit_code);
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
	if (!is_interactive()) {
		if (fclose(file_input) < 0) {
			perror("Could not close input file");
			exit_code = EXIT_FAILURE;
		}
	}
	free(buf);
	if (result < 0 && is_interactive()) puts("exit");
	return exit_code;
}
