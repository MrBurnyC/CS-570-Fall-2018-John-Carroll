/*
 *  Program #4
 *  Anas Khafagi
 *  cssc0043
 *  John Carroll
 *  CS570
 *  11-27-2017
 *  Due: 11-29-2017
 *  Program 4: acts a simple unix shell interpreter, it reads the commands from stdin unless the input is redirected
 *  parses them and returns the appropriate command output. Prints the output to stdout unless the output is redirected.
 *  This shell handles most of the meta-characters {'<','>','&','|',';','','','>!'}. This shell handles up to to ten pipe
 *  commands. This shell can handle both input and output redirection simultanously.
 *  This shell uses getword.c to get the word from stdin and parse into a new string array, of commands and flags.
 *  This array is then passed to the system call "execvp" to execute that command.
 *  This shell employs many system calls including but not limited to {"dup2", "execvp","fork",'pipe", "access", "stat"}.
 */

#ifndef PROGRAM2_P2_H
#define PROGRAM2_P2_H

#include <stdio.h>

#include <signal.h>
#include <stdio.h>
#include <zconf.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include "getword.h"

/* exit codes and useful definitions used throughout the shell */
#define NESTED_PIPELINE_PIPE_FAILED         -34
#define PIPE_INPUT_REDIRECT_FAILED          -33
#define PIPE_OUTPUT_REDIRECT_FAILED         -32
#define FORK_FAILED_EXIT_CODE               -31
#define EXEC_FAILED_EXIT_CODE               -30
#define PIPE_FAILED_EXIT_CODE               -29
#define CD_INVALID_NUM_ARGS                 -28
#define CD_FAIL                             -27
#define LS_F_CD_RESTORE_FAIL                -26
#define LS_F_CD_FAIL                        -25
#define LS_F_DIRECTORY_DOESNT_EXIST         -24
#define TRAVERSE_STAT_FAIL                  -23
#define STAT_FAIL                           -22
#define PERMISSION_DENIED                   -21
#define DIRECTORY_OPEN_ERROR                -20
#define INPUT_FILE_OPEN_ERROR               -19
#define INPUT_REDIRECTION_FAILED            -18
#define OUTPUT_REDIRECTION_FAILED           -17
#define OUTPUT_FILE_OPEN_ERROR              -16
#define REDIRECTION_FILE_EXISTS             -15
#define REDIRECTION_ERROR                   -14
#define AMBIGUOUS_INPUT_REDIRECTION         -13
#define AMBIGUOUS_OUTPUT_REDIRECTION        -12
#define MULTIPLE_PIPELINES                  -11
#define NO_PROCESS_AFTER_PIPE               -10
#define PIPE_FORMAT_ERROR                    -9
#define BACKGORUND_FORMAT_ERROR              -8
#define OUTPUT_REDIRC_FORMAT_ERROR           -7
#define INPUT_REDIRC_FORMAT_ERROR            -6
#define NO_EXECUTABLE_AFTER_INPUT_REDIRC     -5
#define NO_EXECUTABLE_AFTER_OUTPUT_REDIRC    -4

#ifndef QUOTE_MISMATCH
#define QUOTE_MISMATCH                       -3
#endif

#define PARSE_ERROR                          -2
#define ZERO_COMMANDS                         0
#define FAILURE                              -1
#define SUCCESS                               0
#define CHILD                                 0
#define FALSE                                 0
#define TRUE                                  1
#define DIRECTORY                             0
#define STRING_EQUALS                         0
#define FORK_FAILED                           1
#define EXEC_FAILED                           9
#define DUP_FAILED                            1
#define PIPE_FAILED                           1
#define EXECUTABLE                         0111
#define MAX_PIPES                            10 /* max number of pipe lines */
#define MAXITEM                             100 /* max number of words per line */
#define MAXSIZE                           25500 /* max size of the string to hold the commands */

int is_lsF = FALSE;     // flag to indicate that ls-F is in the pipe line
int is_exec = FALSE;    // flag to indicate that exec is in the pipe line
int fds[MAX_PIPES*2];   // holds the file descriptors for the pipes
int over_write = FALSE; // flag to indicate that the file should be over written.
char *prompt = "p2: "; // the prompt for the shell.
int background = FALSE; // a flag to indicate whether the process is background or not.
int pipe_flag = FALSE; // a flag to indicate pipes.
char *outfile = NULL; // holds the name for the output file.
char *infile = NULL; // holds the name for the input file.
int new_argv_size = 0; // holds the arguments to be executed.
int pipe_location[MAX_PIPES]; // locations of the pipes in the array of commands.
/*
 * Parses the input from stdin into a an array of string commands to facilitate calling the system call "execvp()"
 * sets the appropriate flags to either redirect input, set a background process, or create pipelines
 * input:
 *        commands: string array that will hold the addresses of the parsed commands and arguments to execute/
 *        line: holds the stdin unparsed input commands.
 * output: returns the number of words parsed,to faciltate executing them later.
 *        line: will hold the unparsed string
 *        commands: will hold the commands to execute
 * exitcodes:
 *              AMBIGUOUS_INPUT_REDIRECTION: Ambiguous input redirection and cannot be understood.
 *              AMBIGUOUS_OUTPUT_REDIRECTION: Ambiguous output redirection and cannot be understood.
 *              MULTIPLE_PIPELINES: More than ten pipelines are found
 *              PIPE_FORMAT_ERROR: Pipe line format is invalid.
 *              NO_PROCESS_AFTER_PIPE: No process is provided after the pipe symbol.
 *              BACKGORUND_FORMAT_ERROR: background process format is invalid.
 *              QUOTE_MISMATCH: Open quote is not matched with closed quote.
 *              OUTPUT_REDIRC_FORMAT_ERROR: No file name is provided after the output redirection symbol
 *              INPUT_REDIRC_FORMAT_ERROR: No file name is provided after the input redirection symbol
 *              NO_EXECUTABLE_AFTER_INPUT_REDIRC: No executable is provided after input redirection.
 *              NO_EXECUTABLE_AFTER_OUTPUT_REDIRC: No executable is provided after output redirection.
 *
 */
int parse(char **commands, char *line);

/*
 * sets up IO redirection by using the "dup2()" system call to duplicate the file descriptors of stdout or stdin
 * returns: the file descriptor of the last file opened.
 * exitcodes:
 *              REDIRECTION_FILE_EXISTS: The file already exists on output redirection and over_write flag is off.
 *              OUTPUT_FILE_OPEN_ERROR: Could not open output file.
 *              INPUT_FILE_OPEN_ERROR: Could not open input file.
 *              OUTPUT_REDIRECTION_FAILED: "dup2()" system call fails on output redirection.
 *              INPUT_REDIRECTION_FAILED:  "dup2()" system call fails on input redirection.
 */
int set_up_redirection();

/* checks if a file exists by using the system call access()
 * returns:
 *          SUCCESS: if the file/directory exists
 *          FAILURE: if the file/directory does not exist or error occured.
 */
int file_exists(const char *file_name) {
    return access(file_name,F_OK);
}
/* signal handler to catch SIGTERM on program Termination */
void killswitch(int signum);

/*
 * Executes a simplified version of Tcsh's "ls-F".
 * if the input string points to a file name, it checks if the file exists if it does it prints the name of the file
 * if the input string points to a directory, it attempts to open the directory and list all the files on their own line
 * appends '/' to every directory name
 * appends '&' to every 'broken' soft link (non-existent, or inaccessible)
 * appends '@' to every other soft link
 * appends '*' to every file that has one [or more] of the execute bits set
 * input: path_name: string pointing to the path name of a file or directory
 * output: either prints the file name or the directory contents
 * returns:
 *              STAT_FAIL: stat() cannot access the file/directory information.
 *              PERMISSION_DENIED: access is denied to the the file/directory.
 *              DIRECTORY_OPEN_ERROR: The path name doesnt point to a directory.
 *              LS_F_DIRECTORY_DOESNT_EXIST: The path name doesnt point to anything.
 *              TRAVERSE_STAT_FAIL: stat() failed on traversing the directory.
 *              LS_F_CD_FAIL: CD failed on changing the directory to the path provided
 *              LS_F_CD_RESTORE_FAIL: CD failed on restoring the current working directory back to normal.
 *              SUCCESS: if the execution completed successfully in any path.
 */
int exec_ls_f(char *);

/*
 * Executes a command by forking a child and calling the system call "execvp" on the given commands and arguments.
 * input: command: a string with the name of the executable to execute
 *        args: a string array with the arguments/flags for the given executable
 * output: the result of the execution on stdout or the file redirected to.
 * errors:
 *          FORK_FAILED: if the system call "fork" fails to fork a child.
 *
 */
void exec_command(char *command, char **args);

/*
 * Executes the command in the child forked from exec_command, and sets up any IO redirection that needs to be set up.
 * Forks another child if the pipe_flag is set to do vertical piping where the parent creates the pipe and the child
 * reads/writes to it, otherwise it just executes the given command with the arguments provided.
 * input: command: a string with the name of the executable to execute
 *        args: a string array with the arguments/flags for the given executable
 * output: the result of the execution on stdout or the file redirected to.
 * exits: on a failed system call.
 *          REDIRECTION_ERROR: the error code returned from set_up_redirection.
 *          FORK_FAILED: if the system call "fork" fails to fork a child.
 *          PIPE_FAILED: if the "pipe()" system call failed.
 *          DUP_FAILED: if the "dup2()" system call fails on redirection.
 *          EXEC_FAILED: if the "execvp()" system call fails to execute the command.
 */
void run_child_command(char *command, char **args);

/*
 * cleanup method to clear all the flags set to preserve the neutral state.
 */
void clear_flags();

/*
 * prints all the errors in the shell and clears all the flags that are set.
 * errors
 *  ls-F:
 *              STAT_FAIL: stat() cannot access the file/directory information.
 *              PERMISSION_DENIED: access is denied to the the file/directory.
 *              DIRECTORY_OPEN_ERROR: The path name doesnt point to a directory.
 *              LS_F_DIRECTORY_DOESNT_EXIST: The path name doesnt point to anything.
 *              TRAVERSE_STAT_FAIL: stat() failed on traversing the directory.
 *              LS_F_CD_FAIL: CD failed on changing the directory to the path provided
 *              LS_F_CD_RESTORE_FAIL: CD failed on restoring the current working directory back to normal.
 *  cd:
 *              CD_FAIL: Changing directory failed
 *              CD_INVALID_NUM_ARGS: Invalid number of arguments provided to cd.
 *
 *  redirection:
 *              REDIRECTION_FILE_EXISTS: The file already exists on output redirection.
 *              OUTPUT_FILE_OPEN_ERROR: Could not open output file.
 *              INPUT_FILE_OPEN_ERROR: Could not open input file.
 *              OUTPUT_REDIRECTION_FAILED: "dup2()" system call fails on output redirection.
 *              INPUT_REDIRECTION_FAILED:  "dup2()" system call fails on input redirection.
 *  parse:
 *              AMBIGUOUS_INPUT_REDIRECTION: Ambiguous input redirection and cannot be understood.
 *              AMBIGUOUS_OUTPUT_REDIRECTION: Ambiguous output redirection and cannot be understood.
 *              MULTIPLE_PIPELINE: More than ten pipelines are found
 *              PIPE_FORMAT_ERROR: Pipe line format is invalid.
 *              NO_PROCESS_AFTER_PIPE: No process is provided after the pipe symbol.
 *              BACKGORUND_FORMAT_ERROR: background process format is invalid.
 *              QUOTE_MISMATCH: Open quote is not matched with closed quote.
 *              OUTPUT_REDIRC_FORMAT_ERROR: No file name is provided after the output redirection symbol
 *              INPUT_REDIRC_FORMAT_ERROR: No file name is provided after the input redirection symbol
 *              NO_EXECUTABLE_AFTER_INPUT_REDIRC: No executable is provided after input redirection.
 *              NO_EXECUTABLE_AFTER_OUTPUT_REDIRC: No executable is provided after output redirection.
 *  system:
 *              FORK_FAILED_EXIT_CODE: Could not fork process.
 *              PIPE_OUTPUT_REDIRECT_FAILED: Output redirection failed on pipeline
 *              PIPE_INPUT_REDIRECT_FAILED: Input redirection failed on pipeline
 *              PIPE_FAILED_EXIT_CODE: Creating pipeline failed.
 *              EXEC_FAILED_EXIT_CODE: Executing executable failed mostly due to invaild executable name.
 *              NESTED_PIPELINE_PIPE_FAILED: creating nested pipeline failed.
 */
void print_error(int error_code,char* arg);
/*
 * Executes multiple nested pipe lines in a vertical fashion, each child will create a grandchild to execute the
 * command. Finally the last child will execute the first command and break out of the process and return to the function
 * run_child_command() and continue executing the program normally.
 * input: command: a string with the name of the executable to execute
 *        args: a string array with the arguments/flags for the given executable
 * output: the result of the execution on stdout or the file redirected to.
 * exits: on a failed system call.
 *          NESTED_PIPELINE_PIPE_FAILED: creating nested pipeline failed.
 *          FORK_FAILED_EXIT_CODE: Could not fork process.
 *          PIPE_OUTPUT_REDIRECT_FAILED: Output redirection failed on pipeline
 *          PIPE_INPUT_REDIRECT_FAILED: Input redirection failed on pipeline
 *          EXEC_FAILED: Executing executable failed mostly due to invalid executable name.
 *          FORK_FAILED: if the system call "fork" fails to fork a child.
 */
void nested_pipeline(char* command, char** args);
/*
 * converts an int into a string by using sprintf() with a %d flag.
 * input: int number: the number to convert to string.
 * output: string: the converted string.
 */
void to_string(int number,char* string){
    sprintf(string, "%d", number);
}
#endif //PROGRAM2_P2_H
