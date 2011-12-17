#!/usr/local/bin/ruby

require 'rubygems'
require 'RMagick'

jpg   = ARGV[0]
t_jpg = 't_' + jpg

img   = Magick::Image.read(jpg).first
t_img = img.scale(0.25)
t_img.write t_jpg
