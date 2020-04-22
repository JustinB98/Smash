#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

static pid_t smash_pid;
static int interactive;

void metadata_init(FILE *file) {
	smash_pid = getpid();
	interactive = file == stdin;
}

pid_t get_smash_pid() {
	return smash_pid;
}

int is_interactive() {
	return interactive;
}

