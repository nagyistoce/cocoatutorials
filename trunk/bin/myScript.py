#!/usr/bin/env python
# -*- coding: Latin-1 -*-
r"""myScript - a program for generating web pages from photographs


--------------------
Revision information: 
$Id: //depot/bin/webby.py#5 $ 
$Header: //depot/bin/webby.py#5 $ 
$Date: 2008/03/28 $ 
$DateTime: 2008/03/28 15:04:36 $ 
$Change: 83 $ 
$File: //depot/bin/webby.py $ 
$Revision: #5 $ 
$Author: rmills $
--------------------
"""

__author__	= "Robin Mills <robin@clanmills.com>"
__date__	= "$Date"
__version__ = "$Id: //depot/bin/webby.py#5 $"
__credits__ = """Everybody who contributed to Python.
Especially: Guido van Rossum for creating the language.
And: Mark Lutz and David Ascher for the O'Reilly Books to explain it.
And: Ka-Ping Yee for the wonderful module 'pydoc'.
"""

##
# import modules
import sys
import os
import pyexiv2
import datetime
import surd
import time
import datetime
import cgi
import string

##
#
def syntax():
	print "myScript configuration"


##
#
def myScript():
	"""myScript - main program of course"""
	argc	 = len(sys.argv)
	if argc	 < 2:
	   syntax() ;
	   return 
	   
	config = sys.argv[1]   
	print "config = ", config
	
	path = os.path.split(config)[0] ;
	config = os.path.split(config)[1] ;
	
	if path == '':
		path='.'
	##
	# put the current directory onto the search path
	sys.path.insert(0,path)

	module = __import__(config)
	print "config module = ",module.__file__ # eval(config + ".__file__")

	resourceDir = os.path.join(os.path.dirname(os.path.abspath(module.__file__)),config)	;
	print "resourceDir = ",resourceDir # eval(config + ".__file__")
	

##
#
if __name__ == '__main__':
	myScript()

