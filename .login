stty -lcase -tabs erase '^?' intr '^C' kill '^U' rows 24 cols 80

setenv PATH /usr/local/bin:/bin:$HOME/bin:.
setenv	SHELL	$shell
setenv	PAGER	"/bin/more"
setenv	EDITOR	"/bin/vim"
#the following can be specified in ~/.exrc or ~/.vimrc
#setenv EXINIT "set shell=/bin/csh showmatch redraw optimize beautify | map g 1G| map v \!}fmt | map V \!{fmt | ab SUDS San Diego State University"
unset	autologout


#mesg n
# uncomment the previous line to disable "talk" and "write" messages
# directed toward your terminal
