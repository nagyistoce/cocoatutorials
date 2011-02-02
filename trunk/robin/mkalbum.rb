#!/usr/bin/env ruby

##
#
$syntax = "Usage: mkalbum.rb [options] photo-dir name [from [to]] (--help for help)"
#
#   returns   0   OK
#             1   --help or --man requested
#             2   --man can't find its input file
#             3   syntax error
#             4   no files found
#             5   from/to illegal date format
#             6   ContentFlow not implemented yet
##

##
# pull in the libraries
require 'find'
require 'rubygems'
require 'exifr'
require 'builder'
require 'optparse'
require 'date'
require 'time'
require 'parsedate'

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
# recover a datestring from a string (with a little massage if necessary)
# Example: if today is 2011-02-03 (Feb 3, 2011)
# argument:  arg          bLate
#            '01-18'      false     => '2011-01-18 00:00:00'
#            '2011-01-18' true      => '2011-01-18 23:59:59'
#            '06:50'      t|f       => '2011-02-3  06:50'
def fixdate(arg,bLate=false)
    now=Date.today # DateTime.now
	x = arg
	x = "#{now.strftime('%Y-')}#{arg}"        if /^\d\d-\d\d/.match(x)
	x = "#{now.strftime('%Y-%m-%d ')}#{arg}"  if ! /^\d\d\d\d-\d\d-\d\d/.match(x)
	r = DateTime.parse(x)
	if r
		if bLate
			r = "#{r.to_s[0..9]} 23:59:59"
		else
			r=r.strftime('%Y-%m-%d %H:%M:%S')
		end
	end
	return r
end
##

##
# seterror - set the error variable and report
def seterror(error,n,msg,bSyntax=false)
    if error==0
    	error=n
    	puts $syntax if bSyntax
    	puts msg     if !bSyntax && msg
    end
    error
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
        opts.banner = $syntax
    
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
    
        options[:picasa] = false
        opts.on( '-p', '--picasa', 'Output picasa album file' ) do
            options[:picasa]   = true
        end
    
        # This displays the help screen, all programs are
        # assumed to have this option.
        opts.on( '-h', '--help', 'Display this screen' ) do
            puts opts
            puts "    from/to := date/time"
            puts "    date:      2011-01-18  or 01-18 (this year assumed)"
            puts "    time:      06:45"
            error=seterror(error,1,nil)
        end
    end
    optparse.parse!
    
    ##
    # print the manual to stdout if requested
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
            error=seterror(error,2,"*** Could not find #{man}",false)
        end		
    end
    
    ##
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
        
        from = fixdate(from,false)
        to   = fixdate(to  ,true)
        puts "in main: from = #{from}"
        puts "in main: to   = #{to}"
        error = 5 if !from
        error = 5 if !to
    end
    
    ##
    # build the files array
    if (error==0) && from && to
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
                 dt = fixdate(dt.strftime('%Y-%m-%d %H:%M:%S')) if dt
                 matched=dt
              end
              
              if matched && dt
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
    	error=seterror(error,3,nil,true)
    end

    error=seterror(error,4,"*** NO FILES in Album")                if files.length == 0
    error=seterror(error,6,"*** ContentFlow not implemented yet!") if ( error==0) && !options[:picasa]
    
    ##
    # write out the album for the files
    if (error==0) && options[:picasa] 
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
    end
    
    	
    
	error    
end
##

## run the puppy!
exit main

# That's all Folks!
##
