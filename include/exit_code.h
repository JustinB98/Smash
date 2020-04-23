#ifndef EXIT_CODE_H
#define EXIT_CODE_H

/**
 * @return The last foreground process's exit code
 */
int get_exit_code();

/**
 * Sets the exit code of the just finished foreground process
 * @param new_code	New exit code of the just finished foreground process
 */
void set_exit_code(int new_code);

/**
 * Sets error code to EXIT_FAILURE
 */
void set_exit_code_failure();

/**
 * Sets the error code to EXIT_SUCESS
 */
void set_exit_code_success();

#endif /* EXIT_CODE_H */
