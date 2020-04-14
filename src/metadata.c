#include <sys/types.h>
#include <unistd.h>

static pid_t smash_pid;

void metadata_init() {
	smash_pid = getpid();
}

pid_t get_smash_pid() {
	return smash_pid;
}
