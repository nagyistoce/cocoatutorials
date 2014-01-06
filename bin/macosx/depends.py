#!/usr/bin/python

r"""depends.py - find the depenancies of a library

To use this:
	depends.py <pathtolibrary>
	
"""

__author__	= "Robin Mills <robin@clanmills.com>"
__date__	= "$Date: 2008/04/04 $"
__version__	= "$Id: //depot/bin/gps.py#9 $"
__credits__	= """Everybody who contributed to Python.
Especially: Guido van Rossum for creating the language.
And: Mark Lutz for the O'Reilly Books to explain it.
And: Ka-Ping Yee for the wonderful module 'pydoc'.
"""

import string
import os
import sys

##
#
def depend(libname,dependdict):
	""" recursive descent of the libraries """
	# print "depend:libname = ",libname
	if not os.path.isfile(libname):
		print "*** error NOT a FILE ",libname," ***"
	else:
		libname = os.path.abspath(libname)
		if not dependdict.has_key(libname):
			cmd = 'lipo -info "%s"' % libname
			result = ""
			for line in os.popen(cmd).readlines():     	# run lipo command
				line = line[:-1]                       	# strip '\n'
				result += line 
			
			dependdict[libname]=result
			cmd = 'otool -L "%s"' % libname         		# otool -L tells us the dependancies of this library

			for line in os.popen(cmd).readlines():      # run find command
				sep=':'
				if line.find(sep)>=0:
					mylist = string.split(line,sep)
				else:
					mylist = string.split(line)
				if len(mylist) > 1:
					depend(mylist[0],dependdict)

##
#
def NB(x):
	return x.replace(' ','_')
#
##

##
#
def UB(x):
	return x.replace('_',' ')
#
##

##
#
def tsort(dict):
	filename='/tmp/tsortXX.txt'
	file=open(filename,'w')
	for key in dict.keys():
		cmd = 'otool -L "%s"' % key		         		# otool -L tells us the dependancies of this library

		for line in os.popen(cmd).readlines():      # run find command
			mylist = string.split(line)
			if len(mylist) > 1:
				file.write('%s %s\n' % ( NB(key),NB(mylist[0]) ))
	file.close()
				
	bar = '-'*20
	print bar
	sys.stdout.flush()
	cmd='tsort "%s" 2>/dev/null' % (filename)
	# os.system(cmd)
	#print bar
	lines=os.popen(cmd).readlines()
	lines.reverse()	
	for line in lines:
		print UB(line),
	print
	print bar
#
##

##
#
def main(argv):
	""" main function of this program of course """
	dependdict = {}
	libname = argv[0]
	print "libname = ",libname
	depend(libname,dependdict)
	
	longest = 0
	for libname in dependdict.keys():
		if len(libname) > longest:
			longest = len(libname)

	longest += 1

	bPrint = False
	if bPrint:
		for libname in sorted(dependdict.keys()):
			# leave at least n spaces
			n = longest - len(libname)
			if n < 0:
				n = 0
			print libname,' '*n,dependdict[libname]
		
	tsort(dependdict)	
#
##

if __name__ == '__main__':
	main(sys.argv[1:])




##
# for file in os.popen(cmd).readlines():   # run find command
#   num  = 1
#   name = file[:-1]                       # strip '\n'
#   for line in open(name).readlines():    # scan the file
#       pos = string.find(line, "\t")
#       if  pos >= 0:
#           print name, num, pos           # report tab found
#           print '....', line[:-1]        # [:-1] strips final \n
#           print '....', ' '*pos + '*', '\n'
#       num = num+1
##

        