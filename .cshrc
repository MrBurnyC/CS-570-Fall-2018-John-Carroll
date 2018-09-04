#
umask 077
# The current directory should normally be placed LAST in the list (if at all).
setenv PATH /usr/local/bin:/bin:$HOME/bin:.
# skip the rest of this stuff if this is not a 'live' session (e.g., a script)
if ($?prompt) then
	alias	ts	'set noglob; eval `/usr/ucb/tset -s \!*`'
	set filec noclobber ignoreeof notify
# keep eight screenfuls of the most recent commands
	set history	= 176
# start with eight screenfuls from the last login session
	set savehist	= 176
# the following returns the machine name (e.g., edoras.sdsu.edu )
	set prefix	= `hostname`:
# take only the 'root' (get rid of .sdsu.edu, leave only 'edoras')
	set prefix	= $prefix:r:r
# make the prompt track the username, current directory, and command number
	set prompt	= "$prefix $cwd:t[\!]% "
# force copy and move to require confirmation before overwriting existing files
alias cp	cp -ip
alias mv	mv -i
# v repeats the last vi command.
alias v		\!vi
alias d		dirs
alias gdb	gdb -q
alias q		quota -v
# use vi to fix your command-line mistakes:
alias oops	"echo '\!-1:q' >\! /tmp/cmd$$; vi /tmp/cmd$$; source /tmp/cmd$$; rm -f /tmp/cmd$$;"
# usage: ccg prog <args>
# tries to compile prog from prog.c; if there are errors, invokes vi editor on a
# listing of errors.  Use :n to edit prog.c, then :e# to switch back and forth.
alias ccg 'gcc -o \!^ \!^.c \!:2* >& /tmp/,err.\!^:t$$;grep -v ^$ /tmp/,err.\!^:t$$ >! ,err.\!^:t;rm /tmp/,err.\!^:t$$; if !(-z ,err.\!^:t) $EDITOR ,err.\!^:t \!^.c'
# h displays the history file in reverse order, one screenful at a time.
alias h		"history -r | more"
# -F tags directories with a trailing '/', executables with '*', etc.
alias ll	ls -lF
alias la	ls -AgF
# put your favorite misspellings here
alias ce	cd
alias mroe	more
# the following change the current directory, and reset the prompt accordingly:
alias cd	'set old=$cwd; chdir \!*; set prompt = "$prefix $cwd:t[\\!]% "'
alias pu	'set old=$cwd; pushd \!*; set prompt = "$prefix $cwd:t[\\!]% "'
alias po	'set old=$cwd; popd \!*;  set prompt = "$prefix $cwd:t[\\!]% "'
# shell variable 'old' records previous directory name each time the
# current directory is changed - used to 'backup' to where you were:
alias back 	'set back=$old; set old=$cwd; cd $back; unset back; dirs'
set old = $HOME
alias ssh	'ssh -o ForwardX11=yes -o ForwardX11Trusted=yes -o ServerAliveInterval=30 -XY'
endif
