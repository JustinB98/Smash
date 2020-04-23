#ifndef METADATA_H
#define METADATA_H

#define PROGRAM_NAME "smash"

/**
 * Initializes metadata which includes getting the current pid
 * and setting input file
 *
 * @param file	Input file that smash will be reading from
 */
void metadata_init(FILE *file);

/**
 * @return	Smash's pid, which should be the same as smash's pgid
 */
pid_t get_smash_pid();

/**
 * @return	1 if the input file is stdin, 0 otherwise
 */
int is_interactive();

#endif /* METADATA_H */
