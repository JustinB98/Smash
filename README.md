# Smash

```
Smash/
├── src/
│   ├── task.c
│   ├── smash_commands.c
│   ├── signal_handlers.c
│   ├── queue.c
│   ├── pipeline.c
│   ├── parser.c
│   ├── metadata.c
│   ├── main.c
│   ├── linked_list.c
│   ├── job_table.c
│   ├── job.c
│   ├── input.c
│   ├── hashtable.c
│   ├── foreground_job.c
│   ├── exit_code.c
│   ├── debug.c
│   └── args.c
├── shell_tests/
│   ├── reg_tests/
│   │   ├── test04.sh
│   │   ├── test03.sh
│   │   ├── test02.sh
│   │   └── test01.sh
│   ├── finish.sh
│   ├── ec_tests/
│   │   ├── test06.sh
│   │   └── test05.sh
│   ├── assert_unsuccessful_exit.sh
│   ├── assert_successful_exit.sh
│   └── assert_exit_code.sh
├── include/
│   ├── task.h
│   ├── smash_commands.h
│   ├── signal_handlers.h
│   ├── queue.h
│   ├── pipeline.h
│   ├── parser.h
│   ├── metadata.h
│   ├── linked_list.h
│   ├── job_table.h
│   ├── job.h
│   ├── input.h
│   ├── hashtable.h
│   ├── foreground_job.h
│   ├── exit_code.h
│   ├── debug.h
│   └── args.h
├── README
└── Makefile
```

# File Structure
## src
* `task.c` - Accessors for the Task struct
* `smash_commands.c` - Functions that deal with builtin commands
* `signal_handlers.c` - Module that sets up signal handlers and maintains flags, as well as having a function that reap children processes and attempts to update them in the job table
* `queue.c` - Priority queue that is built like as linked list in order to maintain job numbers
* `pipeline.c` - Functions that deal with the Pipeline struct
* `parser.c` - Module that contains a parse function to parse user input and will return a pipeline struct
* `metadata.c` - Module that contains smash's original pid as well as if it's running in interactive mode. Must be initialized.
* `main.c` - Contains main function that initializes smash as well as the loop that continuously reads input from the input file
* `linked_list.c` - Basic implementation of a doubly linked list with an int as a key (to hold a job id and process id)
* `job_table.c` - Function for inserting jobs into the table as well as updating their status (Only used for background jobs)
* `job.c` - Starting a job with fork and waiting for it if it's in the background
* `input.c` - Waits for user input using `pselect(2)` and `getline(3)` to ensure no job is a zombie for too long
* `hashtable.c` - Basic hashtable implementation that uses a linked list to deal with collisions
* `foreground_job.c` - Handles foreground jobs as well as reaping them or detecting if they're stopped and put in the background
* `exit_code.c` - Keeps track of the exit code for the last foreground process
* `debug.c` - For printing debug messages
* `args.c` - For processing command line arguments and exiting if there are any invalid arguments

## include
* `task.h` - Contains the task structure
* `smash_commands.h` - Functionst that deal with builtin commands
* `signal_handlers.h` - Holds the flags of any signal being handled (SIGINT, SIGTSTP and SIGCHLD) and keeps the flags global
* `queue.h` - Contains the queue structure
* `pipeline.h` - Contains the pipeline structure
* `parser.h` - Module that contains a parse function to parse user input and will return a pipeline struct
* `metadata.h` - Contains functions for getting smash's pid or seeing if smash is in interactive mode
* `linked_list.h` - Contains the linked list struct
* `job_table.h` - Contains the functions on accessing the job table
* `job.h` - Contains the function to start the pipeline
* `input.h` - Waits for user input using `pselect(2)` and `getline(3)` to ensure no job is a zombie for too long
* `hashtable.h` - Basic hashtable implementation that uses a linked list to deal with collisions
* `foreground_job.h` - Handles foreground jobs as well as reaping them or detecting if they're stopped and put in the background
* `exit_code.h` - Contains functions to set the exit code or to set it to `EXIT_SUCESSS` or `EXIT_FAILURE`
* `debug.h` - For printing debug messages
* `args.h` - Contains the function to starting process the command line args and accessors for flags passed

## shell_tests
* `reg_tests/` - Tests for running the regular program
  * `test01.sh` - Testing smash's builtin commands
  * `test02.sh` - General Tests with smash such as using ls and wc
  * `test03.sh` - Testing command line arguments
  * `test04.sh` - Testing file redirection with smash
* `ec_tests/` - Tests for running the extra credit program
  * `test05.sh` - Piping Tests
  * `test06.sh` - Globbing Tests
* `finish.sh` - Prints the results of the test
* `assert_unsuccessful_exit.sh` - Asserts that the given exit code is equal to the last command's exit code
* `assert_successful_exit.sh` - Asserts the last command was successful (exit code == 0)
* `assert_unsuccessful_exit.sh` - Asserts the last command was unsuccessful (exit code != 0)

# Makefile Rules
* `all` - Builds the smash program
* `clean` - Removes the build directory (all `.o` and `.d` files) as well as the smash program
* `ec` - Compiles each file with `-DEXTRA_CREDIT`
* `tests` - Recompiles smash and runs the regular tests. Then recompiles with `-DEXTRA_CREDIT` and runs the extra credit tests
* `run_reg_test` - Just run the regular tests without compiling
* `run_ec_tests` - Run the extra credit tests without compiling

## Makefile Notes
To run just the regular tests and compile, run one of the two:
1. `make clean all run_reg_tests`
2. `make clean run_reg_tests`

To run just the extra credit tests, run one of the two:
1. `make clean ec run_ec_tests`
2. `make clean run_ec_tests`

# Parsing Rules:
* Input
  * Input can be as long as `getline(3)` permits (using `realloc(3)`)
* Redirection
  * has to be in the form of `>file` `<file` or `2>file`
  * It must be attached to the symbol
  * It can be anywhere in the input. For example, for the input ls >output.txt / , ls / will run and the output will go to output.txt. This goes for the rest of the redirection symbols
  * If the same redirection symbol is repeated twice, the later one will be used and the previous will be overwritten

* Background jobs
  * A job can be put in the background by attaching & as a separate argument at the end of the input
  * If a command such as ls & / was given, ls would receieve & and / as arguments

* Spaces are trimmed

# Parsing Rules for Extra Credit

* Redirection
  * Similar to the regular program except it is valid inside a pipeline. For example:
```
ls -al >output | grep a
```
Will write ls output to the file "output" and grep will read from a closed stdin and exit with 1 because grep found nothing (It will not exit with 2 since there should be no I/O errors)

* Background jobs
  * A background job is only honored if the & is the very last argument of the pipeline. For example:
```
ls -al & | grep c
```

is the same as:

```
ls -al | grep c
```

* Empty Pipelines
  * Empty Pipelines are ignored
    * `|||` is ignored
	* `ls -al || ||| grep c |||` is the same as `ls -al | grep c`

* Globbing
  * `glob(3)` was not used in this project
  * Globbing only works in the current directory (`src/*.c` would not work)
  * If the extension could not be found, then the original string would be used. For example, if `*.h` did not match anything, then the argument will remain at `*.h`.

# Background jobs 
* Background jobs will be notified to the user after the next command they type. This is to avoid problems with too many things being displayed to stdout at once. A job will be removed from the table and have its memory freed only if the user presses enter or runs another command. Running the "jobs" command will print the finished jobs if they are finished and remove them.
* If a background program request stdin, they will be sent a `SIGTTIN` signal. This is due to the nature of `tcsetpgrp(3)`. This also means if the program is ran in batch mode, background jobs may not work properly, although still possible
* Background jobs are stored in two hashtables. One hashtable is to look up given a pid and the other from the jobid
* Jobids are generated using a priority queue that acts like bash

# Pipelines
Pipelines for smash work in the follow way:
1. Fork a child. This child will manage the pipeline
2. Have the child move itself into its own process group. Have the parent and child do this to avoid race conditions. Also `tcsetpgrp(3)` for both parent and child if the pipeline is in the foreground
3. Have the child loop through the tasks and manage `dup(2)`. Fork new children to run each task. Have the children have the same process group.
4. Once all tasks have been given a new process to run on, wait for all of them to finish, then exit with the exit code of the process that was started last

# Timing
* If the program is the extra credit program and is ran with the `-t` flag, the times will be displayed after the job ran if it was a foreground job, or along with the status if the job was a background job.
* If getting the times fails, (`gettimeofday(2)` fails or `getrusage(2)` fails) then a message will be printed saying that the times could not been seen.
* Times may be slightly off because the times are started when the process gets forked, so if it takes time to run the pipeline.

# Other Notes
* `cd` by itself will go to the directory `$HOME`
* echo and pwd are implemented as builtins. The `/bin` programs can only be used with `/bin/echo` and `/bin/pwd`
* Running a builtin command in the background is not possible. It will be ignored
* Running a builtin command that deals with the job table (such as `jobs`, `fg` and `bg`) in a pipeline will be ignored and prints a message saying job control not possible
