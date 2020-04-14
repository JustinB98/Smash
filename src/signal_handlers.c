#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#include "task.h"
#include "pipeline.h"
#include "signal_handlers.h"
#include "job_table.h"

static void set_up_sigaction(struct sigaction *action, void (*handler)(int)) {
	action->sa_handler = handler;
	sigemptyset(&action->sa_mask); /* Don't want to block any signal by default */
	action->sa_flags = SA_RESTART;
}

void install_signal_handler(int sig, void (*handler)(int)) {
	struct sigaction action;
	set_up_sigaction(&action, handler);
	int result = sigaction(sig, &action, NULL); /* Don't really care about the old value */
	if (result < 0) {
		fprintf(stderr, "Could not install signal handler for %d: %s\n", sig, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

static void sigint_handler(int signum) {
	sigint_flag = 1;
}

static void sigstop_handler(int signum) {
	sigstop_flag = 1;
}

static void sigchld_handler(int signum) {
	sigchld_flag = 1;
}

static void install_signal_handlers() {
	install_signal_handler(SIGINT, sigint_handler);
	install_signal_handler(SIGTSTP, sigstop_handler);
	install_signal_handler(SIGCHLD, sigchld_handler);
}

void signal_handlers_init() {
	install_signal_handlers();
	/* TODO anything else? */
}

void reap_children() {
	pid_t rpid;
	int wstatus;
	while ((rpid = waitpid(-1, &wstatus, WNOHANG | WUNTRACED | WCONTINUED)) > 0) {
		if (WIFSIGNALED(wstatus)) {
			job_table_mark_as_aborted(rpid, 128 + WTERMSIG(wstatus));
		} else if (WIFEXITED(wstatus) || WIFSIGNALED(wstatus)) {
			job_table_mark_as_done(rpid, WEXITSTATUS(wstatus));
		} else if (WIFCONTINUED(wstatus)) {
			job_table_change_status(rpid, RUNNING);
		} else if (WIFSTOPPED(wstatus)) {
			job_table_change_status(rpid, STOPPED);
		}
	}
}

void child_reaper() {
	if (sigchld_flag) {
		reap_children();
		sigchld_flag = 0;
	}
}
