#include "p2.h"

/*
 * Class Info - Program #2 Bernardo Cervantes, John Carroll, CS570 program 2
 * 		Due : 10/5/18 for extra credit and 10/10/18 for regular deadline
 * Synopsis   - Program acts as a simple unix shell. It reads from stdin and parses
 * 		the line and returns the corresponding command output. It handles 
 * 		the metacharacters {'&','<','>','|','\' and EOF. Only hands single pipes
 * 		for input & output redirection.
 * 			- Program uses getword.c to get STDIN and parse it into a string array.
 * 			- Array is a parsed and then passed to evecvp to execute the commands.
 * 			- To handle execution we use system calls like dup2, execvp, fork, and pipe, etc.
 *
 */
char *new_argv[MAXITEM];// Holds addressed to parsed commands to be executed by the shell.
char line[MAXSIZE]; // Holds the unparsed line from stidn
int main(){
	//int exit_code;
	setpgid(0,0); // Sets current process and children to its own process group
	(void) signal(SIGTERM,sighandler); // Signal handler to catch SIGTERM
	while(TRUE){
		printf("%s", prompt);
		new_argv_size = parse(new_argv, line);

		if(new_argv_size == EOF){
			break;

		}
		else if(new_argv_size == ZERO_COMMANDS){
			fflush(stdout);
			continue;
		}

		else if(new_argv_size < PARSE_ERROR){
			fflush(stdout);
			perror("Parse error");
			fflush(stderr);
			continue;	
		}			

		else {
			exec_command(new_argv[0], new_argv);
			clear_flags();


		}


	}

	killpg(getpgrp(),SIGTERM);
	printf("p2 terminated.\n");
	return 0;



}

int parse(char **commands , char *line){ 
	int word_count = 0;
	int word_size = 0;
	int output_redirection = FALSE;
	int input_redirection = FALSE;
	int ambiguous_input_redirection_flag = FALSE;
	int ambiguous_output_redirection_flag = FALSE;

	do {
		if ((word_size = getword(line)) > ZERO_COMMANDS ) { 
			//word_size = getword(line);
			if(strcmp(line, "&") == STRING_EQUALS && backslash_flag == FALSE && background_flag_from_getword == TRUE){

				background_flag = TRUE;
				break;
			}

			/*
			   else if (strcmp(line, "|") == STRING_EQUALS) {
			   if (pipe_flag) { // if the pipe line flag is set and it encounters another '|' return MULTIPLE_PIPELINE
			   return MULTIPLE_PIPELINE;
			   }
			   if (word_count == ZERO_COMMANDS) { // if the '|' is the only symbol on the line return PIPE_FORMAT_ERROR
			   return PIPE_FORMAT_ERROR;
			   } else {
			   pipe_flag = word_count; // set the pipe line flag to the location of the pipeline symbol
			   }
			 *commands++ = NULL; // set the argument at the pipeflag to be null to split the 1st process from
		// the second.

		}*/
			else if (strcmp(line, ">") == STRING_EQUALS) { // if the symbol is '>' set up output redirection
				if (output_redirection) { // if more than one '>' symbol is found set error flag to clear the stream.
					ambiguous_output_redirection_flag = TRUE;
				}
				redirection_flag = TRUE;
				output_redirection = TRUE;

			} else if (strcmp(line, "<") == STRING_EQUALS) { // if the symbol is '<' set up input redirection
				if (input_redirection) {// if more than one '<' symbol is found set error flag to clear the stream.
					ambiguous_input_redirection_flag = TRUE;
				}
				redirection_flag = TRUE;
				input_redirection = TRUE;
				/* Checking for a valid out/input file. Using NULL instead of 0 to check 
				 * since 0 and NULL are equivalent
				 */
			} else if (output_redirection && outfile == NULL) {
				outfile = line;
				line += word_size + 1;
			} else if (input_redirection && infile == NULL) {
				infile = line;
				line += word_size + 1;
			}

			else { // add the commands to the commands array and increment line to point to the new word.
				word_count++;
				*commands++ = line;
				//*commands = line;
				//*commands++;
				line += word_size + 1;
				//backslash_flag = FALSE;
				//background_flag_from_getword = FALSE;
			}


		}
	} while (word_size != EOF && word_size != ZERO_COMMANDS); // read until a line terminator is encountered

	if(word_size == EOF){
		word_count = EOF;

	}

	*commands = NULL; //null terminate string array of commands
	return word_count;
}


void exec_command(char *command, char **args){
	pid_t child_pid;
	int wait_status;
	fflush(stdout);// Flushing before a fork to ensure stdout & stdout buffers are clear for the child.
	fflush(stderr);

	if((child_pid = fork()) == FAIL){
		perror("Forked failed");
		return;	
	}
	/* Executes the child command */
	else if(child_pid == SUCCESS){
		run_child_command(command, args);
	}

	else {
		/* if background flag is not set then the shell waits
		 * for its childrne to die and any leftover zombies.
		 */
		if(!background_flag){
			do{
				wait_status = wait(NULL);

			}while(wait_status != child_pid);

		}
		else{
			printf("%s [%d]\n", command, child_pid);
			background_flag = FALSE;

		}

	}	

}

void run_child_command(char *command, char **args){
	int wait_status;
	pid_t child_pid;
	int redirection_return;	

	if(redirection_flag){

	
		if(redirection_return = set_up_redirection() < REDIRECTION_ERROR){
			perror("Redirection error");
			exit(-2);
		}
	}

	fflush(stdout);
	fflush(stderr);


	if(execvp(command, args) == FAIL){

		perror("Exec failed");
		exit(1);
	}

}

int set_up_redirection(){
	int file_descriptor = 0;
	if(outfile != NULL){
		/*Open file for write only if it doesn't exists. It creates a file with
		 * read and write permissions as indicated b the flags passed to open
		 */
		if(file_exists(outfile) == SUCCESS){
			perror("File exists");	

		}
		file_descriptor = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR, S_IWUSR);
		
		if(file_descriptor < SUCCESS){
			perror("Cannot open overwrite file");
			exit(-3);

		}
		/* Redirecting output from stdout to the open file*/

		if(dup2(file_descriptor, STDOUT_FILENO) < SUCCESS){
			perror("Output redirection with dup2 failed");
			exit(-4);
		}
		
		close(file_descriptor);
	

	}

	if(infile != NULL){
		file_descriptor = open(infile, O_RDONLY); //Only reading file
		if(file_descriptor < SUCCESS){
			perror("Cannot read input file");
			exit(-5);
		}
		if(dup2(file_descriptor, STDIN_FILENO) < SUCCESS){
			perror("Input file redirection failed");
			exit(-6);
		}
		close(-7);



	}
	
	if(infile == NULL && background_flag){
		file_descriptor = open("dev/null", O_RDONLY);
		if(file_descriptor < SUCCESS){
			perror("Input file cannot be opened with background flag");
			exit(-7);
		}
		if(dup2(file_descriptor, STDIN_FILENO) < SUCCESS){
			perror("Input redirection failed with background flag");
			exit(-8);
		}
		close(file_descriptor);
	}

	return file_descriptor;

}

void clear_flags(){
	if(background_flag)
		background_flag = FALSE;
	if(outfile != NULL){
		outfile = NULL;
	}
	if(pipe_flag){
		pipe_flag = FALSE;
	}
	if(infile != NULL){
		infile = NULL;
	}

}
void sighandler(){


}