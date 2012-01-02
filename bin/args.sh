#!/bin/bash

let count=1;
for i in "$@" ; do
	echo $(printf "%02d %s" $count "$i") 
	let count=count+1
done

# That's all Folks!
##

##
# enumerate all files in a directory using ls/dir wild card when filenames contain spaces
# IFS='____';files=( `ls fo*|tr '\n' '____'` );IFS=;args ${files[@]};
# IFS=';';files=( $(ls fo*|tr '\n' ';') );IFS=;args ${files[@]};
#
# 639 rmills@rmills-mbp:~/bin $ # IFS=';';files=( `ls fo*|tr '\n' ';'` );IFS=$( echo ';'|tr ';' '\n');args ${files[@]};

# 01 foo bar
# 02 footer
# 03 formatted.txt
# 640 rmills@rmills-mbp:~/bin $
##
# Explanation
# IFS=";" sets the input file separator
# files=(....) returns a list in variable files
# ${files[@]} expands a list into words (even if word has spaces)
# ls fo*|tr '\n' ';'   # lists fo* and converts new-line to the character ';'
# 
##

