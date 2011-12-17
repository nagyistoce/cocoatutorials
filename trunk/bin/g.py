#!/usr/bin/env python
# -*- coding: Latin-1 -*-
r"""g.py - a program for adding the geotag icon to an image

"""

__author__  = "Robin Mills <robin@clanmills.com>"
__date__    = "$Date"
__version__ = "$Id: //depot/bin/g.py#17 $"
__credits__ = """Everybody who contributed to Python.
Especially: Guido van Rossum for creating the language.
And: Mark Lutz and David Ascher for the O'Reilly Books to explain it.
And: Ka-Ping Yee for the wonderful module 'pydoc'.
"""

##
# import modules
import sys
import os
import string
import glob
import os.path
from PIL import Image

##
# 
def syntax():
    """syntax - print syntax of program """
    print "syntax: g image position width output "



##
#
def g(jpeg,pos,W,out):
	icon   = os.path.join(os.path.dirname(sys.argv[0]),"geotag-128.jpg")
	print "icon = " + icon
	
	# could be options
	M = 2                # option for margin (% of size)
	I = 5                # option for icon size (% of size)

	# parse pos to find p and m
	# pos = [t | b | l | r | n | m]+
	p    = 0
	left = 0x10
	right= 0x20
	top  = 0x01
	bottom=0x02

	m = pos.__contains__('m')
	if  pos.__contains__('t'):
		p += top      
	if  pos.__contains__('b'):
		p += bottom      
	if  pos.__contains__('r'):
		p += right      
	if  pos.__contains__('l'):
		p += left      

	# unknown position
	if ( not p ):
		if not pos.__contains__('n'):
			print "pos not known: " + pos
		return

	print "p = " + str(p)
	print "opening "        \
	+ " jpeg= "     + jpeg  \
	+ " position= " + pos   \
	+ " icon= "     + icon	\
	+ " out= "      + out
	                 
	im_jpeg = Image.open(jpeg)
	im_icon = Image.open(icon)
	
	# create an image im_mask
	R, G, B = 0, 1, 2
	source = im_icon.split()
	im_mask   = source[R].point(lambda i: i > 100 and 255)
	
	w = im_jpeg.size[0]
	h = im_jpeg.size[1]
	z = int(w*I/100.0) # make the icon 5% of the size of the image
	W = int(W)
	H = int(W * h / w)
	# set margin at 2% of the size of the image
	if m:
		m = int(w*M/100.0)
	
	##
	# resize the mask and icon
	im_mask = im_mask.resize((z,z))
	im_icon = im_icon.resize((z,z))
	
	# x,y = position of the box
	# left/right - set x
	x=0
	y=0
	if p & left:
		x = m
			
	if p & right:
		x =w-z-m

	# top/bottom - set y
	if p & top:
		y = m
			
	if p & bottom:
		y = h-z-m

			
	# the icon is a square (originally 128x128)
	box      = (x, y, x+z, y+z)
	print "box = " + str(box)
	im_jpeg.paste(im_icon,box,im_mask)
	
	print "resizing the image"
	im_out = im_jpeg.resize((W,H),Image.ANTIALIAS)
	im_out.save(out, "JPEG")
	im_out.show()

##
#
if __name__ == '__main__':
    argc     = len(sys.argv)
    print "argc = " + str(argc)
    if argc  == 5:
        g(sys.argv[1],sys.argv[2],sys.argv[3],sys.argv[4])
    else:
       syntax() ;



