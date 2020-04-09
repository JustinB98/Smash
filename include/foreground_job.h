#ifndef FOREGROUND_JOB_H
#define FOREGROUND_JOB_H

/*
 * Waits for a specified process to stop or calls a specified function
 * if the smash is sent a SIGTSTP signal, or just ends
 * the process if smash is sent a SIGINT signal.
 * This function should be called in conjunction with sigprocmask(2)
 *
 * @param job		job to send to job table if SIGTSTP is sent
 * @param oset		Set of signals to pass into sigsuspend
 * @param onStop	Function to run if SIGTSTP was signaled to the current process
 * @return			0 if the process was reaped
 *					-1 if the process was sent to the job table
 */
int wait_for_process(JOB *job, sigset_t *oset, void (*onStop)(JOB *));

#endif /* FOREGROUND_JOB_H */
