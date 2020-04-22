#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

static char *file_input;
static int d_flag;

#ifdef EXTRA_CREDIT
static int t_flag;

int has_t_flag() {
	return t_flag;
}
#endif

void print_usage(FILE *stream) {
#ifdef EXTRA_CREDIT
	fprintf(stream, "USAGE: smash [-dt] <file>\n");
#else
	fprintf(stream, "USAGE: smash [-d] <file>\n");
#endif
}

void print_usage_and_exit(int err) {
	FILE *output_file = err ? stderr : stdout;
	print_usage(output_file);
	exit(err);
}

static void validate_arguments(int argc, char *argv[]) {
	if (d_flag > 1) {
		fprintf(stderr, "Too many debug flags\n");
		print_usage_and_exit(1);
	}
#ifdef EXTRA_CREDIT
	if (t_flag > 1) {
		fprintf(stderr, "Too many time flags\n");
		print_usage_and_exit(1);
	}
#endif
	if (optind + 1 == argc) {
		file_input = argv[optind];
	} else if (optind == argc) {
		file_input = NULL;
	} else {
		fprintf(stderr, "Only one file input\n");
		print_usage_and_exit(1);
	}
}

void process_args(int argc, char **argv) {
	int opt;
#ifdef EXTRA_CREDIT
	char *optstr = "hdt";
#else
	char *optstr = "hd";
#endif
	while ((opt = getopt(argc, argv, optstr)) != -1) {
		switch (opt) {
			case 'd':
				++d_flag;
				break;
#ifdef EXTRA_CREDIT
			case 't':
				++t_flag;
				break;
#endif
			case 'h':
				print_usage_and_exit(0);
			case '?':
				print_usage_and_exit(1);
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
