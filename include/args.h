#ifndef ARGS_H
#define ARGS_H

/**
 * Process command line arguments. Will exit if any errors are seen
 *
 * @param argc	argment count from main
 * @param argv	argument vector from main
 */
void process_args(int argc, char *argv[]);

/**
 * @return True if file input was present
 */
int has_file_input();

/**
 * @return file input from argv
 */
char *get_file_input();

/**
 * @return True if -d flag was present
 */
int has_d_flag();

#ifdef EXTRA_CREDIT
/**
 * @return True if -t flag was present
 */
int has_t_flag();
#endif

#endif /* ARGS_H */
