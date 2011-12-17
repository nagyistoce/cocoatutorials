#!/bin/bash

path=`pathname $1`
base=`basename $path`

if [ -e $path ]; then
	echo lipo -arch ppc /P$path -arch i386 $path -create -output combo/$base
	     lipo -arch ppc /P$path -arch i386 $path -create -output combo/$base
else
	echo cannot find $1
fi


