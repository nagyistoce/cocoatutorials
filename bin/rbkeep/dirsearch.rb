#!/usr/local/bin/ruby

require 'find'

dir = ARGV[0]
arg = ARGV[1]

Find.find("#{dir}/") do |f|
    if f.match(/#{arg}/i) 
       print(f + "\n")
    end
end
