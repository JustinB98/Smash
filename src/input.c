#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/select.h>

static void run_signal_handler(void (*sig_handler)()) {
	if (sig_handler) {
		sig_handler();
	}
}

int get_input(FILE *fstream, char **malloced_ptr, size_t *n, void (*sig_handler)()) {
	int fd = fileno(fstream);
	sigset_t set, oset;
	sigfillset(&set);
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);
	sigprocmask(SIG_SETMASK, &set, &oset);
	run_signal_handler(sig_handler);
	while (1) {
		int result = pselect(fd + 1, &rfds, NULL, NULL, NULL, &oset);
		if (result < 0 && errno == EINTR) run_signal_handler(sig_handler);
		else if (result < 0) return result;
		else break; /* ready for input */
	}
	sigprocmask(SIG_SETMASK, &oset, NULL);
	if (getline(malloced_ptr, n, fstream) < 0) return -1;
	sigprocmask(SIG_SETMASK, &oset, NULL);
	run_signal_handler(sig_handler);
	return 0;
}
