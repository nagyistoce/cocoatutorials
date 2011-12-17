#!/bin/bash

if [ ${#*} -gt 0 ]; then
	dir=$1
else
	dir=.
fi

find "$dir" -name "*~" | xargs rm -rf
