#include <stdlib.h>

static int exit_code = 0;

int get_exit_code() {
	return exit_code;
}

void set_exit_code(int new_code) {
	exit_code = new_code;
}

void set_exit_code_failure() {
	set_exit_code(EXIT_FAILURE);
}

void set_exit_code_success() {
	set_exit_code(EXIT_SUCCESS);
}
