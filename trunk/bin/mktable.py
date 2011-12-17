#!/usr/bin/env python

r"""mktable - a to generate an HTML table of images

	
--------------------
Revision information: 
	$Id: //depot/bin/mktable.py#2 $ 
	$Header: //depot/bin/mktable.py#2 $ 
	$Date: 2008/04/29 $ 
	$DateTime: 2008/04/29 01:33:50 $ 
	$Change: 127 $ 
	$File: //depot/bin/mktable.py $ 
	$Revision: #2 $ 
	$Author: rmills $
--------------------
"""

__author__	= "Robin Mills <robin@clanmills.com>"
__date__	= "$Date: 2008/04/29 $"
__version__	= "$Id: //depot/bin/mktable.py#2 $"
__credits__	= """Everybody who contributed to Python.

Especially: Guido van Rossum for creating the language.
And: Mark Lutz for the O'Reilly Books to explain it.
And: Ka-Ping Yee for the wonderful module 'pydoc'.

Olivier Tilloy for the pyexiv2 python wrapper code
Andreas Huggel for the libexiv2 library
"""

import sys
import os
import glob
import pyexiv2
import time
import datetime
import xml.dom.minidom
import cgi

import cmLib

def pathTo(opts,path):
	""" pathTo - get the path to a path """
	u = 'u'
	path = os.path.basename(path)
	if opts.has_key(u):
		path = opts[u] + "/" + path 
	return cgi.escape(path,True)


def syntax(all):
	print "syntax: mktable <path> [options] [-h]"
	if all:
		print "options"
		print "-h        help"
		print "-landscape"
		print "-portrait"
	return

def mktable():
	##
	# get the command line arguments
	args = sys.argv[1:] # ignore the name of the program
	argc = len(args)
	path = 'path'
	opts = \
	{	'n'			: 3
	,	'landscape' : True
	,	'portrait'  : True
	}
	opts[path] = []
	
	opts = cmLib.getOpts(args,opts,path)
#	cmLib.printOpts(opts)
	#

	##
	# provide help if requested
	if 0 == len(opts[path]):
		syntax(opts.has_key('h'))
	
	##
	# find all the images in the given paths
	imagedict = {} # where are the images?
	paths = opts[path]
	
	
	for path in paths:
		path += "/*"
		for file in glob.glob(path):
			path = os.path.abspath(file)
			if os.path.isfile(path):
				try:
					image = pyexiv2.Image(path)
					image.readMetadata()
					timestamp = image['Exif.Image.DateTime']
					xmlDate = cmLib.getXMLtimez(timestamp)
					x = image['Exif.Photo.PixelXDimension']
					y = image['Exif.Photo.PixelYDimension']
					
					
					b = (cmLib.getOpt(opts,'landscape') and x >= y) or (cmLib.getOpt(opts,'portrait') and y > x )
#					print "x,y,b = ",x,y,b
					
					if b:
						imagedict[xmlDate] = [ path, file] 
					#else:
					#	print "*** ignoring image ",path, " ***"

				except:
					print "*** unable to work with ",path , " ***"
	#
	
	
	##
	# really do the business
	if len(imagedict) > 0:
		count 	= 0
		n       = int(float(opts['n']))
		if n < 1:
			n   = 1
			
		print "<table><tr>"
		for xmlDate in sorted(imagedict.keys()):
			value 	= imagedict[xmlDate] 
			path 	= value[0]
			file 	= value[1]
			if count == 0:
				print "<tr>"
				
			if 0 == (count % n):
				print "</tr><tr>"
				count = 0
		
			print '  <td><img src="'+pathTo(opts,path)+'"></td>'
			count  += 1
		
		print "</tr></table>"
	#

#
##

if __name__ == '__main__':
	mktable()
