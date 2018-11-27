/*
 * Program #1
 * Anas Khafagi
 * cssc0043
 * John Carroll
 * CS570
 * 09-13-2017
 * Due: 09-16-2017
 * Program 1: Reads from stdin until EOF signal delimiting each word by a whitespace
    		  inserting it in the input string. The size of the word is returned from the function getword.
*/

#include "getword.h"
#define FALSE                                0
#define TRUE                                 1
#define QUOTE_MISMATCH                      -3

/*
input: pointer to a string input
output: parsed word from the input string, and size of the word
*/
int getword(char *word) {
    int word_flag = FALSE; // a flag to indicate a delimiter has been found to return the word on the next iteration
    int quote_flag = FALSE; // a flag to keep track of the quotes.
    int c, nextChar;
    int count = 0;
    while (count < STORAGE - 1 && !word_flag && (c = getchar()) !=
                                                EOF) {// reads from stdin until EOF signal and while count is less than storage -1. and while a delimiter is not found
        switch (c) {
            case '\n':
            case ';': // treats the semicolon like the newline character
                word_flag = TRUE; // set the word flag to break out of the while loop on the next iteration to return the word
                if (count !=
                    0) { // if count is not 0 return the newline character or semicolon to the stream to catch it in the next getword() call
                    ungetc(c, stdin);
                }
                break;
            case '\t':
            case ' ':
                if (quote_flag) { // if the quote flag is set add the whitespace to the string and increase the count
                    *word++ = (char) c;
                    count++;
                } else {
                    if (count != 0) { // set the word flag to break the while loop and return the word
                        word_flag = TRUE;
                    }
                }
                break;
            case '>':
                if (quote_flag) {
                    *word++ = (char) c;
                    count++;
                } else {
                    nextChar = getchar();
                    if (nextChar ==
                        '!') { // if the less than character is followed by the ! then add them to the string
                        if (count == 0) { // if the string is empty add the >! to the string
                            *word++ = (char) c;
                            *word++ = (char) nextChar;
                            count += 2;
                            word_flag = TRUE;
                        } else { // otherwise add both the '!' and the '>' back to the stream
                            ungetc(nextChar, stdin);
                            ungetc(c, stdin);
                            word_flag = TRUE;
                        }
                    } else { // if the less than character is not followed by a ! return that character to the stream and add the > to the string
                        ungetc(nextChar, stdin);
                        if (count == 0) { // if the string is empty add the > to the string
                            *word++ = (char) c;
                            count++;
                            word_flag = TRUE;
                        } else { // add the > back to the stream
                            ungetc(c, stdin);
                            word_flag = TRUE;
                        }
                    }
                }
                break;
                // parse the meta-characters as delimiters.
            case '&':
            case '<':
            case '!':
            case '|':
                if (quote_flag) {
                    *word++ = (char) c;
                    count++;
                    break;
                } else {
                    if (count == 0) {
                        *word++ = (char) c;
                        count++;
                        word_flag = TRUE;

                    } else {
                        ungetc(c, stdin);
                        word_flag = TRUE;
                    }
                    break;
                }
            case '\'':
                quote_flag = !quote_flag; // flips the quote flag to indicate the start and end of a quote
                break;
            case '\\':
                nextChar = getchar();
                if (quote_flag) { // if a backslash is followed by a quote add the quote to the string
                    if (nextChar == '\'') {
                        *word++ = (char) nextChar;
                        count++;
                    } else { // else just add the backslash and the character verbatim
                        *word++ = (char) c;
                        *word++ = (char) nextChar;
                        count += 2;
                    }
                    break;
                } else {
                    if (nextChar ==
                        '\n') { //if quote flag is not set and the character is a newline character add it back to the stream and continue
                        ungetc(nextChar, stdin);
                        continue;
                    }
                    *word++ = (char) nextChar; // else just add the character to the string without the backslash
                    count++;
                    break;
                }
            default:
                *word++ = (char) c; // adds the character to the string
                count++;
                break;
        }

    }
    *word = '\0';
    if (quote_flag) { // if quote flag is still set return QUOTE_MISMATCH exit code to indicate error.
        return QUOTE_MISMATCH;
    }
    if (count == 0 && !word_flag) {   // if count is 0 and the word flag is not set then it is EOF null terminate the string and return -1
        return EOF;
    } else {          // otherwise null terminate the string clear word flag and return the count.
        return count;

    }
}

