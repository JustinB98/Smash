#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "args.h"

void print_debug_message(char *s, ...) {
	if (!has_d_flag()) return;
	fprintf(stderr, "DEBUG: ");
	va_list ap;
	va_start(ap, s);
	vfprintf(stderr, s, ap);
	va_end(ap);
	fprintf(stderr, "\n");
}
