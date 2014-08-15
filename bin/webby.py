#!/usr/bin/python
# -*- coding: UTF-8 -*-
r"""webby - a program for generating web pages from photographs

This exports:
  - webby.syntax - report syntax

This imports:
  - config/webby.py - page template strings 
                      (see clanmills/webby/webby.py for an example)

This optionally can use
  - config/config directory to find resources required
  - for example: gifs, jpegs, css files and other files required for the pages

--------------------
Revision information: 
$Id: //depot/bin/webby.py#17 $ 
$Header: //depot/bin/webby.py#17 $ 
$Date: 2008/07/15 $ 
$DateTime: 2008/07/15 14:50:18 $ 
$Change: 192 $ 
$File: //depot/bin/webby.py $ 
$Revision: #17 $ 
$Author: rmills $
--------------------
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
import re
import urllib
import shutil
from PIL import Image

import cmLib

##
# global variables
useSips      = os.name == 'darwin' # 'darwin' | 'cygwin' | 'win32' 
mustUpdate   = False
thumbQuality = Image.ANTIALIAS  # NEAREST, BILINEAR, BICUBIC, or ANTIALIAS 
imageQuality = Image.ANTIALIAS
imagesDir    = "Images/" 
thumbsDir    = "Thumbs/" 
next         = "next.html"
default      = 'default'
ext          = '.shtml'
prevgif      = 'prev.gif'
upgif        = 'up.gif'
nextgif      = 'next.gif'
photogif     = 'robinali.gif'
email        = 'webmaster@clanmills.com'
copyright    = '1996-2013 Robin Mills'
prev         = default + ext
lightbox     = 'lightbox'
download     = '2.35mb'
css          = '<link rel="stylesheet" type="text/css" href="/album.css"></link>'
title        = 'define from command arguments'
author       = 'Robin'
cols         = 3
cwd          = os.getcwd()
now          = string.lower(datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S%Z')) # ctime()
year         = string.lower(datetime.datetime.now().strftime('%Y')) # ctime()
width        = 750
capDir       = 0
geotag       = False
bReplace     = False

fcount       = 0
vcount       = 0
seq          = 6 # index of sequence in fileDict[key][tuple]
seqs         = []


##
# change path to enable import from the current directory
sys.path.insert(0,'.')

subs = {}

#
# use a dictionary to hold the data for every pathname
filedict = {}      # pathname : [ filename,xmlDate,datetime,image,aspect,ignore,sequence ]

#
##

##
# 
def syntax():
    """syntax - print syntax of webby.py """
    print "syntax: webby [[-photos] <photoDirectory>] [-config config] [-webdir <webdir>] [-title <title>] [-geotag]+ [-replace]+ [-capdir <capdir>]]+ "


##
#
def fixCaps(s):
    """fixCaps - add spaces before capitalized words """
    result = s
    if s:
        result=''
        if len(s) > 0:
            result = s[:1]
            for c in s[1:]:
                if c.isupper():
                    result += " "
                result += c
    return result
##

##
# 
def treesize(dir):
    """
    get byte count for files in a directory
    """
    result = 0
    dirall = os.path.join(dir,"*")

    for path in glob.glob(dirall):
        if not os.path.isdir(path):
            result += os.path.getsize(path)
    return result

##
#
def nint(f):
    return int(float+0.5)

##
# 
def mb(bytes):
    """
    convert bytes to a count eg 2345 = 2.34 kBytes
    """
    k = 1024 
    m = k*k 
    bytes = float(bytes)

    if ( bytes < 2*m ):
        return ("%6.0f" % (bytes / k)) + " kBytes"   

    return ("%6.2f" % (bytes / m)) + " mBytes"

##
# from PP3E/System/Filetools/cpall.py
verbose     = 0
dcount      = 0
maxfileload = 5000000
blksize     = 1024 * 100

##
# 
def cpfile(pathFrom, pathTo, maxfileload=maxfileload):
    """
    copy file pathFrom to pathTo, byte for byte
    """
    if os.path.getsize(pathFrom) <= maxfileload:
        bytesFrom = open(pathFrom, 'rb').read()   # read small file all at once
        open(pathTo, 'wb').write(bytesFrom)       # need b mode on Windows
    else:
        fileFrom = open(pathFrom, 'rb')           # read big files in chunks
        fileTo   = open(pathTo,   'wb')           # need b mode here too 
        while 1:
            bytesFrom = fileFrom.read(blksize)    # get one block, less at end
            if not bytesFrom: break               # empty after last chunk
            fileTo.write(bytesFrom)
##

##
# 
def cpall(dirFrom, dirTo):
    """
    copy contents of dirFrom and below to dirTo
    """
    global dcount, fcount
    for file in os.listdir(dirFrom):                      # for files/dirs here
        pathFrom = os.path.join(dirFrom, file)
        pathTo   = os.path.join(dirTo,   file)            # extend both paths
        if not os.path.isdir(pathFrom):                   # copy simple files
            try:
                if verbose > 1: print 'copying', pathFrom, 'to', pathTo
                cpfile(pathFrom, pathTo)
                fcount = fcount+1
            except:
                print 'Error copying', pathFrom, 'to', pathTo, '--skipped'
                print sys.exc_info()[0], sys.exc_info()[1]
        else:
            if verbose: print 'copying dir', pathFrom, 'to', pathTo
            try:
                os.mkdir(pathTo)                          # make new subdir
                cpall(pathFrom, pathTo)                   # recur into subdirs
                dcount = dcount+1
            except:
                print 'Error creating', pathTo, '--skipped'
                print sys.exc_info()[0], sys.exc_info()[1]

##

##
#   Add the geotag icon
def geotagIcon(jpeg,pos):
    icon = os.path.expanduser('~/bin/geotag_32.png')

    print "geotagIcon " + jpeg

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
          + " icon= "     + icon
    try:
        im_jpeg = Image.open(jpeg)
        im_jpeg.load()
        im_icon = Image.open(icon)
        im_icon.load()
    
        # create an image im_mask
        R, G, B = 0, 1, 2
        source = im_icon.split()
        im_mask   = source[R].point(lambda i: i > 100 and 255)
    
        w = im_jpeg.size[0]
        h = im_jpeg.size[1]
        W = w   # W was originally a parameter to stamp and scale in one operation
        H = h
        z = int(w/20) # make the icon 5% of the size of the image
        W = int(W)
        H = int(W * h / w)
        # set margin at 2% of the size of the image
        if m:
            m = w/50
    
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
    
        im_jpeg.save(jpeg, "JPEG")
    except:
        print "shit - trouble adding the geotag icon"

##
# 
""" sorting functions """
def reverseString(s):      return s[::-1]
def compare(x,y):          return 0 if x == y else 1 if x > y else -1
def cmpSize(x,y):          return compare(os.path.getsize(filedict[x][0]),os.path.getsize(filedict[y][0]))
def cmpName(x,y):          return compare(x,y)
def cmpNameDesc(x,y):      return compare(y,x)
def cmpNameReversed(x,y):  return compare(reverseString(filedict[x][0]),reverseString(filedict[y][0]))

def cmpDate(x,y):
    global seq
    result= (filedict[x][seq] - filedict[y][seq]) if (filedict[x][seq] != 0 or filedict[y][seq] != 0) else 0
    if result==0:
        result = compare(filedict[x][1],filedict[y][1])
    if result==0:
        result=cmpName(x,y)
    return result

##

##
#
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
#
def visitfile(filename,pathname,myData):
    """visitfile - use by the directory walker"""
    # print "filename,pathname = " , filename , " -> ",pathname
    global fcount, vcount
    aspect = 3.0 / 4.0
    try:
        image = pyexiv2.ImageMetadata(pathname)
        image.read()

        try:
            timestamp = image['Exif.Photo.DateTimeOriginal'].value # 'Exif.Image.DateTime']
        except:
            timestamp = datetime.datetime.fromtimestamp(os.stat(pathname)[stat.ST_MTIME])

        try:
            aspect = float(image['Exif.Photo.PixelYDimension'].value) / float(image['Exif.Photo.PixelXDimension'].value)
        except:pass

        xmlDate = getXMLtimez(timestamp)
        ignore=False
        sequence=0
        filedict[pathname] = [filename, xmlDate,timestamp,image,aspect,ignore,sequence]
        fcount = fcount + 1
    except:
    #   print "*** TROUBLE with " + pathname + " ***"
        pass
    vcount += 1
##

##
#
def visitor(myData, directoryName, filesInDirectory):        # called for each dir 
    """visitor - used by the directory walker"""
#   print "in visitor",directoryName, "myData = ",myData
#   print "filesInDirectory => ",filesInDirectory

    # disable the recursive search
    if os.path.abspath(myData) == os.path.abspath(directoryName):
        for filename in filesInDirectory:                        # do non-dir files here
            pathname = os.path.join(directoryName, filename)     # fnames have no dirpath
            if not os.path.isdir(pathname):                      # myData is searchKey
                visitfile(filename,pathname,myData)
##

##
#
def searcher(startdir,myData):
    """searcher - walk the tree"""
    # recursive search
    global fcount, vcount
    fcount = vcount = 0
    myData = startdir
    os.path.walk(startdir, visitor, myData)
##

##
#
def writeImages(webDir,filename,pathname,width,aspect,rotate,cols,bGeotagIcon):
    """writeImages - create images and thumbnails"""
    # write the image
    global useSips,mustUpdate
    imagename = os.path.join(webDir,imagesDir)
    if not os.path.isdir(imagename):
        os.mkdir(imagename)
    imagename = os.path.join(imagename, filename)
#   print "writeImages = " + imagename + " width = " + str(width)

    if mustUpdate or not os.path.exists(imagename):
        if useSips:
#            cmd = 'sips --setProperty formatOptions normal --resampleHeightWidthMax ' + str(width) \
#            + ' "' + pathname + '" --out "' + imagename + '"'

            W = width
            cmd  = 'sips '
            if rotate:
                cmd += ' --rotate ' + str(rotate)
            cmd += ' --setProperty formatOptions normal --resampleHeightWidthMax ' + str(W) \
                + ' "' + pathname + '" --out "' + imagename + '"'
            #    print cmd
            os.system(cmd)
        else:
            image    = Image.open(pathname)
            imageBox = image.getbbox()
            w = float(imageBox[2])
            h = float(imageBox[3])
            W = float(width)

            scale = (W/w) if w > h else (W/h) ;

            if rotate==90 or rotate==270:
                a = float(h)/float(w)
                scale = scale/a

            size  = ( int(w*scale),int(h*scale))
            print 'filename =', filename,     'size =', size,     'rotate =', rotate 
            image.thumbnail(size,imageQuality)
            image.rotate(-rotate).save(imagename, "JPEG")

        if bGeotagIcon:
            geotagIcon(imagename,"brm")

    # write thumbnail
    imagename = os.path.join(webDir,thumbsDir)
    if not os.path.isdir(imagename):
        os.mkdir(imagename)

    imagename = os.path.join(imagename, filename)
    if mustUpdate or not os.path.exists(imagename):
        if useSips:
#            cmd = 'sips --setProperty formatOptions normal --resampleHeightWidthMax ' + str(width/cols) \
#            + ' "' + pathname + '" --out "' + imagename + '"'

            W = float(width)
            W = int ( W / cols )
            if aspect > 1.0:
                W = W / aspect 

            cmd  = 'sips '
            if rotate:
                cmd += ' --rotate ' + str(rotate)
            cmd += ' --setProperty formatOptions normal --resampleHeightWidthMax ' + str(W) \
                + ' "' + pathname + '" --out "' + imagename + '"'
            print cmd

#            cmd = 'sips --setProperty formatOptions normal --resampleWidth ' + str(W) \
#            + ' "' + pathname + '" --out "' + imagename + '"'

            os.system(cmd)
        else:
            image    = Image.open(pathname)
            imageBox = image.getbbox()
            w = float(imageBox[2])
            h = float(imageBox[3])
            W = float(width)/float(cols)

            scale = (W/w) if w > h else (W/h) ; 
            if rotate==90 or rotate==270:
                a = float(h)/float(w)
                scale = scale/a

            size  = ( int(w*scale),int(h*scale))
            print 'filename=', filename      ,'size =', size,     'rotate =', rotate 
            image.thumbnail(size,thumbQuality)
            image.rotate(-rotate).save(imagename, "JPEG")

        if bGeotagIcon:
            geotagIcon(imagename,"brm")

##
#
def writeWebPage(webDir,filename,pathname,webPageString,subs,width,aspect,rotate,cols,bGeotagIcon):
    """writeWebPage - write out a photo page"""
    global bReplace
    if os.path.exists(webDir) and bReplace:
        shutil.rmtree(webDir)
    if not os.path.isdir(webDir):
        os.mkdir(webDir)

    if not os.path.isdir(webDir):
        throw ("the directory " , webDir , " isn't there");

    pname = os.path.splitext(filename)[0]
    path  = os.path.join(webDir,pname+subs['ext'])
    file  = open(path,"wt")
#   print "writeWebPage = " + webDir
#   print subs
#   file.write(webPageString)
    file.write(string.Template(webPageString).safe_substitute(subs))

    file.close()
    writeImages(webDir,filename,pathname,width,aspect,rotate,cols,bGeotagIcon)

##
#
def writeIndexPage(webDir,filedict,indexPageString,subs,width,cols):
    """writeIndexPage - write out the home page"""

#    for k in sorted(filedict.keys()):
#        filename   = filedict[k][0]
#        pathname   = filedict[k][1]

    path = os.path.join(webDir,subs['default']+subs['ext'])
    file = open(path,"wt")

    file.write(string.Template(indexPageString).safe_substitute(subs))
    file.close()

    print "writeIndexPage done"

##
#
def writeLightboxPage(webDir,filedict,lightboxPageString,subs,width,cols):
    """writeLightboxPage - write out the lightbox HTML"""

    path = os.path.join(webDir,lightbox+ext)
    file = open(path,"wt")

    file.write(string.Template(lightboxPageString).safe_substitute(subs))
    file.close()

    print "writeLightboxPage done"

##
#
def f(r):
    return float(r.numerator)/float(r.denominator)

##
#
def ff(x):
    return f(x[0])+ (f(x[1])/60.0) + (f(x[2])/3660.0)

##
#
def fs(x):
    return  str( int(f(x[0])) ) + "&deg; " +\
            str( int(f(x[1])) ) + "' "     +\
            str( int(f(x[2])) ) + '&quot; '

##
#
def noop():
    return

##
#
def nint(f):
    return (int) (f+0.5)

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
def readStory(filename):
    result=''
    igs=set()
    global title,now,seqs
    try:
        pat      =re.compile(r".ignore[ \t]+(.*)")
        pat_title=re.compile(r".title[ \t]+(.*)")
        pat_now  =re.compile(r".now[ \t]+(.*)")
        pat_seq  =re.compile(r".seq[ \t]+(.*)")
        f = open(filename)
        lines = f.readlines()
        for line in lines:
            if re.match(pat_title,line):
                title=re.match(pat_title,line).groups()[0]
                print "*** title ",title
            elif re.match(pat_now,line):
                now=re.match(pat_now,line).groups()[0]
                print "*** now ",now
            elif re.match(pat_seq,line):
                seqs.append(re.match(pat_seq,line).groups()[0])
            elif re.match(pat,line):
                for ig in re.match(pat,line).groups()[0].split():
                    igs.add(ig)
            else:
                result += line
    except:
        result=''

    return result,igs


##
#
def main(argv):
    """main - main program of course"""

    argc     = len(argv)
    if argc < 2:
        syntax()
        return

    opts = cmLib.getOpts(sys.argv[1:],{},'photos')
    cmLib.printOpts(opts)

    global url,subs,default,ext,prevgif,upgif,nextgif,lightbox,width
    global download,css,photogif,email,copyright,title,cols,author,cwd,time
    global subs
    global capDir
    global geotag
    global bReplace

    photoDir = cmLib.getOpt(opts,'photos',os.path.expanduser('~/Pictures'))
    title    = cmLib.getOpt(opts,'title' ,fixCaps(os.path.basename(os.path.expanduser(photoDir))))
    webDir   = cmLib.getOpt(opts,'webdir',os.path.join(cwd,"temp"))
    webDir   = cmLib.getOpt(opts,'webdir',os.path.join(cwd,year))
    capDir   = cmLib.getOpt(opts,"capdir",False)
    config   = cmLib.getOpt(opts,'config','webby')
    geotag   = cmLib.getOpt(opts,'geotag',False)
    bReplace = cmLib.getOpt(opts,'replace',False)
    cols     = eval(cmLib.getOpt(opts,'cols','3'))
    width    = eval(cmLib.getOpt(opts,'width','750'))
    dirs     = os.path.split(photoDir)
    base     = os.path.basename(dirs[0]) if dirs[1] == '' else dirs[1]
    webDir   = os.path.join(webDir,base)

    mepath   = sys.argv[0]
    me       = os.path.splitext(os.path.basename(mepath))[0] #'webby'

    photoDir = os.path.abspath(photoDir) 
    webDir   = os.path.abspath(webDir) 
    title    = fixCaps(os.path.split(photoDir)[1]) if title == "" else title

    ##
    # put the config directory and current directory on search path
    sys.path.insert(0,cwd)
    sys.path.insert(0,config)

    print "config = ", config
#   print sys.path

    configModule = __import__(me)
    configs = dir(configModule)

    indexPageString    = configs.__contains__('indexPageString')
    lightboxPageString = configs.__contains__('lightboxPageString')
    webPageString      = configs.__contains__('webPageString')

    if configs.__contains__('loadLibrary'):
        subs['now'      ]  = now 
        print "main subs = ",subs
        subs = objMerge(subs,configModule.loadLibrary(subs))
        print "and now subs = ",subs

    if os.path.abspath(mepath) == os.path.abspath(configModule.__file__):
        print "unable to locate your config webby.py file"
        return

    print "configModule = ",configModule.__file__
    resourceDir = os.path.dirname(os.path.abspath(configModule.__file__))

    # test that we have things to proceed
    if not os.path.isdir(photoDir):
        print photoDir + ' is not a directory'
        return
    if not os.path.isdir(resourceDir):
        print resourceDir + ' is not a directory'
        return
    if not os.path.isdir(webDir):
        os.mkdir(webDir)
    if not os.path.isdir(webDir):
        print webDir + ' is not a directory'
        return

    print "title = "      ,title
    print "photoDir = "   ,photoDir
    print "webDir = "     ,webDir
    print "resourceDir = ",resourceDir

    subs['default'  ]  = cmLib.getOpt(opts,'default'  ,default  )
    subs['ext'      ]  = cmLib.getOpt(opts,'ext'      ,ext      )
    subs['prevgif'  ]  = cmLib.getOpt(opts,'prevgif'  ,prevgif  )
    subs['upgif'    ]  = cmLib.getOpt(opts,'upgif'    ,upgif    )
    subs['nextgif'  ]  = cmLib.getOpt(opts,'nextgif'  ,nextgif  )
    subs['lightbox' ]  = cmLib.getOpt(opts,'lightbox' ,lightbox )
    subs['download' ]  = cmLib.getOpt(opts,'download' ,download )
    subs['css'      ]  = cmLib.getOpt(opts,'css'      ,css      )
    subs['photogif' ]  = cmLib.getOpt(opts,'photogif' ,photogif )
    subs['email'    ]  = cmLib.getOpt(opts,'email'    ,email    )
    subs['copyright']  = cmLib.getOpt(opts,'copyright',copyright)
    subs['title'    ]  = cmLib.getOpt(opts,'title'    ,title    )
    subs['cols'     ]  = cmLib.getOpt(opts,'cols'     ,str(cols))
    subs['author'   ]  = cmLib.getOpt(opts,'author'   ,author   )
    subs['now'      ]  = cmLib.getOpt(opts,'now'      ,now      )

    ext     = subs['ext']
    default = subs['default']

    ##
    if 1:   
        myData = 0 

        searcher(photoDir,myData)

        ##
        # read story and set of patterns to ignore
        story,ignore=readStory(os.path.join(photoDir,'story.txt'))

        nIgnored=0
        for k in sorted(filedict.keys(),cmpDate):
            pathname = k
            filename = filedict[k][0]
            bIgnore  = False
            for ig in ignore:
                if not bIgnore and re.search(ig,filename): 
                    bIgnore = True 
            if bIgnore:
                nIgnored+=1
            filedict[k].append(bIgnore)

        subs['title'] = title
        subs['now']   = now
        
        print 'Visited %d files, found %d ignored %d' % (vcount, fcount,nIgnored)

        ##
        # bump sequence (index=seq) for all images with .seq in story.txt
        global seqs
        S=0 # increment for every sequenced image
        for s in seqs:
            S=S+1
            scount=0
            K=0
            for k in filedict.keys():
                stub=os.path.splitext(os.path.basename(k))[0]
                if stub.find(s) != -1:
                    scount=scount+1
                    K=k
            if  scount==1:
                filedict[K][seq]=S
            #   print K,S

        #for k in sorted(filedict.keys(),cmpDate):
        #    print filedict[k][0],filedict[k][seq]
        # sys.exit(0)
        
        ##
        # figure out the prev/next pages
        pages = len(filedict.keys()) - nIgnored
        page  = 0
        prev  = []
        next  = []
        titles= []
        captions = []
        prior   = default
        for k in sorted(filedict.keys(),cmpDate):
            pathname   = k
            filename   = filedict[k][0]
            timestring = filedict[k][1]
            bIgnore    = filedict[k][5]
            if bIgnore:
                continue
                
            pname      = os.path.splitext(filename)[0]

            titles.append(pname)
            prev.append(prior)
            prior = pname

            if page > 0:
                next.append(pname)
            page += 1
            if page == pages:
                next.append(default)
        ##
        page=0

        ##
        # write the pages one at a time
        # and build the lightbox photos string
        # and the index pages thumbs string
        subs['pages' ] = str(pages)
        photos = "["
        thumbs = ""
        thumbsdiv = ""

        ##
        # alts (used by K2).  alternative titles for photos
        # TODO: add this capability to story.txt
        alt = {};
        alt['001'] = 'Traffic in Hanoi'
        alt['705'] = 'Sunday on the river' 
        alts=alt
        

        for k in sorted(filedict.keys(),cmpDate):
            pathname   = k
            filename   = filedict[k][0]
            timestring = filedict[k][1]
            timestamp  = filedict[k][2]
            image      = filedict[k][3]
            aspect     = filedict[k][4]
            bIgnore    = filedict[k][5]
            sequence   = filedict[k][seq]
            pname      = os.path.splitext(filename)[0]
            
            if bIgnore:
                continue        

            unknown    = 1000

            lat = unknown
            lon = unknown
            latr='N'
            lonr='W'
            bGeotag = False
            rotate = 0
            als='sea level'

            try:

#               Value   0th Row 0th Column
#               1   top left side
#               2   top right side
#               3   bottom  right side
#               4   bottom  left side
#               5   left side   top
#               6   right side  top
#               7   right side  bottom
#               8   left side   bottom
                if image['Exif.Image.Orientation'].value == 6:
                    rotate = 90
                if image['Exif.Image.Orientation'].value == 8:
                    rotate = 270
                if image['Exif.Image.Orientation'].value == 3:
                    rotate = 180

                latr= str(image['Exif.GPSInfo.GPSLatitudeRef'   ].value)
                lonr= str(image['Exif.GPSInfo.GPSLongitudeRef'  ].value)
                altr= int(image['Exif.GPSInfo.GPSAltitudeRef'   ].value)

                lat = ff (image['Exif.GPSInfo.GPSLatitude'      ].value)
                lon = ff (image['Exif.GPSInfo.GPSLongitude'     ].value)
                las = fs (image['Exif.GPSInfo.GPSLatitude'      ].value)
                los = fs (image['Exif.GPSInfo.GPSLongitude'     ].value)

                alt = f  (image['Exif.GPSInfo.GPSAltitude'      ].value)
                alt = nint ( (alt * 1000 / 25.4) /12 ) # metres => feet

                if altr==1:
                    als = str(alt) + " feet below sea level"
                else:
                    als = str(alt) + " feet above sea level"

                bGeotag = True

            except KeyError:
                noop()
            if not geotag:
                bGeotag=False

            ##
            # if there's a caption, use it!  (for Picasa support)
            caption=0
            try:
                caption= str(image['Iptc.Application2.Caption'    ].value)
            except KeyError:
                noop()
                
            # no caption, see if there's a caption in the image in .picasaoriginals
            if caption==0:
                try:
                    po='.picasaoriginals'
                    poPath = os.path.join(os.path.dirname(pathname),po,filename);
                    poImage = pyexiv2.ImageMetadata(poPath)
                    poImage.read()
                    caption  = str(poImage['Iptc.Application2.Caption'].value)
                except:
                    caption=0

            # if there's a capDir, try it
            if caption==0 and capDir: 
                try:
                    capImage = pyexiv2.ImageMetadata(os.path.join(capDir,filename))
                    capImage.read()
                    caption  = str(capImage['Iptc.Application2.Caption'].value)
                except KeyError:
                    noop()
                    
            # strip off leading [' and trailing ']
            if type(caption)==type(''):
                if caption[:2] == "['":
                    caption=caption[2:]
                if caption[-2:] == "']":
                    caption=caption[:-2]
                # strip off leading [" and trailing "]      
                if caption[:2] == '["':
                    caption=caption[2:]
                if caption[-2:] == '"]':
                    caption=caption[:-2]

#           print timestring, "=> ",filename

            # if no caption and name =~ /xxx_nnnn/ use blank
            if caption==0:
                if re.compile(r"[A-Z][A-Z][A-Z]_[0-9][0-9][0-9][0-9]").match(pname):
                    caption='&nbsp;'
                if re.compile(r"[a-z][a-z][a-z]_[0-9][0-9][0-9][0-9]").match(pname):
                    caption='&nbsp;'

            subs['pname' ]  = fixCaps(pname)
            caption=fixCaps(caption)
            captions.append(caption)
            if caption:
                print "caption = ", caption
                subs['pname']=caption

            title=subs['pname']
            if title in alts:
                title = alts[title]
                subs['pname']=title

            subs['image' ]  = os.path.join(imagesDir,filename)
            subs['thumb' ]  = cgi.escape(os.path.join(thumbsDir,filename))
            subs['iname' ]  = cgi.escape(pname+subs['ext'])
            subs['prev'  ]  = prev[page]
            subs['next'  ]  = next[page]
            subs['page'  ]  = str(page+1)
            subs['map'   ]  = ''
            subs['ptime' ]  = string.lower(timestamp.strftime('%Y-%m-%d %H:%M:%S%Z'))#ctime()
            subs['width' ]  = width
            subs['width2']  = width-200
            subs['rotate']  = rotate

            if lat != unknown:
                lsc =     las  + latr + " " +     los  + lonr
                loc = str(lat) + latr + "," + str(lon) + lonr
                subs['map'   ] = "<br>" + \
                    """Location: """  + lsc + "<br>" + \
                    """Elevation: """ + als + "<br>" + \
                    """Google Maps: <a href="http://maps.google.com/?q=""" + loc + \
                    """&z=17&t=k"  target="_blank">click here</a><br>Google Earth: <a href="http://maps.google.com/?q=""" + loc + \
                    """&z=17&iwloc=addr&output=kml">click here</a>""" + \
                    """ """
            if webPageString:    
                writeWebPage(webDir,filename,pathname,configModule.webPageString,subs,width,aspect,rotate,cols,bGeotag)

            bStartOfRow  = page % cols == 0 
            page        += 1
            bLast        = page == pages
            photos       = photos + '"' + cgi.escape(subs['thumb']) + '"'
            bEndOfRow    = page % cols == 0 or bLast
            e            = cgi.escape(subs['thumb'])
            I            = cgi.escape(subs['image'])
            i            = cgi.escape(subs['pname'])

            if bStartOfRow:
                thumbs    += "\n<tr>\n"
                thumbsdiv += "\n<tr>\n"


            thumbs         += '  <td><center><a href="' + subs['iname'] + '"><img src="' + e + '" longdesc="' + I + '"></a><br>' + title  + '</center></td>\n'
            thumbsdiv      += '  <td><center><div class=boxshadow><div class=boxmain><a href="' + subs['iname'] + '"><img src="' + e + '" class="thumb"></a><br>' + title  + '</div></div></center></td>\n'

            if bEndOfRow or bLast:
                thumbs    += '</tr>\n'
                thumbsdiv += '</tr>\n'

            if not bLast:
                photos  += ',\n'

        #   print 'length = ' ,len(thumbs)
        ##

        photos += "];\n"

        # build the captions string
        Captions='[';
        for caption in captions:
            if not caption:
                caption='None'
            Captions+= "'" + cgi.escape(caption.replace("'","\\'")) + "',\n"
        Captions+= ']\n'
        # print '--------------------------'
        # print Captions
        # print '--------------------------'
        
        # print 'length = ' ,len(thumbs)

        subs['story'     ] = story
        subs['photos'    ] = photos
        subs['captions'  ] = Captions
        subs['thumbs'    ] = thumbs
        subs['thumbsdiv' ] = thumbsdiv
        subs['download'  ] = mb(treesize(os.path.join(webDir,imagesDir)))

        print "wrote ",pages," pages"

        ##
        # write the index page and lightbox
        if indexPageString:
            writeIndexPage   (webDir,filedict,configModule.indexPageString   ,subs,width,cols)

        if lightboxPageString:
            writeLightboxPage(webDir,filedict,configModule.lightboxPageString,subs,width,cols)
        ##

        ##
        # recursively copy resourceDir to webDir
        print "copy from : ",resourceDir
        print "copy to   : ",webDir
        if os.path.exists(resourceDir) and os.path.isdir(resourceDir):
            cpall(resourceDir,webDir)
        ##

#   except:
#       print "syntax: webby <photoDirectory> <webDir> <title>"

#   finally:
#       print "goodnight!"

# 
##

##
#
if __name__ == '__main__':
    main(sys.argv)
