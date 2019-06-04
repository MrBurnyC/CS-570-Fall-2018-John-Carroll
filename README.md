## CS570 Operating Systems by John Carroll Fall 2018 SDSU (GO AZTECS)

- These are all the files used for the semester.
There are five program starting with program zero. The prompt is found below as it was found
in the README in class.

## File Structure 
- I pushed from SDSU's server eduoam which includes all of Dr. Carroll's files such as 
.bash , .cshrc, .mailrc and etc files that are only seen by running the command `ls -la`.

- The programs are in their corresponding directories *except* for program 2. 
 program 2 is actually program 2. This is because we used program 2 as a basic for 
 program 4. Program 2 is in the directory "oldTwo"

## Prompt for program zero:

Change your password (passwd)
and update your finger information (chfn)
to reflect your first and last name
(you don't have to enter phone numbers or other information unless you want
to -- only your first and last name is required).
Change your shell to /bin/tcsh (chsh)
Familiarize yourself with mail (mail) and the on-line man pages (man man).
I have deposited several startup files into your account
(.cshrc .tcshrc .exrc .mailrc .login .logout .vimrc README), and these files are
also publicly available under ~cs570 .  Other useful files are also contained
in that directory.  The file vi_commands summarizes the features of the
editor, and the .c and .h files correspond to code in the notes.
The files needed for your first programming assignment are under the
directory ~cs570/Zero and the file ~cs570/program0 lists the directions for
our initial mini-programming assignment. The file ~cs570/calendar lists the
due date for program0 and other relevant dates pertaining to the course.

Do the following two commands:
cd
ln -s ~cs570/calendar
...so that you will begin receiving email reminding you of the CS570 due dates.

Create a TWO-line (not 1, not 3) file called ~/.forward containing your
escaped student login name on line 1, and your preferred email address on
line 2.  For example, if I had assigned you cssc1234 and you receive mail
at myname@gmail.com, your ~/.forward file should look like:

\cssc1234,
myname@gmail.com

You MUST have a '\' just in front of the "cssc" to avoid mail loops.  Note
that '\' is NOT the same thing as '/' -- make sure you use '\' .  Some systems
want addresses to be comma-separated, so make sure you have a comma at the end
of that first line (so both '\' and ',' are significant).
