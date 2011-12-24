#!/usr/bin/env python

import os
import sys
import optparse

def bar():
	print '---------------'

def main(argv):
	print "hello world argv=",argv,'len = ',len(argv)
	
	parser = optparse.OptionParser()
	parser.add_option("-f", "--filename", action="store", type="string", dest="filename")
	
	(options, args) = parser.parse_args(argv)
	
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

if __name__ == '__main__':
	main(sys.argv[1:])	

# That's all Folks!
##