#ifndef SIGNAL_HANDLERS_H
#define SIGNAL_HANDLERS_H

volatile sig_atomic_t sigint_flag, sigstop_flag, sigchld_flag;

void install_signal_handler(int sig, void (*handler)(int));

void signal_handlers_init();

/*
 * Function that looks at sigchld_flag and will reap any child process
 */
void child_reaper();

#endif /* SIGNAL_HANDLERS_H */
