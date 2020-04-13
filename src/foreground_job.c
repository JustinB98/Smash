#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "job.h"
#include "job_table.h"
#include "signal_handlers.h"
#include "exit_code.h"
#include "debug.h"

int wait_for_process(JOB *job, sigset_t *oset, void (*onStop)(JOB *)) {
	pid_t pid = job->pid;
	sigint_flag = 0;
	sigstop_flag = 0;
	while (1) {
		/* Want to ensure we don't go into deadlock waiting for a child that */
		/* is already done */
		if (sigchld_flag) goto wait_for_process_skip_suspend;
		sigsuspend(oset);
wait_for_process_skip_suspend:
		if (sigstop_flag) {
			killpg(pid, SIGSTOP);
			if (onStop) onStop(job);
			sigstop_flag = 0;
			return -1;
		}
		if (sigint_flag) {
			killpg(pid, SIGKILL);
			puts("");
			sigint_flag = 0;
		}
		int exit_status;
		pid_t wait_pid_result = waitpid(pid, &exit_status, WNOHANG);
		child_reaper();
		if (wait_pid_result == pid) {
			int exit_code = WEXITSTATUS(exit_status);
			print_debug_message("ENDED FOREGROUND JOB: %s (ret=%d)",
								job->task->full_command,
								exit_code);
			set_exit_code(exit_code);
			return 0;
		}
	}
}
