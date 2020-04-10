#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "args.h"

void print_debug_message(char *s, ...) {
	if (!has_d_flag()) return;
	printf("DEBUG: ");
	va_list ap;
	va_start(ap, s);
	vprintf(s, ap);
	va_end(ap);
	puts("");
}
