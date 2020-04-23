#ifndef SMASH_RUNNER_H
#define SMASH_RUNNER_H

#ifdef EXTRA_CREDIT
/**
 * Runs pipeline. Never returns. Only call from child process
 *
 * @param pipeline	Pipeline to run
 */
void child_process_start_job(PIPELINE *pipeline);
#else
/**
 * Runs pipeline. Never returns. Only call from child process
 *
 * @param task	task to run
 */
void child_process_start_job(TASK *task);
#endif

#endif /* SMASH_RUNNER_H */
