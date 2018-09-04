/*             inout2.c
 * Class Info- Bernardo Cervantes , John Carroll, CS570 ,
 *	       Due: 9/5/18 @ 8PM		 
 * Synopsis  - Takes input from the keyboard and echoes that
 *             input back to the terminal.
 *
 * Objective - Illustrates the use of an assignment statement
 *             as part of the test expression in a while loop.
 */

/* Include Files */
#include <stdio.h>
#include "getword.h"


int getword(char *w){
    int wordCount = 0; //Must initalize to zero or else you get garbage.
    int iochar = 0;
    int true = 0;      //bool for checking '$'
    int true2 = 0;
    iochar = getchar();

    while(iochar == ' '){
	iochar = getchar();	
    }

    if(iochar == EOF){
	*w = '\0';
	return -255;
    }

   // if(iochar ==' '){
   //	iochar == getchar();
   // }

    if(iochar == '\n'){
	*w = '\0';
	return 0;
    }
    
    if(iochar == ';'){
	*w = '\0';
	return 0;
    }
    
    if(iochar == '$'){
	true = 1;
	iochar = getchar();
	if(iochar == EOF){
		return 0;
	}
}

    for(;;){

	if(iochar == EOF){
		//*w = '\0';
		if(wordCount < 0 || wordCount > 0){
			iochar = ungetc(iochar,stdin);
			*w = '\0';
			//if( wordCount < 0){
			//	wordCount = wordCount * -1;
			//	w += wordCount;
			//}
			//w +=  wordCount;
			//*w = '\0';	
			return wordCount;
		}

		//w++;
		//*w = EOF;
		return -255; 
	}

	if(iochar == ' '){
		*w = '\0';
		return wordCount;
	}

	if(iochar == '\n'|| iochar == ';'){
		iochar = ungetc(iochar,stdin);
		*w = '\0';
		return wordCount;
	}
	
	*w = iochar;
	w++;
	
	if(true == 1){
		wordCount--;
		//wordCount = -wordCount;
	}

	else{
		wordCount++;
	}
	
	iochar = getchar();

			       		
    }

}
