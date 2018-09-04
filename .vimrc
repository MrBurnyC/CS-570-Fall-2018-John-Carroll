set undolevels=0 compatible
map!  Bi{\tt Ea}
map  !G tr -d '\015' 
map  :s/\<\([A-Za-z]\)\([A-Za-z]*\)\>/\u\1\L\2/g
map g 1G
map!  hxpa
map q f r
map Q F r:s/[ 	]*$//
map!  /: *$A
map v !}fmt -78
map V :%s/=0D$//
map  :1,$s/^| *$//
set autowrite optimize redraw showmatch showmode
set report=2
set w1200=10
ab  SUDS San Diego State University
ab  mroe more
ab  MAIN main(){}
ab  IFELSE if ( 1 ) then    {        }    else    {        }
