#!/usr/bin/env python
# -*- coding: Latin-1 -*-
r"""readPDF - convert a PDF file to ascii

This exports:
  - nothing yet

This imports:
  - pyPdf

This optionally can use
  - nothing yet

--------------------
Revision information: 
$Id: //depot/bin/webby.py#9 $ 
$Header: //depot/bin/webby.py#9 $ 
$Date: 2008/05/01 $ 
$DateTime: 2008/05/01 00:41:29 $ 
$Change: 128 $ 
$File: //depot/bin/webby.py $ 
$Revision: #9 $ 
$Author: rmills $
--------------------
"""

__author__	= "Robin Mills <robin@clanmills.com>"
__date__	= "$Date"
__version__ = "$Id: //depot/bin/webby.py#9 $"
__credits__ = """Everybody who contributed to Python.
Especially: Guido van Rossum for creating the language.
And: Mark Lutz and David Ascher for the O'Reilly Books to explain it.
And: Ka-Ping Yee for the wonderful module 'pydoc'.
"""

##
# import modules

import sys
import pyPdf



##
#
def getPDFContent(path):
	"""getPDFContent - read the text from a PDF file"""
	content = ""
	
	# Load PDF into pyPDF
	pdf = pyPdf.PdfFileReader(file(path, "rb"))
	
	# Iterate pages
	for i in range(0, pdf.getNumPages()):
		# Extract text from page and add to content
		content += pdf.getPage(i).extractText() + "\n"
	
	# Collapse whitespace
	content = " ".join(content.replace("\\xa0", " ").strip().split())
	
	return content
#


##
#
if __name__ == '__main__':
	pdf = sys.argv[1]
	print getPDFContent(pdf).encode('utf-8')

