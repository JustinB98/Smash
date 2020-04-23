#ifndef SIGNAL_HANDLERS_H
#define SIGNAL_HANDLERS_H

/* Signal flags that will be set in handlers */
volatile sig_atomic_t sigint_flag, sigstop_flag, sigchld_flag;

/**
 * Installs a signal handler for the current process using sigaction(2)
 * Used in a similar way to signal(2)
 * Exits with EXIT_FAILURE if any error occurs
 *
 * @param sig		Signal to set handler for
 * @param handler	Function callback for handler
 */
void install_signal_handler(int sig, void (*handler)(int));

/**
 * Installs signal handlers for the relevent signals for a shell
 * Signals such as SIGCHLD, SIGTSTP, SIGTTIN, SIGTTOU, SIGINT
 */
void signal_handlers_init();

/*
 * Function that looks at sigchld_flag and will reap any child process
 * and then attempt to update the job in the job table
 */
void child_reaper();

#endif /* SIGNAL_HANDLERS_H */
