#ifndef SMASH_COMMANDS_H
#define SMASH_COMMANDS_H
#include "task.h"

/*
 * Executes a command specific to smash
 * @param task	task to execute if it's a smash command
 * @return		1 if task was a smash command
 *				0 if task was not a smash command
 *				-1 if task was a smash command and that command ran into trouble
 */
int execute_smash_command(TASK *task);

/*
 * Determines if smash should be exited
 * @param task	Task to be ran (possibly not exit)
 * @return		1 if task was exit task
 *				0 if task was not exit task
 *				-1 if task was an exit task, but had incorrect arguments
 */
int should_exit(TASK *task);

#endif /* SMASH_COMMANDS_H */
