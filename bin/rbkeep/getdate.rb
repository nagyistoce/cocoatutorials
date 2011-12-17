#!/usr/local/bin/ruby

require 'rubygems'
require 'exifr'
puts EXIFR::JPEG.new(ARGV[0]).date_time


# http://www.devdaily.com/ruby/ruby-jpeg-file-date-time-parse-format-images-photos
# gem install exifr

# http://www.ruby-forum.com/topic/166600
# gem install fxruby

# a ruby script to get the date and timestamp from a jpeg file.
# this script prints a string, something like '20080801.224705'.
# created by alvin alexander, http://devdaily.com.
# released here under the creative commons share-alike license:
# http://creativecommons.org/licenses/by-sa/3.0/

# require 'rubygems'
# require 'exifr'
# puts EXIFR::JPEG.new(ARGV[0]).date_time

# input_file = ARGV[0]
# 
# # get the date/time info from the JPEG file
# d1 = EXIFR::JPEG.new(input_file).date_time
# 
# 
# 
# # convert to the format we need
# #d = 'Wed Apr 01 13:51:16 -0800 2009'
# a,b,d,hms,poop,y = d1.to_s.split
# d2 = "#{a} #{b} #{d} #{hms} #{y}"
# 
# # create a real date
# d3 = DateTime::strptime(d2, '%a %b %d %H:%M:%S %Y')
# 
# # get this date in the desired format
# s = d3.strftime('%Y%m%d.%H%M%S')
# puts s
