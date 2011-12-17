#!/usr/bin/env python
# -*- coding: Latin-1 -*-
r"""t.py - my test bed


--------------------
Revision information: 
$Id: //depot/bin/webby.py#12 $ 
$Header: //depot/bin/webby.py#12 $ 
$Date: 2008/07/11 $ 
$DateTime: 2008/07/11 13:41:48 $ 
$Change: 180 $ 
$File: //depot/bin/webby.py $ 
$Revision: #12 $ 
$Author: rmills $
--------------------
"""

__author__	= "Robin Mills <robin@clanmills.com>"
__date__	= "$Date"
__version__ = "$Id: //depot/bin/webby.py#12 $"
__credits__ = """Everybody who contributed to Python.
Especially: Guido van Rossum for creating the language.
And: Mark Lutz and David Ascher for the O'Reilly Books to explain it.
And: Ka-Ping Yee for the wonderful module 'pydoc'.
"""

##
# import modules
import os
import sys
import string

def syntax():
	print "syntax: test <CapitalizedWord>"
	
##
#
def objMerge(a,b):
	''' objMerge(a,b) -> c '''
	r = a
	for i in b.keys():
		r[i] = b[i]
	return r
##

##
#
def fixCaps(s):
	result = ""
	if len(s) > 0:
		result = s[:1]
		for c in s[1:]:
			if c.isupper():
				result += " "
			result += c
	return result
##			
	
if not len(sys.argv) == 2:
	syntax()
else:	
		
	s = sys.argv[1]
	print "argument = ",s
	s = fixCaps(s)
	print "fixed argument = ",s
	
	a = { 'a' : 10 , 'b' : 20 , 'c' : 30 }
	b = { 'a' :	 9 , 'd' : 40 }
	
	print objMerge(a,b)


from DSV import DSV

file		= open(sys.argv[1],"r")
data		= file.read()
file.close()

qualifier	= DSV.guessTextQualifier(data) # optional
data		= DSV.organizeIntoLines(data, textQualifier = qualifier)
delimiter	= DSV.guessDelimiter(data) # optional
data		= DSV.importDSV(data, delimiter = delimiter, textQualifier = qualifier)
# hasHeader = DSV.guessHeaders(data) # optional 

print "qualifier = ",qualifier
print "delimiter = ",delimiter


from wxPython.wx import *

class MyApp(wxApp):
	def OnInit(self):
		global data
		frame = wxFrame(NULL, -1, "Hello from wxPython")
		frame.Show(true)
		self.SetTopWindow(frame)
		parent = frame
		dlg = DSV.ImportWizardDialog(parent, -1, 'DSV Import Wizard', sys.argv[1])
		dlg.ShowModal()
		headers, data = dlg.ImportData() # may also return None
		dlg.Destroy()
		for d in data:
			for x in d:
				print x.encode('utf-8')
			print
		return false

app = MyApp(0)
app.MainLoop()

