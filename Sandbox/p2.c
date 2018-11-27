#include "p2.h"

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

void killswitch(int signum) {
}

void clear_flags() {
    if (outfile != NULL)
        outfile = NULL;
    if (pipe_flag)
        pipe_flag = FALSE;
    if (infile != NULL)
        infile = NULL;
    if (background)
        background = FALSE;
    if (over_write)
        over_write = FALSE;
    if (is_exec)
        is_exec = FALSE;
    if (is_lsF)
        is_lsF = FALSE;

}

void print_error(int error_code, char *arg) {
    switch (error_code) {
        case STAT_FAIL:
            fprintf(stderr, "-p2: ls-F: could not access {%s} information.\n", arg);
            break;
        case PERMISSION_DENIED:
            fprintf(stderr, "-p2: ls-F: permission denied for {%s}.\n", arg);
            break;
        case DIRECTORY_OPEN_ERROR:
            fprintf(stderr, "-p2: ls-F: Could not open the directory {%s}.\n", arg);
            break;
        case LS_F_DIRECTORY_DOESNT_EXIST:
            fprintf(stderr, "-p2: ls-F: the directory {%s} does not exist.\n", arg);
            break;
        case TRAVERSE_STAT_FAIL:
            fprintf(stderr,"-p2: ls-F: lstat() failed on accessing the file {%s} on traversing the directory.\n",arg);
            break;
        case LS_F_CD_FAIL:
            fprintf(stderr,"-p2: ls-F: cd() failed on changing the current working directory to {%s}.\n",arg);
            break;
        case LS_F_CD_RESTORE_FAIL:
            fprintf(stderr,"-p2: ls-F: cd() failed on restoring the current working directory to {%s}.\n",arg);
            break;
        case CD_FAIL:
            fprintf(stderr, "-p2: cd: %s: No such file or directory.\n", arg);
            break;
        case CD_INVALID_NUM_ARGS:
            fprintf(stderr, "-p2: Cd: can only take one argument.\n");
            break;
        case REDIRECTION_FILE_EXISTS:
            fprintf(stderr, "-p2: file {%s} already exists.\n", outfile);
            break;
        case OUTPUT_FILE_OPEN_ERROR:
            fprintf(stderr, "-p2: could not open output file {%s}. \n", outfile);
            break;
        case INPUT_FILE_OPEN_ERROR:
            printf("-p2: could not open input file {%s}. \n", infile);
            break;
        case OUTPUT_REDIRECTION_FAILED:
            fprintf(stderr, "-p2: output redirection failed on file {%s}.\n",arg);
            break;
        case INPUT_REDIRECTION_FAILED:
            fprintf(stderr, "-p2: input redirection failed on file {%s}.\n",arg);
            break;
        case AMBIGUOUS_INPUT_REDIRECTION:
            fprintf(stderr, "-p2: ambiguous input redirection.\n");
            break;
        case AMBIGUOUS_OUTPUT_REDIRECTION:
            fprintf(stderr, "-p2: ambiguous output redirection.\n");
            break;
        case MULTIPLE_PIPELINES:
            fprintf(stderr, "-p2: this shell can only handle 10 pipelines.\n");
            break;
        case PIPE_FORMAT_ERROR:
            fprintf(stderr, "-p2: syntax error near unexpected token `|\'.\n");
            break;
        case NO_PROCESS_AFTER_PIPE:
            fprintf(stderr, "-p2: no process provided after `|\'.\n");
            break;
        case BACKGORUND_FORMAT_ERROR:
            fprintf(stderr, "-p2: syntax error near unexpected token `&\'.\n");
            break;
        case QUOTE_MISMATCH:
            fprintf(stderr, "-p2: Program 2 doesnt allow unmatched quotes.\n");
            break;
        case OUTPUT_REDIRC_FORMAT_ERROR:
            fprintf(stderr, "-p2: syntax error expected file name after output redirection.\n");
            break;
        case INPUT_REDIRC_FORMAT_ERROR:
            fprintf(stderr, "-p2: syntax error expected file name after input redirection.\n");
            break;
        case NO_EXECUTABLE_AFTER_INPUT_REDIRC:
            fprintf(stderr, "-p2: cannot redirect input because no executable has been provided.\n");
            break;
        case NO_EXECUTABLE_AFTER_OUTPUT_REDIRC:
            fprintf(stderr, "-p2: cannot redirect output because no executable has been provided.\n");
            break;
        case FORK_FAILED_EXIT_CODE:
            fprintf(stderr, "Could not fork process #%s.\n",arg);
            break;
        case PIPE_OUTPUT_REDIRECT_FAILED:
            fprintf(stderr, "-p2: output redirection failed on pipeline #%s.\n",arg);
            break;
        case PIPE_INPUT_REDIRECT_FAILED:
            fprintf(stderr, "-p2: input redirection failed on pipeline #%s.\n",arg);
            break;
        case PIPE_FAILED_EXIT_CODE:
            fprintf(stderr, "-p2: pipe line #%s failed.\n",arg);
            break;
        case EXEC_FAILED_EXIT_CODE:
            fprintf(stderr, "-p2: %s: command not found.\n", arg);
            break;
        case NESTED_PIPELINE_PIPE_FAILED:
            fprintf(stderr, "-p2: Nested pipe line #%s failed.\n",arg);
        default:
            break;
    }
    clear_flags(); // clear all the flags on an error to return to neutral state.
}

int main() {
    int exit_code;
    setpgrp(); // sets this process and children to its own process group.
    signal(SIGTERM, killswitch); // signal handler to catch SIGTERM and exit Gracefully.
    char *new_argv[MAXITEM]; // holds the parsed commands with flags to be executed by this shell.
    char line[MAXSIZE]; // holds the unparsed line from stdin.
    while (TRUE) {
        printf("%s", prompt);

        new_argv_size = parse(new_argv, line);
        if (new_argv_size == EOF) {
            break;
        } else if (new_argv_size == ZERO_COMMANDS) {
            fflush(stdout);
            continue;
        } else if (new_argv_size < PARSE_ERROR) {
            fflush(stdout);
            print_error(new_argv_size, NULL);
            fflush(stderr);
            continue;
        }

        if (strcmp(new_argv[0], "cd") == STRING_EQUALS) { // if the executable is cd then run the cd command
            if (new_argv_size == 1) { // if there are no other arguments change directory to HOME
                if (chdir(getenv("HOME")) == FAILURE) {
                    print_error(CD_FAIL, getenv("HOME"));
                } else {
                    clear_flags();
                }
            } else if (new_argv_size == 2) { // if two arguments are provided change directory to second argument
                if (chdir(new_argv[1]) == FAILURE) {
                    print_error(CD_FAIL, new_argv[1]);
                } else {
                    clear_flags();
                }
            } else { // cd only accepts 0 or 1 arguments
                print_error(CD_INVALID_NUM_ARGS, NULL);
            }
        } else if (strcmp(new_argv[0], "ls-F") == STRING_EQUALS) { // if the executable is ls-F then run the ls-F command
            if (new_argv_size == 1) {  // if there are no arguments just call ls-F command with the current directory
                if ((exit_code = exec_ls_f(".")) < SUCCESS) {
                    print_error(exit_code, ".");
                } else {
                    clear_flags();
                }
            } else if (new_argv_size >= 2) {
                // if ls-F is in the pipe line.
                if (pipe_flag) {
                    is_lsF = TRUE; // set the ls-F flag to true to indicate ls-F
                    exec_command(new_argv[0], new_argv);
                } else { // if there is more than 1 argument ls-F all the arguments in the new_argv array.
                    int i;
                    for (i = 1; i < new_argv_size; i++) {
                        if ((exit_code = exec_ls_f(new_argv[i])) < SUCCESS) {
                            print_error(exit_code, new_argv[i]);
                        }
                    }
                }
                clear_flags();
            }
        } else if (strcmp(new_argv[0], "exec") == STRING_EQUALS) {
            fflush(stdout);// flush buffers before forking a new process to clear the buffers for the child.
            fflush(stderr);
            // if exec is in the pipe line.
            if (pipe_flag) {
                is_exec = TRUE; // set exec flag
                run_child_command(new_argv[1], (new_argv + 1)); // pass the command right after exec and the array
                                                                // starting from that command. Does not fork a child
                                                                // so the shell will end right after the exec command
            } else {
                int redirection_return;
                if ((redirection_return = set_up_redirection()) < REDIRECTION_ERROR) { // set up any IO redirection
                    print_error(redirection_return, NULL);
                    exit(REDIRECTION_ERROR);
                }
                if (execvp(new_argv[1], new_argv + 1) == FAILURE) { // execute the process normally and exit p4.
                    print_error(EXEC_FAILED_EXIT_CODE, new_argv[0]);
                    exit(EXEC_FAILED);
                }
            }
        } else {
            exec_command(new_argv[0], new_argv); // if its neither 'Cd' nor 'ls-F' nor 'exec' execute all other executables
            clear_flags();
        }

    }
    killpg(getpgrp(), SIGTERM); // Terminate any children that are still running
    printf("p2 terminated.\n"); // print termination message after catching SIGTERM signal
    return 0;
}

int exec_ls_f(char *path_name) {
    DIR *directory_pointer; // hold the information about the directory
    struct dirent *dp; // represents the directory stream.
    struct stat sb; // represents the file/directory information.
    // get the current working directory and store it in old_directory
    char old_directory[1024];
    getcwd(old_directory, sizeof(old_directory));
    if (file_exists(path_name) == FAILURE) { // if the path doesnt point to anything return LS_F_DIRECTORY_DOESNT_EXIST exit code
        return LS_F_DIRECTORY_DOESNT_EXIST;
    }
    if (stat(path_name, &sb) != SUCCESS) { // if stat was unable to retrieve information return STAT_FAIL exit code
        return STAT_FAIL;
    }
    if (S_ISREG(sb.st_mode) != DIRECTORY) { // if the path does not point to a directory just print the path name again
        printf("%s\n", path_name);
        return SUCCESS;
    }
    if (access(path_name, R_OK) != SUCCESS) { // if the file is unreadable return PERMISSION_DENIED exit code.
        return PERMISSION_DENIED;
    } else {
        if ((directory_pointer = opendir(path_name)) == NULL) {
            return DIRECTORY_OPEN_ERROR;
        }
        // move the current directory to the path provided to be able to lstat properly.
        if (chdir(path_name) == FAILURE) {
            print_error(LS_F_CD_FAIL, path_name);
        }
        while (directory_pointer) { // iterate through the directory printing the file name on each line.
            if ((dp = readdir(directory_pointer)) != NULL) { // if there is still more to read print the file name
                if (lstat(dp->d_name, &sb) != SUCCESS) { // if stat was unable to retrieve information return STAT_FAIL exit code
                    print_error(TRAVERSE_STAT_FAIL, dp->d_name);
                } else {
                    if (S_ISDIR(sb.st_mode)) { // if it is a directory
                        printf("%s/\n", dp->d_name);
                    } else if (S_ISLNK(sb.st_mode)) { // if it is a link
                        if (stat(dp->d_name, &sb) != SUCCESS) { // check if it is a broken link
                            printf("%s&\n", dp->d_name);
                        } else
                            printf("%s@\n", dp->d_name); // valid link
                    } else if (sb.st_mode & EXECUTABLE) { // checks if any of the executable bits are set
                        printf("%s*\n", dp->d_name);
                    } else
                        printf("%s\n", dp->d_name); // just a regular file.
                }
            } else {
                closedir(directory_pointer);
                break;
            }
        }
    }
    //restore the current working directory back to the old directory
    if (chdir(old_directory) == FAILURE) {
        print_error(LS_F_CD_RESTORE_FAIL, old_directory);
        return LS_F_CD_RESTORE_FAIL;
    }
    return SUCCESS;
}


void exec_command(char *command, char **args) {
    pid_t child_pid;
    int wait_status;
    fflush(stdout); // flush buffers before forking a new process to clear the buffers for the child.
    fflush(stderr);
    if ((child_pid = fork()) == FAILURE) {
        print_error(FORK_FAILED_EXIT_CODE, "1");
        return;
    } else if (child_pid == CHILD) { // execute the command in the child
        run_child_command(command, args);
    } else {
        if (!background) {// if the command is not a background command wait for the specific child to complete
            // execution. reaping zombies if you encounter them.
            do {
                wait_status = wait(NULL);
            } while (wait_status != child_pid);
        } else {
            printf("%s [%d]\n", command, child_pid);
            background = FALSE;
        }
    }
}

void run_child_command(char *command, char **args) {
    pid_t child_pid;
    int redirection_return;
    if ((redirection_return = set_up_redirection()) < REDIRECTION_ERROR) { // set up any IO redirection
        print_error(redirection_return, NULL);
        exit(REDIRECTION_ERROR);
    }
    if (pipe_flag) {// set up vertical piping.
        int current;
        // create first pipe at fds[0],fds[1]
        if (pipe(fds) == FAILURE) {
            print_error(PIPE_FAILED_EXIT_CODE, "1");
            exit(PIPE_FAILED);
        }
        fflush(stdout); // flush buffers before forking a new process to clear the buffers for the child.
        fflush(stderr);
        if ((child_pid = fork()) == FAILURE) {
            print_error(FORK_FAILED_EXIT_CODE, "2");
            exit(FORK_FAILED);
            //grandchild
        } else if (child_pid == CHILD) {
            // more than 2 commands handle nested pipe lines.
            if (pipe_flag > 1)
                nested_pipeline(command, args);
                // only 2 commands.
            else {
                if (dup2(fds[1], STDOUT_FILENO) < SUCCESS) {
                    print_error(PIPE_OUTPUT_REDIRECT_FAILED, "1");
                    exit(DUP_FAILED);
                }
                close(fds[1]);
                close(fds[0]);
                if (is_lsF) {
                    int exit_code;
                    if ((exit_code = exec_ls_f(args[1])) < SUCCESS) {
                        print_error(exit_code, args[1]);
                        exit(FAILURE);
                    }
                    else
                        exit(SUCCESS);
                } else {
                    if (execvp(command, args) == FAILURE) {
                        print_error(EXEC_FAILED_EXIT_CODE, command);
                        exit(EXEC_FAILED);
                    }
                }
            }
        } else {
            current = pipe_location[pipe_flag - 1];
            if (is_exec) //if exec is in the pipe line decrement current to ignore the exec and fix the array indices
                current--;
            if (dup2(fds[0], STDIN_FILENO) < SUCCESS) { // in the child redirect input to the output end of the pipe
                // to read the output of the grandchild.
                char num[2]; // convert the index of the pipe for error message
                to_string(pipe_flag-1,num); // converts int to char* for printing.
                print_error(PIPE_INPUT_REDIRECT_FAILED, num);
                exit(DUP_FAILED);
            }
            close(fds[0]); // close read end of the pipe
            close(fds[1]); // close write end of the pipe.
            if (execvp(args[current + 1], args + (current + 1)) == FAILURE) { // execute the last command in the array.
                print_error(EXEC_FAILED_EXIT_CODE, args[current + 1]);
                exit(EXEC_FAILED);
            }
        } // no pipe line execute the command normally.
    } else {
        if (execvp(command, args) == FAILURE) {
            print_error(EXEC_FAILED_EXIT_CODE, command);
            exit(EXEC_FAILED);
        }
    }
}


int parse(char **commands, char *line) {
    int word_count = 0;
    int wordsize = 0;
    int output_redirection = FALSE;
    int input_redirection = FALSE;
    int ambigous_input_redirection_flag = FALSE;
    int ambigous_output_redirection_flag = FALSE;
    do {
        if ((wordsize = getword(line)) > ZERO_COMMANDS) {
            if (strcmp(line, "&") == STRING_EQUALS) {
                background = TRUE;
                break;
            } else if (strcmp(line, "|") == STRING_EQUALS) {
                if (pipe_flag > 10) {
                    return MULTIPLE_PIPELINES;
                }// get the locations of the pipes to facilitate execution of commands later
                int location;
                if (word_count == ZERO_COMMANDS) { // if the '|' is the only symbol on the line return PIPE_FORMAT_ERROR
                    return PIPE_FORMAT_ERROR;
                } else {
                    if (pipe_flag == 0) { // if first pipe set the location to the word_count
                        location = word_count;
                    } else { // else offset the location by the number of pipes already parsed.
                        location = word_count + pipe_flag;
                    }
                    pipe_location[pipe_flag++] = location; // store the location of the pipe and then increment the flag
                }
                *commands++ = NULL; // set the argument at the pipeflag to be null to split the 1st process from
                // the second.
            } else if (strcmp(line, ">") == STRING_EQUALS) { // if the symbol is '>' set up output redirection
                if (output_redirection) { // if more than one '>' symbol is found set error flag to clear the stream.
                    ambigous_output_redirection_flag = TRUE;
                }
                output_redirection = TRUE;
            } else if (strcmp(line, ">!") == STRING_EQUALS) {
                if (output_redirection) { // if more than one '>' symbol is found set error flag to clear the stream.
                    ambigous_output_redirection_flag = TRUE;
                }
                output_redirection = TRUE;
                over_write = TRUE; // set the over_write flag to over write the file if it exists
            } else if (strcmp(line, "<") == STRING_EQUALS) { // if the symbol is '<' set up input redirection
                if (input_redirection) {// if more than one '<' symbol is found set error flag to clear the stream.
                    ambigous_input_redirection_flag = TRUE;
                }
                input_redirection = TRUE;
            } else if (output_redirection && outfile == NULL) {
                outfile = line;
                line += wordsize + 1;
            } else if (input_redirection && infile == NULL) {
                infile = line;
                line += wordsize + 1;
            } else { // add the commands to the commands array and increment line to point to the new word.
                word_count++;
                *commands++ = line;
                line += wordsize + 1;
            }
        }
        if (wordsize == QUOTE_MISMATCH) // check for quote mismatch
            return QUOTE_MISMATCH;
    } while (wordsize != EOF && wordsize != ZERO_COMMANDS); // read until a line terminator is encountered
    if (wordsize == EOF)
        word_count = EOF;
    if (ambigous_input_redirection_flag) {
        return AMBIGUOUS_OUTPUT_REDIRECTION;
    }
    if (ambigous_output_redirection_flag) {
        return AMBIGUOUS_INPUT_REDIRECTION;
    }
    if (background && word_count == ZERO_COMMANDS) {
        return BACKGORUND_FORMAT_ERROR;
    }
    if (output_redirection && outfile == NULL) {
        return OUTPUT_REDIRC_FORMAT_ERROR;
    }
    if (input_redirection && infile == NULL) {
        return INPUT_REDIRC_FORMAT_ERROR;
    }
    if (infile != NULL && word_count == ZERO_COMMANDS) {
        return NO_EXECUTABLE_AFTER_INPUT_REDIRC;
    }
    if (outfile != NULL && word_count == ZERO_COMMANDS) {
        return NO_EXECUTABLE_AFTER_OUTPUT_REDIRC;
    }
    if (pipe_flag && pipe_location[pipe_flag - 1] + 1 >= word_count + pipe_flag) {
        return NO_PROCESS_AFTER_PIPE;
    }
    *commands = NULL; // null terminate the string array.
    return word_count;
}

int set_up_redirection() {
    int file_descriptor = 0;
    if (outfile != NULL) {
        if (file_exists(outfile) == SUCCESS && !over_write) { // if the file exists and and over_write is not set return
            //error.
            return REDIRECTION_FILE_EXISTS;
        }
        file_descriptor = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if (file_descriptor < SUCCESS) {
            print_error(OUTPUT_FILE_OPEN_ERROR, outfile);
            exit(OUTPUT_FILE_OPEN_ERROR);
        }
        if (dup2(file_descriptor, STDOUT_FILENO) < SUCCESS) { // redirect output from stdout to newly opened file.
            print_error(OUTPUT_REDIRECTION_FAILED, outfile);
            exit(DUP_FAILED);
        }
        close(file_descriptor);
    }
    if (infile != NULL) {
        // open file for read only.
        file_descriptor = open(infile, O_RDONLY);
        if (file_descriptor < SUCCESS) {
            print_error(INPUT_FILE_OPEN_ERROR, infile);
            exit(INPUT_FILE_OPEN_ERROR);
        }
        if (dup2(file_descriptor, STDIN_FILENO) < SUCCESS) { // redirect input from stdin to newly opened file.
            print_error(INPUT_REDIRECTION_FAILED, infile);
            exit(DUP_FAILED);
        }
        close(file_descriptor);
    }
    if (infile == NULL && background) { // redirect background process input to dev/null to avoid having a deadlock
        // open the file dev/null as read only
        file_descriptor = open("/dev/null", O_RDONLY);
        if (file_descriptor < SUCCESS) {
            print_error(INPUT_FILE_OPEN_ERROR, "/dev/null");
            exit(INPUT_FILE_OPEN_ERROR);
        }
        if (dup2(file_descriptor, STDIN_FILENO) < SUCCESS) { // redirect the input from stdin to dev/null.
            print_error(INPUT_REDIRECTION_FAILED, "/dev/null");
            exit(DUP_FAILED);
        }
        close(file_descriptor);
    }
    return file_descriptor;
}

void nested_pipeline(char *command, char **args) {
    int current; // current pipe line to execute
    pid_t grandChild_pid; // pid of the current grand_child
    int j = 1; // offset for pipes
    int i;
    int start_fd = 0; // the read end of the current pipe.
    // loop until one before the last pipe
    for (i = pipe_flag - 1; i > 0; i--) {
        start_fd = (2 * j); // get the read end of the current pipe.
        if (pipe((fds + (2 * j))) == FAILURE) { // open a new pipe
            char num[2];
            to_string(i,num);
            print_error(NESTED_PIPELINE_PIPE_FAILED, num);
            exit(PIPE_FAILED);
        }
        fflush(stdout);
        fflush(stderr);
        if ((grandChild_pid = fork()) == FAILURE) {
            char num[2];
            to_string(i,num);
            print_error(FORK_FAILED_EXIT_CODE, num);
            exit(FORK_FAILED);
            //grandchild ls
        } else if (grandChild_pid != CHILD) { // if it is the parent of the grandchild execute the command.
            if (dup2(fds[start_fd - 1], STDOUT_FILENO) < SUCCESS) {
                char num[2];
                to_string(i,num);
                print_error(PIPE_OUTPUT_REDIRECT_FAILED, num);
                exit(DUP_FAILED);
            }
            if (dup2(fds[start_fd], STDIN_FILENO) < SUCCESS) {
                char num[2];
                to_string(i,num);
                print_error(PIPE_INPUT_REDIRECT_FAILED, num);
                exit(DUP_FAILED);
            }
            close(fds[start_fd - 2]); // close read end of inherited pipe
            close(fds[start_fd - 1]); // close write end of inherited pipe
            close(fds[start_fd]);     // close read end of own pipe
            close(fds[start_fd + 1]); // close write end of own pipe
            current = pipe_location[i - 1]; // get the previous pipe to execute the middle command.
            if (is_exec) // decrement current to fix command offset in array.
                current--;
            if (execvp(args[current + 1], args + (current + 1)) == FAILURE) {
                print_error(EXEC_FAILED_EXIT_CODE, args[current + 1]);
                exit(EXEC_FAILED);
            }
        } else {
            if (i - 1 == 0) { // if last command execute it in the child and exit the process.
                if (dup2(fds[start_fd + 1], STDOUT_FILENO) < SUCCESS) {
                    char num[2];
                    to_string(i,num);
                    print_error(PIPE_OUTPUT_REDIRECT_FAILED, num);
                    exit(DUP_FAILED);
                }
                close(fds[start_fd + 1]);
                close(fds[start_fd]);
                if (is_lsF) { // if the command is ls-F call exec_ls_f() and exit either on failure or success to terminate
                              // the process.
                    int exit_code;
                    if ((exit_code = exec_ls_f(args[1])) < SUCCESS) {
                        print_error(exit_code, args[1]);
                        exit(FAILURE);
                    }
                    else
                        exit(SUCCESS);
                } else {
                    if (execvp(command, args) == FAILURE) {
                        print_error(EXEC_FAILED_EXIT_CODE, command);
                        exit(EXEC_FAILED);
                    }
                }
            }
            j++; // increment the pipe offset.
        }
    }
}

