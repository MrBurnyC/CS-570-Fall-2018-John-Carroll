#include "p2.h"

/*
 * Class Info - Program #4 Bernardo Cervantes, John Carroll, CS570 program 2
 * 		Due : 11/28/18 for extra credit and 12/5/18 for regular deadline
 * Synopsis   - Program acts as a simple unix shell. It reads from stdin and parses
 * 		the line and returns the corresponding command output. It handles 
 * 		the metacharacters {'&','<','>','|','\',"<<" and EOF. Now handles up to 10 pipes
 * 		for input & output redirection.
 * 			- Program uses getword.c to get STDIN and parse it into a string array.
 * 			- Array is a parsed and then passed to evecvp to execute the commands.
 * 			- To handle execution we use system calls like dup2, execvp, fork, and pipe, etc.
 *
 */
char *new_argv[MAXITEM];// Holds addressed to parsed commands to be executed by the shell.
char line[MAXSIZE]; // Holds the unparsed line from stidn
int main(){
	setpgid(0,0); // Sets current process and children to its own process group
	(void) signal(SIGTERM,sighandler); // Signal handler to catch SIGTERM
	while(TRUE){
		printf("%s", prompt);

		if(hereis_flag){
			set_up_hereis_doc();
			hereis_flag = FALSE;//Can now execute the commands
		}
		else{
			new_argv_size = parse(new_argv, line);
		}

		if(new_argv_size == EOF){
			break;

		}
		else if(new_argv_size == ZERO_COMMANDS){
			fflush(stdout);
			continue;
		}

		else if(new_argv_size < PARSE_ERROR){
			fflush(stdout);
			print_error(new_argv_size, NULL);		
			fflush(stderr);
			continue;	
		}
		/* Handing cd for the shell since you cannot
		 * use execvp to execute cd.
		 *
		 */
		else if(strcmp(new_argv[0], "cd") == STRING_EQUALS){
			if(new_argv_size == 1){

				if(chdir(getenv("HOME")) == FAIL){
					print_error(CD_FAIL, getenv("HOME"));		
				}
				else{
					clear_flags();
				}
			}

			/* Case for two arguments to change directory*/
			else if(new_argv_size == 2){
				if(chdir(new_argv[1]) == FAIL){
					print_error(CD_FAIL, new_argv[1]);		
				}
				else{
					clear_flags();
				}	
			}
			else {// cd only accepts 0 or 1 arguemnts
				print_error(CD_INVALID_NUM_ARGS, NULL);	
			}
		}
		else {
			/*Don't execute the commands yet*/
			if(hereis_flag){
				continue;
			}

			else{
				exec_command(new_argv[0], new_argv);
				clear_flags();
			}
		}

	}

	killpg(getpgrp(),SIGTERM);
	printf("p2 terminated.\n");
	return 0;
}

/* Parameters:Takes a pointer to a char pointer array which is new_argv, 
 * and pointer to a char array line which is line
 */

int parse(char **commands , char *line){ 
	int word_count = 0;
	int word_size = 0;
	int output_redirection = FALSE;
	int input_redirection = FALSE;
	int location; // gets the locations of the pipes to execute the commands easier laster on.

	do {
		if((word_size = getword(line))) {//I don't do > ZERO_COMMANDS because of the negative values from getword 
			if(strcmp(line, "&") == STRING_EQUALS && backslash_flag == FALSE && background_flag_from_getword == TRUE){
				background_flag = TRUE;
				break;
			}
			else if (strcmp(line, "|") == STRING_EQUALS) {
				/* if statement executes if pipe_flag is anything but 0. 
				 * Case preventing more than one pipe which will be handled in p4.c according to class lecture.	
				 */
				if (pipe_flag > 10) { 
					//perror("Cannot have more than 10 pipes");		
					return MULTIPLE_PIPELINES;
				}
				if (word_count == ZERO_COMMANDS) { // case where pipe is the only arguement on line/STDIN 
					//perror("Need to an argument to pipe to.");
					return PIPE_FORMAT_ERROR;
				} else {
					if(pipe_flag == 0){
						location = word_count;//first pipe is set to the location of word_count

					}else{// offset by the location by the number of pipe already parsed.
						location = word_count + pipe_flag;
					}
					pipe_location[pipe_flag++] = location; // store the location of the piepe an then increment the pipe flag.
				}
				*commands++ = NULL; // set the argument at the pipeflag to be null to split the 1st process from
				// the second.a

			}
			else if (strcmp(line, ">") == STRING_EQUALS) { // if the symbol is '>' set up output redirection
				if (output_redirection) { // if more than one '>' symbol is found set error flag to clear the stream.
					perror("Ambiguous redirection output. Cannot execute");
					return -1;
				}
				redirection_flag = TRUE;
				output_redirection = TRUE;

			} else if (strcmp(line, "<") == STRING_EQUALS) { // if the symbol is '<' set up input redirection
				if (input_redirection) {// if more than one '<' symbol is found set error flag to clear the stream.
					perror("Ambiguous redirection. Cannot execute.");
					return -1;
				}
				redirection_flag = TRUE;
				input_redirection = TRUE;

			}else if(strcmp(line, "<<") == STRING_EQUALS){
				if(hereis_flag){
					perror("Ambiguous hereis document. Cannot execute");
					exit(-1);
				}
				hereis_flag = TRUE;	
			}

			/* Checking for a valid out/input file. Using NULL instead of 0 to check 
			 * since 0 and NULL are equivalent
			 * Setting address pointer to coresponding file and incrementing line
			 * which is the stdin.
			 */
			else if (output_redirection && outfile == NULL) {
				outfile = line;
				line += word_size + 1;
			} else if (input_redirection && infile == NULL) {
				infile = line;
				line += word_size + 1;
			}

			/* Creating a pointer to the hereis delimiter*/
			else if(hereis_flag && hereis_delimiter == NULL){
				hereis_delimiter = line;
				word_count++;
				line += word_size + 1;
				//*commands++ = tmp_name; //Pointer to filename where hereis_doc contents is at.
			}
			else { // add the commands to the commands array and increment line to point to the new word.
				word_count++;
				*commands++ = line;
				//line += abs(word_size) + 1;// Using abs because of the negative sign from $ 
				line += word_size + 1; 
			}

		}
	} while (word_size != EOF && word_size != ZERO_COMMANDS); // read until a line terminator is encountered

	if(word_size == EOF){
		word_count = EOF;

	}
	if(background_flag && word_count == ZERO_COMMANDS){
		return BACKGROUND_FORMAT_ERROR;
	}
	if(output_redirection && outfile == NULL){
		return OUTPUT_REDIRC_FORMAT_ERROR;
	}

	if(input_redirection && infile == NULL){
		return INPUT_REDIRC_FORMAT_ERROR;
	}

	if(infile != NULL && word_count == 0){
		return NO_EXECUTABLE_AFTER_INPUT_REDIRC;
	}
	if(outfile != NULL && word_count == 0){
		return NO_EXECUTABLE_AFTER_OUTPUT_REDIRC;
	}
	if(pipe_flag && pipe_location[pipe_flag -1] + 1 >= word_count + pipe_flag){
		return NO_PROCESS_AFTER_PIPE;
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
		print_error(FORK_FAILED_EXIT_CODE, "1");
		return;
	}
	/* Executes the child command */
	else if(child_pid == SUCCESS){
		run_child_command(command, args);

	}else {
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
	//it wait_status;
	pid_t child_pid;
	int redirection_return;	
	int hereis_executed_flag = FALSE;

	if(redirection_flag){
		if((redirection_return = set_up_redirection()) < SUCCESS){
			print_error(redirection_return, NULL);
			exit(REDIRECTION_ERROR);
		}
	}


	if(pipe_flag){
		//int fds[2]; //file descriptors
		int current; //current pipe
		if(pipe(fds) == FAIL){
			print_error(PIPE_FAILED_EXIT_CODE, "1");
			exit(PIPE_FAILED_EXIT_CODE);
		}
		fflush(stdout); //Flush before fork everything.
		fflush(stderr);
		if((child_pid = fork()) == FAIL){
			print_error(PIPE_FAILED_EXIT_CODE, "1");
			exit(PIPE_FAILED_EXIT_CODE);
		}

		/* The grandchild handles the redirection to the output to the of the pipe
		 * or write end of the pipe. One can think of it has the stdout of the pipe.
		 */
		else if(child_pid == SUCCESS){
			//More than two commands handle nested pipe lines
			if(pipe_flag > 1){
				nested_pipeline(command, args);
			}
			//Only two commands.
			else{

				if(dup2(fds[1], STDOUT_FILENO) < SUCCESS){
					print_error(PIPE_OUTPUT_REDIRECT_FAILED, "1");
					exit(DUP_FAILED);
				}

				close(fds[1]);
				close(fds[0]);
				if(execvp(command,args) == FAIL){
					perror("Execvp failed inside of pipeline code");
					exit(-14);
				}
			}

		}else{
			/*
			   if(!background_flag){
			   do{
			   wait_status = wait(NULL);
			   }while(wait_status != child_pid);
			   */

			//Redirecting child input to the output end of the pipe
			current = pipe_location[pipe_flag - 1];
		if(dup2(fds[0], STDIN_FILENO) < SUCCESS){//to read the outpt of the grandchild
			char num[2];
			to_string(pipe_flag -1 , num);
			print_error(PIPE_INPUT_REDIRECT_FAILED, num);	
			exit(DUP_FAILED);
		}
		if(hereis_flag){

			set_up_hereis_doc();
			dup(saved_stdin);
			hereis_flag = FALSE;
		}
		close(fds[0]);
		close(fds[1]);


		/*Executing hereis section */
		if(execvp(args[current + 1], args + (current + 1 )) == FAIL){
			print_error(EXEC_FAILED_EXIT_CODE, args[current + 1]);
			exit(EXEC_FAILED_EXIT_CODE);
		}
}
}
else{//no pipe line so execute the command normally

	if(execvp(command, args) == FAIL){
		print_error(EXEC_FAILED_EXIT_CODE, command);	
		exit(EXEC_FAILED_EXIT_CODE);
	}
}

}
int set_up_hereis_doc(){
	char mystring [MAXSIZE] = "";
	char iochar = NULL;
	int i = 0;
	int delimiter_line = 1;	
	FILE *fileptr1;
	int file_descriptor_in;//using for replacing stdin with a our file
	int temp_line = 1;//Starts at line 1
	/*644 meanms you can read and write and so can other users. */
	//FILE* file = open(tmp_name,O_WRONLY | O_APPEND | O_CREAT, 0644);
	hereis_doc = fopen(tmp_name, "w+");
	//hereis_doc = freopen(tmp_name, "w+", stdin);	
	//file_descriptor_ptr = fileno(hereis_doc);
	//dup2(*file_descriptor_ptr, STDIN_FILENO);
	if(hereis_doc == NULL){
		perror("fopen failed");
		exit(-36);
	}

	while(iochar != -1){ //Use -1 instead of EOF because we defined EOF to be -255
		iochar = getchar();
		mystring[i] = iochar;
		i++;
	}

	/*Null terminating the char array and removes the last newline */
	//i--;

	mystring[i] ='\0';

	fputs(mystring, hereis_doc);
	fflush(hereis_doc); // need to fflush so the contents will appear in the file immediately.

	delimiter_line = search_in_File(tmp_name, hereis_delimiter);
	/*Removing line from file by copying it to a 
	 * new file and then replacing it without the delimiter
	 * line
	 */
	rewind(hereis_doc);
	fileptr1 = fopen("replica","w");

	iochar = NULL; //Reset from last while loop because it's last value is -1 
	while(iochar != -1){
		iochar = getc(hereis_doc);
		if(iochar == '\n'){
			temp_line++;
		}
		//Copy all except the delimiter line
		if(temp_line != delimiter_line){
			putc(iochar , fileptr1);
		}		

	}	

	if(hereis_doc){
		fclose(hereis_doc);
	}

	if(fileptr1){
		fclose(fileptr1);
	}
	remove(tmp_name);
	rename("replica", tmp_name);
	/* Redirecting stdin to get input from our file tmp_name. */
	saved_stdin = dup(0); //save stdin to restore it later
	file_descriptor_in = open(tmp_name, O_RDONLY);
	dup2(file_descriptor_in, 0);
	close(file_descriptor_in);
	//hereis_doc = &file;
	return OK_TO_EXECUTE_COMMANDS;

}

int search_in_File(char *fname, char *str){
	FILE *fp;
	int line_num =1;
	char temp[512] = "";

	if((fp = fopen(fname, "r")) == NULL){
		perror("Couldn't read tmp file");
		return -1;
	}

	/* Loop until one finds the line that needs to
	 * be taken out
	 */
	while(fgets(temp, 512, fp) != NULL){
		if((strstr(temp,str)) != NULL){
			//printf("A match found on line: %d\n", line_num);
			break;
		}
		line_num++;

	}

	if(fp){
		fclose(fp);
	}

	return line_num;	
}
int set_up_redirection(){
	int file_descriptor = 0;

	if(outfile != NULL){
		/*Open file for write only if it doesn't exists. It creates a file with
		 * read and write permissions as indicated b the flags passed to open
		 */
		if(file_exists(outfile) == SUCCESS){
			perror("File exists");
			return 	-1;

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
		if(dup2(file_descriptor, STDIN_FILENO) < SUCCESS){//redirect input from stdin to newly opened file.
			perror("Input file overwritten");
			exit(-6);
		}
		close(file_descriptor);
	}

	if(infile == NULL && background_flag){//redirect background process input to dev/null to avoid having deadlock
		file_descriptor = open("/dev/null", O_RDONLY);
		if(file_descriptor < SUCCESS){
			perror("Cannot open file with background flag ");
			exit(-7);
		}
		if(dup2(file_descriptor, STDIN_FILENO) < SUCCESS){ //redirect the input from stdin to dev/null
			perror("input redirection failed with background flag");
			exit(-9);
		}
		close(file_descriptor);
	} 
	return file_descriptor;

}

void nested_pipeline(char *command, char **args){
	int current;//currrent pipe line to execute	
	pid_t grandChild_pid; //pid of the current grand_child
	int j = 1; //offset for pipes
	int i;
	int start_fd = 0; // the read end of the current pipe.

	//loop until one before the last pipe.
	for(i = pipe_flag - 1; i > 0; i--){
		start_fd = (2*j); // get the read end of the current pipe.
		if(pipe((fds + (2 * j))) == FAIL){
			char num[2];
			to_string(i,num);
			perror("Nested pipeline failed");
			exit(-17);

		}
		fflush(stdout);
		fflush(stderr);
		if((grandChild_pid = fork()) == FAIL){ 
			char num[2];
			to_string(i,num);
			print_error(FORK_FAILED_EXIT_CODE, num);
			exit(DUP_FAILED);		
		}
		/* If it is the parent of the the grandchild, then execute the command */	
		else if (grandChild_pid != SUCCESS){
			if(dup2(fds[start_fd - 1], STDOUT_FILENO) < SUCCESS){
				char num[2];
				to_string(i, num);
				print_error(PIPE_OUTPUT_REDIRECT_FAILED, num);
				exit(DUP_FAILED);		
			}
			if(dup2(fds[start_fd], STDIN_FILENO) < SUCCESS){
				char num[2];
				to_string(i,num);
				print_error(PIPE_INPUT_REDIRECT_FAILED, num);
				exit(DUP_FAILED);		
			}
			close(fds[start_fd - 2]); //close read end of inherited pipe
			close(fds[start_fd - 1]); //close write end of inherited pipe
			close(fds[start_fd]);	  //close rend end of own pipe
			close(fds[start_fd + 1]); //close write end of own pipe
			current = pipe_location[i - 1]; //get the previous pipe to execute the middle command.

			if(execvp(args[current + 1], args + (current + 1)) == FAIL){
				perror("Exec failed within nested pipe code");
				exit(-20);	
			}

		}
		else{
			if(i - 1 == 0){
				if(dup2(fds[start_fd + 1], STDOUT_FILENO) < SUCCESS){
					char num[2];
					to_string(i,num);
					print_error(PIPE_OUTPUT_REDIRECT_FAILED,num);
					exit(DUP_FAILED);
				}
				close(fds[start_fd + 1]);
				close(fds[start_fd]);

				if(execvp(command, args) == FAIL){
					print_error(EXEC_FAILED_EXIT_CODE, command);
					exit(EXEC_FAILED_EXIT_CODE);
				}


			}		
			j++;
		}
	}		
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

void print_error(int error_code, char *arg){
	switch (error_code){
		case CD_FAIL:
			fprintf(stderr, ":570:  cd: %s: No such file or directory.\n", arg);
			break;
		case CD_INVALID_NUM_ARGS:
			fprintf(stderr, ":570:  Cd: can only take one argument.\n");
			break;
		case REDIRECTION_FILE_EXISTS:
			fprintf(stderr, ":570:  file {%s} already exists.\n", outfile);
			break;
		case OUTPUT_FILE_OPEN_ERROR:
			fprintf(stderr, ":570:  could not open output file {%s}. \n", outfile);
			break;
		case INPUT_FILE_OPEN_ERROR:
			printf(":570:  could not open input file {%s}. \n", infile);
			break;
		case OUTPUT_REDIRECTION_FAILED:
			fprintf(stderr, ":570:  output redirection failed on file {%s}.\n",arg);
			break;
		case INPUT_REDIRECTION_FAILED:
			fprintf(stderr, ":570:  input redirection failed on file {%s}.\n",arg);
			break;
		case AMBIGUOUS_INPUT_REDIRECTION:
			fprintf(stderr, ":570:  ambiguous input redirection.\n");
			break;
		case AMBIGUOUS_OUTPUT_REDIRECTION:
			fprintf(stderr, ":570:  ambiguous output redirection.\n");
			break;
		case MULTIPLE_PIPELINES:
			fprintf(stderr, ":570:  this shell can only handle 10 pipelines.\n");
			break;
		case PIPE_FORMAT_ERROR:
			fprintf(stderr, ":570:  syntax error near unexpected token `|\'.\n");
			break;
		case NO_PROCESS_AFTER_PIPE:
			fprintf(stderr, ":570:  no process provided after `|\'.\n");
			break;
		case BACKGROUND_FORMAT_ERROR:
			fprintf(stderr, ":570:  syntax error near unexpected token `&\'.\n");
			break;
		case OUTPUT_REDIRC_FORMAT_ERROR:
			fprintf(stderr, ":570:  syntax error expected file name after output redirection.\n");
			break;
		case INPUT_REDIRC_FORMAT_ERROR:
			fprintf(stderr, ":570:  syntax error expected file name after input redirection.\n");
			break;
		case NO_EXECUTABLE_AFTER_INPUT_REDIRC:
			fprintf(stderr, ":570:  cannot redirect input because no executable has been provided.\n");
			break;
		case NO_EXECUTABLE_AFTER_OUTPUT_REDIRC:
			fprintf(stderr, ":570:  cannot redirect output because no executable has been provided.\n");
			break;
		case FORK_FAILED_EXIT_CODE:
			fprintf(stderr, "Could not fork process #%s.\n",arg);
			break;
		case PIPE_OUTPUT_REDIRECT_FAILED:
			fprintf(stderr, ":570:  output redirection failed on pipeline #%s.\n",arg);
			break;
		case PIPE_INPUT_REDIRECT_FAILED:
			fprintf(stderr, ":570:  input redirection failed on pipeline #%s.\n",arg);
			break;
		case PIPE_FAILED_EXIT_CODE:
			fprintf(stderr, ":570:  pipe line #%s failed.\n",arg);
			break;
		case EXEC_FAILED_EXIT_CODE:
			fprintf(stderr, ":570:  %s: command not found.\n", arg);
			break;
		case NESTED_PIPELINE_PIPE_FAILED:
			fprintf(stderr, ":570:  Nested pipe line #%s failed.\n",arg);
		default:
			break;
	}
	clear_flags(); // clear all the flags on an error to return to neutral state.switch (error_code) {
}
void sighandler(){


}
