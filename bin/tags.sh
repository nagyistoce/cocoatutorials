#!/bin/bash

for t in $(exiftool -list)
do
	if [ $t == 'Command-line' ]; then
	 	break
	elif [[ $t != 'Available' && $t != 'tags:' ]]; then 
	 	echo $t
	fi
done | sort
