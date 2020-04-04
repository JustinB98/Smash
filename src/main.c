#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "input.h"

int main(int argc, const char *argv[], const char *env[]) {
	size_t n = getpagesize();
	char *buf = malloc(n);
	int result = 0;
	while (1) {
		printf("smash> ");
		fflush(stdout);
		result = get_input(stdin, &buf, &n, NULL);
		if (result < 0) break;
		printf("Input: %s", buf);
	}
	free(buf);
	if (result < 0) puts("");
	return result < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
