#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#ifdef EXTRA_CREDIT
#include <sys/resource.h>
#endif

#include "task.h"
#include "pipeline.h"
#include "job.h"
#include "job_table.h"
#include "signal_handlers.h"
#include "exit_code.h"
#include "debug.h"
#include "metadata.h"

#define SAFE_KILL(pid, sig) \
	if (kill((pid), (sig)) < 0) { \
		perror("Could not send signal to foreground process"); \
	}

static void give_terminal_back_to_smash() {
	if (is_interactive() && tcsetpgrp(STDIN_FILENO, get_smash_pid()) < 0) {
		perror("Could not put smash back into foreground of terminal");
	}
}

static void onSigStop(pid_t pid, JOB *job, void (*onStop)(JOB *)) {
	print_debug_message("Received SIGSTOP signal while waiting for foreground process");
	SAFE_KILL(-pid, SIGTSTP);
	if (onStop) onStop(job);
	sigstop_flag = 0;
	give_terminal_back_to_smash();
}

static void onSigInt(pid_t pid) {
	print_debug_message("Received SIGINT signal while waiting for foreground process");
	SAFE_KILL(-pid, SIGINT);
	puts("");
	sigint_flag = 0;
}

static int handle_child(JOB *job, pid_t pid, int exit_status, void (*onStop)(JOB *)) {
	if (WIFSTOPPED(exit_status)) {
		print_debug_message("Foreground process is stopped, putting in background");
		if (onStop) onStop(job);
		give_terminal_back_to_smash();
		set_exit_code(128 + WSTOPSIG(exit_status));
		return -1;
	}
	int exit_code;
	if (WIFSIGNALED(exit_status)) {
		exit_code = 128 + WTERMSIG(exit_status);
	} else {
		exit_code = WEXITSTATUS(exit_status);
	}
	print_debug_message("ENDED FOREGROUND JOB: %s (ret=%d)",
			job->pipeline->full_command,
			exit_code);
	set_exit_code(exit_code);
	give_terminal_back_to_smash();
#ifdef EXTRA_CREDIT
	finalize_times(job);
	print_times(job);
#endif
	return 0;

}

/* Return -1 on foreground sleeping, 0 for terminating, 1 for child still in foreground */
static int onSigChld(JOB *job, pid_t pid, void (*onStop)(JOB *)) {
	print_debug_message("Received SIGCHLD signal while waiting for foreground process");
	int exit_status;
	pid_t wait_pid_result = waitpid(pid, &exit_status, WNOHANG | WUNTRACED);
	if (wait_pid_result < 0 && errno != ECHILD) {
		perror("waitpid");
	}
	print_debug_message("Tried to reap foreground process. Result: %d", wait_pid_result);
	child_reaper();
	if (wait_pid_result == pid) {
		return handle_child(job, pid, exit_status, onStop);
	}
	return 1;
}

int wait_for_process(JOB *job, sigset_t *oset, void (*onStop)(JOB *)) {
	pid_t pid = job->pid;
	if (is_interactive() && tcsetpgrp(STDIN_FILENO, pid) < 0) {
		perror("Could not put job into foreground on terminal");
	}
	sigint_flag = 0;
	sigstop_flag = 0;
	while (1) {
		/* Want to ensure we don't go into deadlock waiting for a child that */
		/* is already done */
		if (sigchld_flag) goto wait_for_process_skip_suspend;
		print_debug_message("Suspending... waiting for foreground process: %s", job->pipeline->full_command);
		sigsuspend(oset);
wait_for_process_skip_suspend:
		if (sigstop_flag) {
			onSigStop(pid, job, onStop);
			return -1;
		}
		if (sigint_flag) {
			onSigInt(pid);
			/* We still have to wait for the child process to finish, so don't return */
		}
		if (sigchld_flag) {
			int result = onSigChld(job, pid, onStop);
			if (result <= 0) return result;
		}
	}
}
