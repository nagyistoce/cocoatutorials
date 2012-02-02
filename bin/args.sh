#!/bin/bash

##
# args.sh [arg]...
##

ignore()
{
	echo >> /dev/null
}

ignore $((count=1))
for i in "$@" ; do
	echo $(printf "%2d: %s" $count "$i") 
	ignore $(( count++ ))
done

# That's all Folks!
##

