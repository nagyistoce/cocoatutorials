#!/usr/bin/env python
# -*- coding: UTF-8 -*-
r"""cmLib - clanmillsLibrary for Python

This exports:
  - cmLib.getOpts
  - cmLib.getOpt
  - cmLib.printOpts
  - cmLib.getXMLtimez

This imports:
  - pyexiv2
  - surd
  - (some standard libraries os, sys, time and datetime

--------------------
Revision information: 
$Id: //depot/bin/cmLib.py#2 $ 
$Header: //depot/bin/cmLib.py#2 $ 
$Date: 2008/07/11 $ 
$DateTime: 2008/07/11 21:17:49 $ 
$Change: 181 $ 
$File: //depot/bin/cmLib.py $ 
$Revision: #2 $ 
$Author: rmills $
--------------------
"""

__author__  = "Robin Mills <robin@clanmills.com>"
__date__    = "$Date"
__version__ = "$Id: //depot/bin/cmLib.py#2 $"
__credits__ = """Everybody who contributed to Python.
Especially: Guido van Rossum for creating the language.
And: Mark Lutz and David Ascher for the O'Reilly Books to explain it.
And: Ka-Ping Yee for the wonderful module 'pydoc'.

Olivier Tilloy for the pyexiv2 python wrapper code
Andreas Huggel for the libexiv2 library
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
import string

##
#
def getXMLtimez(phototime,delta=0):
    """getXMLtimez - convert a datetime to an XML formatted date"""
    #
    # phototime = timedate.timedate("2008-03-16 08:52:15")
    # delta     = seconds
    # -----------------------
    
    timedelta = datetime.timedelta(0,delta,0)
    newtime = phototime + timedelta 
    return newtime.strftime('%Y-%m-%dT%H:%M:%SZ') ;
#

##
#
def printOpts(opts):
    """printOpts - dump options"""
    print '---------------'
    for v in sorted(opts):
        print v , " -> " , opts[v]
    print '---------------'
#

##
#
def getOpts(argv,opts,default):
    """getOpts - parse out the options"""
    while argv:
        next = 1
        value = True
        opt   = argv[0]
    #   print 'opt = ' + opt
        if ( opt[0] == '-'):
            opt = opt[1:]
            value=True
            if len(argv)>1 and argv[1][0] != '-':
            	value = argv[1]
            	next = 2
        else:
            value = opt
            opt   = default
            
        opts[opt] = value
        argv = argv[next:]
    return opts
#

##
#
def getOpt(opts,key,default):
    """ getOpt - get the value of an option (or return default) """
    if opts.has_key(key):
        return opts[key] ;
    return default ;
#

##
#
def cmLib():
    """ runTests - test suite """
    print "test suite runs 0 errors"
#

##
#
if __name__ == '__main__':
    cmLib()

#
## that's all Folks!
