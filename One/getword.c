/*getword.c  - is a lexical analyzer that meets the requirements
 *	       of Program 0.
 *
 * Class Info- Bernardo Cervantes , John Carroll, CS570 ,
 *	       Due: 9/5/18 @ 8PM
 *		 
 * Synopsis  - Takes input from the keyboard (stdin) or from
 * 	       Data0/input# and writes to the Storage array 
 *	       and returns a int back to p0.
 *
 * Objective - Returns the correct input back to p0 so that 
 *             p0 will print out the correct int and string
 *	       that is identical to the same output in the 
 *	       p0 prompt and the what autograder produces.
 *             
 */

/* Include Files */
#include <stdio.h>
#include "getword.h"


int getword(char *w){
	int wordCount = 0; //Must initalize to zero or else you get garbage.
	int iochar = 0;
	int true = 0;      //bool for checking '$'

	/* Stores intital char */

	iochar = getchar(); 

	/* Skipping leading spaces. */

	while(iochar == ' '){
		iochar = getchar();	
	}

	/* Check if EOF signal is in the beginning
	 * and puts null char in array (storage) to print
	 * an empty string. 
	 * Returns -255
	 */

	if(iochar == EOF){
		*w = '\0';    
		return -255;
	}

	/* Checks if newline is in the beginning of the input.
	 * Puts null char in array to print empty string.
	 * Returns 0
	 */

	if(iochar == '\n'){
		*w = '\0';
		return 0;
	}

	/*
	 * Checks if ";" is first char  
	 * in input. Puts null char in array to print
	 * null empty string.
	 * Return 0 
	 */ 

	if(iochar == ';'){
		*w = '\0';
		return 0;
	}


	/*
	 * Checks if "$" is first char  
	 * in input. Puts null char in array to print
	 * null empty string. Gets next char and checks 
	 * if it is EOF. Sets boolean true to 1 or true.
	 * Return 0 
	 */ 

	if(iochar == '$'){
		true = 1;
		iochar = getchar();
		if(iochar == EOF){
			return 0;
		}
	}

	/*
	 * Infinite for loop is used to get any remaining.  
	 * characters with exception chars accounted for such as
	 * EOF spaces, newlines and semicolons which are the same
	 * as the first checks but this time we return wordCount since
	 * these characters are not the first char in the words anymore.
         * when the word is processed the storage array has been written
	 * with the word or an empty string based on some execptions
	 * and the wordcount is returned which stops the for loop.    
	 */ 

	for(;;){

		/* Checks for EOF. If wordCount is anything but zero. It returns
		 * the word count. If not it returns -255. 
		 */
	
		if(iochar == EOF){
			if(wordCount < 0 || wordCount > 0){
				iochar = ungetc(iochar,stdin);
				*w = '\0';
				return wordCount;
			}

			return -255; 
		}

		/* Checks for space. If found, write null char to 
		 * array and 
		 * return wordCount.
		 */

		if(iochar == ' '){
			*w = '\0';
			return wordCount;
		}


		/* Checks for newline or semicolon. 
		 * If found, write null char to 
		 * array and 
		 * return wordCount.
		 */

		if(iochar == '\n'|| iochar == ';'){
			iochar = ungetc(iochar,stdin);
			*w = '\0';
			return wordCount;
		}


		/* Writes current iochar to array.
		 * Increments pointer to next array index. 
		 */

		*w = iochar;
		w++;

		/* Check if $ was at the beginning of a word.
		 * If true then wordCount is decremented 
		 *  and is negative. If not then the wordCount 
		 *  is incremented and is postive. 
		 */

		if(true == 1){
			wordCount--;
		}

		else{
			wordCount++;
		}
		/*Gets next char and repeats loop. */

		iochar = getchar();


	}

}
