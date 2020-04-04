#ifndef INPUT_H
#define INPUT_H

int get_input(FILE *fstream, char **malloced_ptr, size_t *n, void (*sig_handler)());

#endif /* INPUT_H */
