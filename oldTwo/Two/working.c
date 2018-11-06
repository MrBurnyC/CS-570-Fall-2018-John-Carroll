#include "p2.h"

int main(){
	int exit_code;
	setpgid(0,0); // sets current process and children to its own process groups.
	(void)signal(SIGTERM, sighandler); //signal handler to catch SIGTERM and exit .
	char *new_argv[MAXITEM]; // addressed to parsed commands with flags to be executed .
	char line[MAXSIZE]; // holds unparsed line from stdin. 


}

int parse(char **commands , char *line){

}

void sighandler(){


}
