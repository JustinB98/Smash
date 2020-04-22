#ifndef ARGS_H
#define ARGS_H

void process_args(int argc, char *argv[]);

int has_file_input();

char *get_file_input();

int has_d_flag();

#ifdef EXTRA_CREDIT
int has_t_flag();
#endif

#endif /* ARGS_H */
