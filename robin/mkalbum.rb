#!/usr/bin/env ruby

##
# pull in the libraries
require 'find'
require 'rubygems'
require 'exifr'
require 'builder'
require 'optparse'

##
# generate a guid (not totally unique, but fast and propably good enough)
def genuuid
	# http://www.ruby-forum.com/topic/164078
    values = [
        rand(0x0010000),
        rand(0x0010000),
        rand(0x0010000),
        rand(0x0010000),
        rand(0x0010000),
        rand(0x1000000),
        rand(0x1000000),
     ] 
     "%04x%04x%04x%04x%04x%06x%06x" % values
end
##

##
# main  - main entry point of course
def main
	error=0
    files = []
    options = {}

    ##
    # parse the command line options
    optparse = OptionParser.new do|opts|
        # Set a banner, displayed at the top
        # of the help screen.
        opts.banner = "Usage: mkalbum.rb [options] photo-dir name [from [to]] (--help for help)"
    
        # Define the options, and what they do
        options[:verbose] = false
        opts.on( '-v', '--verbose', 'Output information' ) do
            options[:verbose] = true
        end
    
        # Define the options, and what they do
        options[:man] = false
        opts.on( '-m', '--man', 'Output man page' ) do
            options[:man] = true
        end

        options[:debug] = false
        opts.on( '-d', '--debug', 'Output a lot of information' ) do
            options[:debug]   = true
            options[:verbose] = true
        end
    
        # This displays the help screen, all programs are
        # assumed to have this option.
        opts.on( '-h', '--help', 'Display this screen' ) do
            puts opts
            error=1
        end
    end
    optparse.parse!
    
    ##
    # print the manual to stdout if requested (and exit)
    if (error==0) && options[:man]
   	    error=1
        ext = File.extname(__FILE__)
		man = File.join(File.dirname(__FILE__), File.basename(__FILE__,ext) + ".txt")
		puts man
        # Make sure the file exists
        if (File.exist?(man)) then

            # Open the file
            File.open(man) do | file |
                # For each line in the file 
                file.each_line do | line | 
                    puts "#{line}"
                end
            end
        else
            puts "*** Could not find #{man}"
    	    error=2
        end		
    end
    
    # process what remains in ARGV
    if (error==0) && ( (2..4).include? ARGV.length )
        any  = 'any'
        dir  = ARGV[0]
        name = ARGV[1]
        from = ARGV.length >= 3 ? ARGV[2] : any
        to   = ARGV.length >= 4 ? ARGV[3] : from
        
        if from == any
        	from = '1900-01-01'
        	to   = '2199-12-31'
        end
        
        ##
        # parse the dates
        # much better to use a pattern to test the date/time formats
        # (this will do for the moment)
        now = Time.now
        now = now.strftime('%Y-%m-%d ')
        
        # append time if missing
        from = from + " 00:00:00" if !from.match(':') 
        to   = to   + " 23:59:59" if !to.match(':') 
        
        # prefix date if missing
        from = now + from if !from.match('-') 
        to   = now + to   if !to.match('-') 
        
        ##
        # swap from/to if necessary
        r = [from,to].sort
        from = r[0]
        to   = r[1]
        
        ##
        # announce ourselves
        if options[:verbose]
            puts '--------------------------------'
            puts "from = #{from}"
            puts "to   = #{to}"
            puts '--------------------------------'
        end
        
        ##
        # build a hash/dictionary date-time <-> path to file
        photos = {} 
        exts = [ 'jpg','tiff'] # extensions to match
        
        Find.find("#{dir}/") do |f|
              next if File.directory?(f)
              matched = false
              for ext in exts
                 matched = f.match(/.#{ext}$/i)
                 break if matched
              end
              
              if matched
                 f = File.expand_path(f)
                 dt = EXIFR::JPEG.new(f).date_time
                 matched=dt
              end
              
              if matched
                 dt = dt.strftime('%Y-%m-%d %H:%M:%S')
                 photos[dt] = f
                 puts "#{f} -> #{dt}" if options[:debug]
              end
              
              if ! matched
                 puts "IGNORE #{f}" if options[:verbose]
              end
        end
    
        ##
        # filter off the interesting files
        photos.keys.sort.each do | dt |
            if (from < dt) && (dt < to) 
        	    f=photos[dt]
        	    puts "#{dt} -> #{f}" if options[:verbose]
        	    files.push(f)
        	else
        	    puts "SKIP #{from} #{to} #{dt} #{f}" if options[:debug]
        	end
        end
    else
        puts "Usage: mkalbum.rb [options] photo-dir name [from [to]] (--help for help)"
        error=3
    end
    
    ##
    # write out the album for the files
    if files.length > 0 
        puts '' if options[:verbose]

        ##
        # figure out the properties of the album
        albumid = genuuid
        props   = [ { 'name'   => 'uid'      , 'type'    => 'string'   , 'value'       => albumid             } \
                  , { 'name'   => 'token'    , 'type'    => 'string'   , 'value'       => ']album:'+albumid   } \
                  , { 'name'   => 'name'     , 'type'    => 'string'   , 'value'       => name                } \
                  , { 'name'   => 'date'     , 'type'    => 'real64'   , 'value'       => '40574.633530'      } \
                  , { 'name'   => 'category' , 'type'    => 'num'      , 'value'       => '0'                 } \
                  ]
        xml_filename = albumid + '.pal'
        xml_file     = File.open(xml_filename,'w')
    
        ##
        # write out the xml for the album
        # http://www.xml.com/pub/a/2006/01/04/creating-xml-with-ruby-and-builder.html?page=2
        xml = Builder::XmlMarkup.new( :target => xml_file , :indent => 2 )
        xml.picasa2album do
            xml.DBID    genuuid
            xml.AlbumID albumid
        
            props.each do | prop |
              xml.property( 'name' => prop['name'] , 'value' => prop['value'] , 'type' => prop['type']) 
            end
        
            xml.files do
                files.each do | filename |
                    xml.filename( filename )
                end
            end
        end
        xml_file.close
        puts "xml written to #{xml_filename}"
    else
        puts "*** NO FILES in Album" if error==0
        error = 4 if error==0
    end
    
	error    
end
##

## run the puppy!
exit main

# That's all Folks!
##
