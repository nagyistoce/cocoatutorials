#!/usr/bin/env ruby

##
#
# resize.rb - resize images
#
##


##
# http://ruby.about.com/od/advancedruby/a/optionparser.htm
# A script that will pretend to resize a number of images

require 'optparse'
options = {}


optparse = OptionParser.new do|opts|

    # Set a banner, displayed at the top
    # of the help screen.
    opts.banner = "Usage: resize.rb [options] file1 file2 ...  (--help for help)"

    # Define the options, and what they do
    options[:verbose] = false
    opts.on( '-v', '--verbose', 'Output more information' ) do
        options[:verbose] = true
    end

    options[:quick] = false
    opts.on( '-q', '--quick', 'Perform the task quickly' ) do
        options[:quick] = true
    end

    options[:logfile] = nil
    opts.on( '-l', '--logfile FILE', 'Write log to FILE' ) do|file|
        options[:logfile] = file
    end

    options[:scale] = false
    opts.on( '-s', '--scale SCALE', 'scale the image SCALE' ) do|scale|
        options[:scale] = scale
    end


    # This displays the help screen, all programs are
    # assumed to have this option.
    opts.on( '-h', '--help', 'Display this screen' ) do
        puts opts
        exit
    end
end

def optparse.help
    puts @banner
end

##
# Parse the command-line. 
# There are two forms of the parse method.
# 1) 'parse'  simply parses ARGV
# 2) 'parse!' parses ARGV 
#     and removes any options found there, as well as any parameters for the options.
#     What's left is the list of files to resize.
optparse.parse!



##
# echo the options
puts "Being verbose" if options[:verbose]
puts "Being quick"   if options[:quick]
puts "Logging to file #{options[:logfile]}" if options[:logfile]
puts "Scale #{options[:scale]}" 

##
# do the business
count = 0
ARGV.each do|f|
    count+=1
    puts "Resizing image #{f}..."
    sleep 0.5
end
 
if count==0 
	optparse.help
end

# That's all folks!
##
