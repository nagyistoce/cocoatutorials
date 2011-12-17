#!/usr/bin/python
import sys 
import os

print "hello world from python"

# remove the output, change to the correct directory and execute shell -c testAll()

os.system("rm ~/bin/doug.txt") ;
os.chdir("/Scripting/gptech/extendscript/public/libraries/macintosh/debug") ;
os.system("shell -c testAll\\(\\)  < ~/bin/doug.js > ~/bin/doug.txt 2> ~/bin/stderr.txt" ) ;

print "good night from python"
