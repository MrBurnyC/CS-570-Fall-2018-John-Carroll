/* getword.h - header file for the getword() function used in
   CS570 Fall 2017
   San Diego State University
*/

#include <stdio.h>
#include <string.h>
#include <strings.h>

#define STORAGE 255
      /* This is one more than the max wordsize that getword() can handle */

int getword(char *w);
/* (Note: the preceding line is an ANSI C prototype statement for getword().
    It will work fine with edoras' C compilers.)

* The getword() function gets one word from the input stream.
* It returns -1 iff end-of-file is encountered;
* otherwise, it returns the number of characters in the word
*
* INPUT: a pointer to the beginning of a character string
* OUTPUT: -1 or the number of characters in the word
* SIDE EFFECTS: bytes beginning at address w will be overwritten.
*   Anyone using this routine should have w pointing to an
*   available area at least STORAGE bytes long before calling getword().

Upon return, the string pointed to by w contains the next word in the line
from stdin. A "word" is a string containing a single metacharacter or a string
consisting of non-metacharacters delimited by tabs, blanks, or metacharacters.

The metacharacters are "<", ">", "|", ";", "'", "&", and the combination ">!" .
The last word on a line may be terminated by the newline character OR by
end-of-file.  Word collection is "greedy": getword() always tries each time
to read the largest word that does not violate the rules.  For example, >!foo
is parsed as ">!" and then "foo", NOT as ">" and "!foo" .

getword() skips leading tabs and blanks, so if getword() is called and there are
no more words on the line, then w points to an empty string. All strings,
including the empty string, will be delimited by a zero-byte (eight 0-bits),
as per the normal C convention (this delimiter is not 'counted' when determining
the length of the string that getword will report as a return value).

The backslash character "\" is special, and may change the behavior of
the character that directly follows it on the input line.  When "\" precedes
a metacharacter, that metacharacter is treated like most other characters.
(That is, the symbol will be part of a word rather than a word delimiter.)

Thus, three calls applied to the input
Null&void
will return 4,1,4 and produce the strings "Null", "&", "void", respectively.

However, one call to getword() applied to the input
Null\&void
returns 9 and produces the string "Null&void".
Note that the '\' is NOT part of the resulting string!

Similarly, "\<" is treated as the [non-meta]character "<", "\>" is ">",
"\&" is "&", "\|" is "|", and "\\" represents the [non-special] character "\".
The combination "\ " should be treated as " ", and therefore
allow a space to be embedded in a word:
Null\ void
returns 9 and produces the string "Null void".
(A backslash preceding any other character should simply be ignored; in
particular, a backslash before a newline will not affect the meaning
of that newline.)  To keep things from getting too complicated, I promise never
to test stuff like "\>!" or ">\!" ...

Another way to allow spaces (and almost anything else) inside a word is to use
pairs of single quotes ("'").  Thus
'Null void'
also returns 9 and produces the string "Null void".  (Note that, like the "\",
the two "'" marks are NOT part of the word -- if we really wanted a "'" in a
word, you'd have to specify it with \', of course.)  In program2, seeing an
opening quote but no corresponding closing quote will be flagged as an error,
but for now, a string starting with the metacharacter "'" will terminate
if an unexpected newline (or EOF) is encountered.  Note that
'Null\'void'
again returns 9 but produces the string "Null'void".
In all other cases, a "\" inside a quoted string is an actual "\".  For example,
'7\>3'
returns 4 and produces the string "7\>3" (whereas '7\'3' produces only "7'3").
Note also that a closing quote does not automatically end a word; it just means
that you should stop using the special rules that apply "inside" quotes.

The integer that getword() returns is the length of the resultant
string to which w points. There is one exception to this: If the rest of
the line consists of zero or more blanks/tabs followed by end-of-file, then w
still points to an empty string, but the returned integer is (-1).

Example: Suppose the input line were
Hi there&
(Assume there are trailing blanks, followed by EOF.)
Four calls to getword(w) would return 2,5,1,-1 and fill each of the
areas pointed to by w with the strings "Hi", "there", "&", and "",
respectively.

Example: Suppose the input line were
Hi there&		
(Assume there are two trailing tabs, followed by a newline character.)
Four calls to getword(w) would return 2,5,1,0 and fill each of the
areas pointed to by w with the strings "Hi", "there", "&", and "",
respectively.  (If EOF followed the newline, then a fifth call would
produce "" and return -1.)

Note that we would obtain exactly the same results if the input
line had been
    Hi   there		&
(This example has leading blanks and a newline right after the ampersand.)

The metacharacter ";" should be treated EXACTLY like a newline.  In the
earlier example, getword() returned zero when it encountered two blanks
and a newline, and left a null string in the storage area pointed to by w.
Similarly, if getword() encounters two blanks followed by a semicolon,
it should likewise return zero and leave a null string in the storage area
pointed to by w.  (It does NOT return a 1, and it does NOT put ";" in the
the storage array.)

If the word scanned is longer than STORAGE-1, then getword() constructs the
string consisting of the first STORAGE-1 bytes only. (As usual, a zero-byte
is appended. The next getword() call will begin with the rest of that word.)

Useful manpages to consider are those for ungetc() and getchar().

*/
/* getword.h - header file for the getword() function used in
   CS570 Fall 2017
   San Diego State University
*/

#include <stdio.h>
#include <string.h>
#include <strings.h>

#define STORAGE 255
      /* This is one more than the max wordsize that getword() can handle */

int getword(char *w);
/* (Note: the preceding line is an ANSI C prototype statement for getword().
    It will work fine with edoras' C compilers.)

* The getword() function gets one word from the input stream.
* It returns -1 iff end-of-file is encountered;
* otherwise, it returns the number of characters in the word
*
* INPUT: a pointer to the beginning of a character string
* OUTPUT: -1 or the number of characters in the word
* SIDE EFFECTS: bytes beginning at address w will be overwritten.
*   Anyone using this routine should have w pointing to an
*   available area at least STORAGE bytes long before calling getword().

Upon return, the string pointed to by w contains the next word in the line
from stdin. A "word" is a string containing a single metacharacter or a string
consisting of non-metacharacters delimited by tabs, blanks, or metacharacters.

The metacharacters are "<", ">", "|", ";", "'", "&", and the combination ">!" .
The last word on a line may be terminated by the newline character OR by
end-of-file.  Word collection is "greedy": getword() always tries each time
to read the largest word that does not violate the rules.  For example, >!foo
is parsed as ">!" and then "foo", NOT as ">" and "!foo" .

getword() skips leading tabs and blanks, so if getword() is called and there are
no more words on the line, then w points to an empty string. All strings,
including the empty string, will be delimited by a zero-byte (eight 0-bits),
as per the normal C convention (this delimiter is not 'counted' when determining
the length of the string that getword will report as a return value).

The backslash character "\" is special, and may change the behavior of
the character that directly follows it on the input line.  When "\" precedes
a metacharacter, that metacharacter is treated like most other characters.
(That is, the symbol will be part of a word rather than a word delimiter.)

Thus, three calls applied to the input
Null&void
will return 4,1,4 and produce the strings "Null", "&", "void", respectively.

However, one call to getword() applied to the input
Null\&void
returns 9 and produces the string "Null&void".
Note that the '\' is NOT part of the resulting string!

Similarly, "\<" is treated as the [non-meta]character "<", "\>" is ">",
"\&" is "&", "\|" is "|", and "\\" represents the [non-special] character "\".
The combination "\ " should be treated as " ", and therefore
allow a space to be embedded in a word:
Null\ void
returns 9 and produces the string "Null void".
(A backslash preceding any other character should simply be ignored; in
particular, a backslash before a newline will not affect the meaning
of that newline.)  To keep things from getting too complicated, I promise never
to test stuff like "\>!" or ">\!" ...

Another way to allow spaces (and almost anything else) inside a word is to use
pairs of single quotes ("'").  Thus
'Null void'
also returns 9 and produces the string "Null void".  (Note that, like the "\",
the two "'" marks are NOT part of the word -- if we really wanted a "'" in a
word, you'd have to specify it with \', of course.)  In program2, seeing an
opening quote but no corresponding closing quote will be flagged as an error,
but for now, a string starting with the metacharacter "'" will terminate
if an unexpected newline (or EOF) is encountered.  Note that
'Null\'void'
again returns 9 but produces the string "Null'void".
In all other cases, a "\" inside a quoted string is an actual "\".  For example,
'7\>3'
returns 4 and produces the string "7\>3" (whereas '7\'3' produces only "7'3").
Note also that a closing quote does not automatically end a word; it just means
that you should stop using the special rules that apply "inside" quotes.

The integer that getword() returns is the length of the resultant
string to which w points. There is one exception to this: If the rest of
the line consists of zero or more blanks/tabs followed by end-of-file, then w
still points to an empty string, but the returned integer is (-1).

Example: Suppose the input line were
Hi there&
(Assume there are trailing blanks, followed by EOF.)
Four calls to getword(w) would return 2,5,1,-1 and fill each of the
areas pointed to by w with the strings "Hi", "there", "&", and "",
respectively.

Example: Suppose the input line were
Hi there&		
(Assume there are two trailing tabs, followed by a newline character.)
Four calls to getword(w) would return 2,5,1,0 and fill each of the
areas pointed to by w with the strings "Hi", "there", "&", and "",
respectively.  (If EOF followed the newline, then a fifth call would
produce "" and return -1.)

Note that we would obtain exactly the same results if the input
line had been
    Hi   there		&
(This example has leading blanks and a newline right after the ampersand.)

The metacharacter ";" should be treated EXACTLY like a newline.  In the
earlier example, getword() returned zero when it encountered two blanks
and a newline, and left a null string in the storage area pointed to by w.
Similarly, if getword() encounters two blanks followed by a semicolon,
it should likewise return zero and leave a null string in the storage area
pointed to by w.  (It does NOT return a 1, and it does NOT put ";" in the
the storage array.)

If the word scanned is longer than STORAGE-1, then getword() constructs the
string consisting of the first STORAGE-1 bytes only. (As usual, a zero-byte
is appended. The next getword() call will begin with the rest of that word.)

Useful manpages to consider are those for ungetc() and getchar().

*/

