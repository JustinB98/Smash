#ifndef SIGNAL_HANDLERS_H
#define SIGNAL_HANDLERS_H
#include <signal.h>

volatile sig_atomic_t sigint_flag, sigstop_flag, sigchld_flag;

void signal_handlers_init();

/*
 * Function that looks at sigchld_flag and will reap any child process
 */
void child_reaper();

#endif /* SIGNAL_HANDLERS_H */
