#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "task.h"
#include "pipeline.h"
#include "job.h"
#include "job_table.h"
#include "signal_handlers.h"
#include "exit_code.h"
#include "debug.h"
#include "metadata.h"

int wait_for_process(JOB *job, sigset_t *oset, void (*onStop)(JOB *)) {
	pid_t pid = job->pid;
	tcsetpgrp(STDIN_FILENO, pid);
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
			print_debug_message("Received SIGSTOP signal while waiting for foreground process");
			kill(-pid, SIGTSTP);
			if (onStop) onStop(job);
			sigstop_flag = 0;
			tcsetpgrp(STDIN_FILENO, get_smash_pid());
			return -1;
		}
		if (sigint_flag) {
			print_debug_message("Received SIGINT signal while waiting for foreground process");
			kill(-pid, SIGINT);
			puts("");
			sigint_flag = 0;
			/* We still have to wait for the child process to finish, so don't return */
		}
		if (sigchld_flag) {
			print_debug_message("Received SIGCHLD signal while waiting for foreground process");
			int exit_status;
			pid_t wait_pid_result = waitpid(pid, &exit_status, WNOHANG | WUNTRACED);
			print_debug_message("Tried to reap foreground process. Result: %d", wait_pid_result);
			child_reaper();
			if (wait_pid_result == pid) {
				if (WIFSTOPPED(exit_status)) {
					print_debug_message("Foreground process is stopped, putting in background");
					if (onStop) onStop(job);
					tcsetpgrp(STDIN_FILENO, get_smash_pid());
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
				tcsetpgrp(STDIN_FILENO, get_smash_pid());
				return 0;
			}
		}
	}
}
