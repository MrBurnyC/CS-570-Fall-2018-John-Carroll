#include <stdio.h>
main(){
	printf("Bonjour, ");
	fflush(stdout);
	if(fork() == 0){
		printf("Sayonara\n");

	}
	else{
		printf("Adios\n");
	}
}
