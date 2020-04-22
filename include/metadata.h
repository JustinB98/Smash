#ifndef METADATA_H
#define METADATA_H

#define PROGRAM_NAME "smash"

void metadata_init(FILE *file);

pid_t get_smash_pid();

int is_interactive();

#endif /* METADATA_H */
