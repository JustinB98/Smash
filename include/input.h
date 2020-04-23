#ifndef INPUT_H
#define INPUT_H

/**
 * Function to grab input in a signal safe way using pselect(2). The malloced_ptr and
 * n passed in will be possibly updated via getline(3)
 *
 * @param fstream		File to be read from
 * @param malloced_ptr	Pointer to the variable to store the results in
 * @param n				Pointer to the varable used to store the result of n
 *
 * @return				-1 if something went wrong with getline(3) or pselect(2)
 *						 0 if successful
 */
int get_input(FILE *fstream, char **malloced_ptr, size_t *n, void (*sig_handler)());

#endif /* INPUT_H */
