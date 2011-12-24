#!/usr/bin/env python

import os
import sys
import optparse

def bar():
	print '---------------'
	
##
#
def visitor(myData, directoryName, filesInDirectory):        # called for each dir 
	"""visitor - used by getAllFiles"""
	# print "in visitor",directoryName, "myData = ",myData
	# print "filesInDirectory => ",filesInDirectory
	for filename in filesInDirectory:
		pathname = os.path.join(directoryName, filename)     # fnames have no dirpath
		if os.path.exists(pathname) and os.path.isfile(pathname):
			myData[pathname]=pathname
##

##
#
def getAllFiles(startdir):
	"""searcher - walk the tree"""
	# recursive search
	myData = {}
	os.path.walk(startdir, visitor, myData)
	return myData
##

##
#
def printError(message,error):
	if not error:
		print "***",message,"***"
	return error+1
	
##
# main entry point of course
def main(argv):
	error=0
	
	parser = optparse.OptionParser()
	parser.add_option("-f", "--filename", action="store", type="string", dest="filename")
	parser.add_option("-d", "--debug", action="store_true", dest="debug")
	(options, args) = parser.parse_args(argv)
	
	if options.debug:
		for arg in args:
			print "arg = ",arg
	
		bar()
		print "options =>>>>--- ",options, 'type of options = ',type(options)
		print 'type of dir(options)', type(dir(options))
		for o in dir(options):
			print 'o = ',o
		bar()
		
		print parser.option_list
		for opt in parser.option_list:
			print opt
		
		print "filename = ",options.filename, 'of type',type(options.filename)
		
		print parser;
		bar()
	
	if len(args)==1 and not os.path.isdir(args[0]):
		error = printError("%s is not a directory"%args[0],error)

	if len(args) != 1:
		message = "only one directory may be given" if len(args) else "no directory given"
		error = printError(message,error)

	if not error:
		for arg in args:
			filedict = getAllFiles(arg)
			for file in filedict.keys():
				print file
##

##
# called from the command-line
if __name__ == '__main__':
	main(sys.argv[1:])	

# That's all Folks!
##
