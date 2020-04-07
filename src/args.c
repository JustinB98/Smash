#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

static char *file_input;
static int d_flag;

void print_usage(FILE *stream) {
	fprintf(stream, "USAGE: smash [-d] <file>\n");
}

void print_usage_and_exit() {
	print_usage(stderr);
	exit(EXIT_FAILURE);
}

static void validate_arguments(int argc, char *argv[]) {
	if (d_flag > 1) {
		fprintf(stderr, "Too many debug flags\n");
		print_usage_and_exit();
	}
	if (optind + 1 == argc) {
		file_input = argv[optind];
	} else if (optind == argc) {
		file_input = NULL;
	} else {
		fprintf(stderr, "Only one file input\n");
		print_usage_and_exit();
	}
}

void process_args(int argc, char **argv) {
	int opt;
#ifdef EXTRA_CREDIT
	char *optstr = "dt";
#else
	char *optstr = "d";
#endif
	while ((opt = getopt(argc, argv, optstr)) != -1) {
		switch (opt) {
			case 'd':
				++d_flag;
				break;
			case '?':
				print_usage_and_exit();
		}
	}
	validate_arguments(argc, argv);
}

int has_file_input() {
	return file_input != NULL;
}

char *get_file_input() {
	return file_input;
}

int has_d_flag() {
	return d_flag;
}
