#!/usr/bin/python
# -*- coding: Latin-1 -*-
r"""threes - find HDR candidates

This exports:
  - threes.syntax - report syntax

This imports:
  - cmLib.py = clanmills library (to be replaced soon)
  - PIL (from Image Library)


"""

__author__  = "Robin Mills <robin@clanmills.com>"
__date__    = "$Date"
__version__ = "$Id: //depot/bin/webby.py#17 $"
__credits__ = """Everybody who contributed to Python.
Especially: Guido van Rossum for creating the language.
And: Mark Lutz and David Ascher for the O'Reilly Books to explain it.
And: Ka-Ping Yee for the wonderful module 'pydoc'.
"""

##
# import modules
import sys
import os
import stat
import pyexiv2
import datetime
import surd
import time
import datetime
import cgi
import string
import glob
import subprocess
from PIL import Image

import cmLib

cwd          = os.getcwd()
now          = datetime.datetime.now().ctime()
width        = 750

fcount       = 0
vcount       = 0
hdrDir       = "HDR"
photoDir     = "."

##
# change path to enable import from the current directory
sys.path.insert(0,'.')

subs = {}

#
# use a dictionary to hold the data for every pathname
filedict = {}      # pathname : [ filename,xmlDate,datetime ]
datedict = {}      # date     : [ pathname ... ]

#
##

##
def syntax():
    """syntax - print syntax of webby.py """
    print "syntax: threes [[-photos] <photoDirectory>] [-hdrdir <hdrdir>]]+ "

##
def getXMLtimez(phototime):
    """convert a datetime to XML standard date string"""
    #
    # s = "2008-03-16 08:52:15"
    # -----------------------
    # get the  timezone and time offset the Zulu time
    delta   = time.altzone if time.daylight == 1 else time.timezone
    name    = time.tzname[time.daylight]

    # print delta, "secs = ",delta/60,"mins = ",delta/3600,"hrs tzone = ",name
    timedelta = datetime.timedelta(0,delta,0)
    # print timedelta #  , " => " , timedelta.__class__

    # -----------------------
    # parse a date string
    # photo       = s # "2008-03-16 08:52:15"
    # phototime  = datetime.datetime.strptime(photo,'%Y-%m-%d %H:%M:%S')

    newtime = phototime + timedelta 
    # print newtime #, " => ",newtime.__class__

    return newtime.strftime('%Y-%m-%dT%H:%M:%SZ') ;
##

##
def visitfile(filename,pathname,myData):
    """visitfile - use by the directory walker"""
    # print "filename,pathname = " , filename , " -> ",pathname
    global fcount, vcount
    aspect = 3.0 / 4.0
    try:
        image = pyexiv2.Image(pathname)
        image.readMetadata()
        
        try:
            timestamp = image['Exif.Photo.DateTimeOriginal'] # 'Exif.Image.DateTime']
        except:
            timestamp = datetime.datetime.fromtimestamp(os.stat(pathname)[stat.ST_MTIME])
        
        try:
            aspect = float(image['Exif.Photo.PixelYDimension']) / float(image['Exif.Photo.PixelXDimension'])
        except:pass
        size=os.stat(pathname)[stat.ST_SIZE]
        
        xmlDate = getXMLtimez(timestamp)
        fcount = fcount + 1
        
        zdict = {}
        for k in image.exifKeys():
        	zdict[k]=k
        sKey = 	'Exif.Photo.ExposureMode'
        if zdict.has_key(sKey):
        	if image[sKey] == 2: # 'Auto bracket'
	        	filedict[pathname] = [filename, xmlDate,timestamp,image,aspect]
        		if not datedict.has_key(xmlDate):
        			datedict[xmlDate]=[]
        		t=timestamp.timetuple()
        		datedict[xmlDate].append( { 'path': pathname , 'timestamp' : time.mktime(t) , 'size' : size });
    except:
    #   print "*** TROUBLE with " + pathname + " ***"
        pass
    vcount += 1
##

##
def visitor(myData, directoryName, filesInDirectory):        # called for each dir 
    """visitor - used by the directory walker"""
    print "in visitor:",directoryName, "myData = ",myData,"hdrDir = ",hdrDir
#   print "filesInDirectory => ",filesInDirectory

    for filename in filesInDirectory:                        # do non-dir files here
        pathname = os.path.join(directoryName, filename)     # fnames have no dirpath
        bIgnore  = directoryName.find('picasaoriginals')>0 \
                or os.path.isdir(pathname)                 \
                or filename[:1] == '.'
                
        if not bIgnore:
        	visitfile(filename,pathname,myData)
##

##
def searcher(startdir,myData):
    """searcher - walk the tree"""
    # recursive search
    global fcount, vcount
    fcount = vcount = 0
    myData = startdir
    os.path.walk(startdir, visitor, myData)
##

##
def threes(opts):
    """threes - main program of course"""

    photoDir = cmLib.getOpt(opts,'photos',os.path.expanduser('~/Pictures'))
    hdrDir   = cmLib.getOpt(opts,'hdrdir',os.path.join(cwd,"HDR"))
    
    photoDir = os.path.abspath(photoDir) 
    
    # test that we have things to proceed
    if not os.path.isdir(photoDir):
        print photoDir + ' is not a directory'
        return
    if not os.path.isdir(hdrDir):
    	os.mkdir(hdrDir)
    if not os.path.isdir(hdrDir):
        print hdrDir + ' is not a directory'
        return
    

    print "photoDir =" ,photoDir, "hdrDir =",hdrDir
    myData = 10
    
    ##
    if 1:   
        searcher(photoDir,myData)
        print 'Found %d files, visited %d' % (fcount, vcount)
        
        olddate = 0 ;
        
        groups=[]
        batch=[]

        ##
        # sort the files into groups
        for k in sorted(datedict.keys()): # ,cmpDate):
         	for p in datedict[k]:
         		newdate=p['timestamp']
         		d=newdate-olddate
         		path=p['path']
         		if d>3:
         			if len(batch)>1:
         				groups.append(batch)
         			batch=[]
         			olddate=newdate
         		bAdd=True
         		for x in batch:
         			if x['size']==p['size']:
         				bAdd=False
         		if bAdd:
         			batch.append(p)

         		# print k,p['path'],newdate-olddate
         		# olddate=newdate
         		
		##
		# pump the groups into hdrDir
		file=0
        for g in groups:
			if len(g)==3:
				print g[0]['path']
				for p in g:
					file=file+1
					subprocess.call([ 'cp',p['path'],os.path.join(hdrDir,("%04d.jpg"%file) ) ])
				file=file+7
			else:
				print g[0]['path'],len(g)
        ##

# 
##

##
if __name__ == '__main__':
    argc     = len(sys.argv)
    print "hello world"
    if argc  > 1:
        opts = cmLib.getOpts(sys.argv[1:],{},'photos')
        cmLib.printOpts(opts)

        threes(opts)
    else:
       syntax() ;


#  C:\Users\rmills\Pictures\TripHome\Test>exiv2 -pt A.jpg B.jpg C.jpg | grep -i expos
#  A.jpg                 Exif.Photo.ExposureTime                      Rational    1  1/320 s
#  A.jpg                 Exif.Photo.ExposureBiasValue                 SRational   1  0
#  A.jpg                 Exif.Photo.ExposureMode                      Short       1  Auto bracket
#  A.jpg                 Exif.CanonCs.ExposureProgram                 Short       1  Program (P)

#  B.jpg                 Exif.Photo.ExposureTime                      Rational    1  1/640 s
#  B.jpg                 Exif.Photo.ExposureBiasValue                 SRational   1  -1
#  B.jpg                 Exif.Photo.ExposureMode                      Short       1  Auto bracket
#  B.jpg                 Exif.CanonCs.ExposureProgram                 Short       1  Program (P)

#  C.jpg                 Exif.Photo.ExposureTime                      Rational    1  1/160 s
#  C.jpg                 Exif.Photo.ExposureBiasValue                 SRational   1  +1
#  C.jpg                 Exif.Photo.ExposureMode                      Short       1  Auto bracket
#  C.jpg                 Exif.CanonCs.ExposureProgram                 Short       1  Program (P)
#  C:\Users\rmills\Pictures\TripHome\Test>
