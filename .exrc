" sample ~/.exrc file -- comments begin with double quotes
"
" options:
set autowrite optimize redraw showmatch showmode
set wrapmargin=0 report=2
set w1200=10
"
" key mappings to create (or change) commands:
" since G moves to the end of the file, let's let g move to the beginning
map g 1G
" find the next space and replace it by a carriage return --  try 3q
map q f r
map Q F r
" save the file, feed it to the spelling checker, append the output
" from spell to the end of the file (u undoes the results, of course)
map  G:w! %.bak:r!spell %.bak
" replace the paragraph with the results of feeding
" the paragraph to the 'fmt' command (reformats the paragraph)
map  !}fmt -78
" capitalize every word on the current line
map  :s/\<[a-z]/\u&/g
" capitalize first letter of every word of the line, lowercase all other letters
map  :s/\<\([A-Za-z]\)\([A-Za-z]*\)\>/\u\1\L\2/g
" using 'map!' instead of 'map' takes effect in insert mode (not command mode)
map!  hxpa
" the above makes control-P exchange the last two characters (while inserting)
" 
" abbreviations:
ab  SUDS San Diego State University
ab  mroe more
ab  IFELSE if ( 1 )    {        }    else    {        }
