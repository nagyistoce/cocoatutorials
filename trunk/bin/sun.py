#!/usr/bin/python

import os
import sys
import time
import string
import optparse
import xml.dom.minidom


##
def sun(argv):
	month=time.localtime().tm_mon

	parser = optparse.OptionParser()
	# parser.add_option("-f", "--file", dest="filename", help="write report to FILE", metavar="FILE")
	# parser.add_option("-q", "--quiet", action="store_false", dest="verbose", default=True,  help="foo")

	bArgsOK=True
	(options, args) = parser.parse_args()
	if len(args):
		month=string.atoi(args[0])
	dst = 1 if month>=3 and month<=10 else 0

	for day in range(1,31+1):
		xmlPath  	= '/tmp/sun.xml'
		cmd 		= 'curl --silent http://www.earthtools.org/sun/37.3/-121.9/%d/%d/-8/%d > %s' % (day,month,dst,xmlPath)
		os.system(cmd)

		dom = xml.dom.minidom.parse(xmlPath)
	
		if day==1:
			print '     sunrise    sunset'
		for key in   [ 'sunrise', 'sunset' ]:
			print '%02d-%02d %s ' % (month,day,xml.dom.minidom.parse(xmlPath).getElementsByTagName(key)[0].firstChild.data),
		print
		
	return 0 if bArgsOK else 1

# 
##


##
#
if __name__ == '__main__':
	sun(sys.argv)

# That's all Folks!
##
