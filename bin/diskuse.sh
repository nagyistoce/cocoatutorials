#!/bin/bash

f=/tmp/diskuse.txt
F=/tmp/diskuse.old
L=100000
T=60

while [ true ]
do
	df -g >> $f
	size=`ls -alt $f | cut -d' ' -f 8`
	if [ $size -gt $L ]; then
		rm -rf $F
		mv  $f $F
		echo "swapped"
		ls -alt /tmp/disk*
	fi
	sleep $T
done

# That's all Folks!!
##
