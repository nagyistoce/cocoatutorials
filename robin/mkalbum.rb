#!/usr/bin/env ruby

##
#
$syntax = "Usage: mkalbum.rb [options] photo-dir [name [from [to]]] (--help for help)"
#
#   returns   0   OK
#             1   --help or --man requested
#             2   --man can't find its input file
#             3   syntax error
#             4   no files found
#             5   from/to illegal date format
#             6   ContentFlow not implemented yet
#             7   the output directory already exists
#             8   an essential directory does not exist
#             9   RMagick is requested and not available
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
require 'fileutils'
require 'progressbar'

begin 
$bRMagick=require 'RMagick'
rescue LoadError
$bRMagick=false
end

$bMac     = RUBY_PLATFORM.match('darwin')
$bWindows = RUBY_PLATFORM.match('indows') || RUBY_PLATFORM.match('ming')
$bLinux   = RUBY_PLATFORM.match('inux')

##
# recursive directory copier
class DaCopier
    # http://iamneato.com/2009/07/28/copy-folders-recursively
    def initialize(exclude=[],verbose=false)
        @exclude = exclude
        @verbose = verbose
    end

    def copy(src, dest)
        stage(dest) if File.directory?(src)

        Dir.foreach(src) do |file|
            next if exclude?(file)

            s = File.join(src, file)
            d = File.join(dest, file)

            if File.directory?(s)
                FileUtils.mkdir(d)
                copy(s, d)
            else
                FileUtils.cp(s, d)
            end
            puts d if verbose
        end if File.directory?(src)

        puts "cp #{src} #{dest}".chomp if verbose

        if File.file?(src)
            FileUtils.cp(src,dest)
            puts dest if verbose
        end
    end

private
    def stage(dest)
        if File.directory?(dest)
            FileUtils.rm_rf(dest)
        end
        FileUtils.mkdir(dest)
    end

    def exclude?(file)
        @exclude.each do |s|
            if file.match(/#{s}/i)
                return true
            end
        end
        false
    end

    def verbose
        @verbose
    end
end
##

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
# spaceCap "LasVegas" "Las Vegas"
def spaceCap(s)
    return s.gsub(/([a-z])([A-Z])/,'\1 \2')
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
        puts msg     if msg
        puts $syntax if bSyntax
    end
    error
end
##

##
# update a file and substitute
def templateUpdate(filename,a,b)
    file  = File.open(filename,'r')
    lines = []
    while (line = file.gets)
        lines << line
    end
    file.close()

    file = File.open(filename,'w')
    for line in lines
        line=line.gsub(a,b)
        file.write(line)
    end
    file.close()
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
    begin 
        optparse = OptionParser.new do|opts|
            # Set a banner, displayed at the top of the help screen.
            opts.banner = $syntax

            # Define the options, and what they do
            options[:verbose] = false
            opts.on( '-v', '--verbose', 'Output information' ) do
                options[:verbose] = true
            end

            options[:debug] = false
            opts.on( '-d', '--debug', 'Output a lot of information' ) do
                options[:debug]   = true
                options[:verbose] = true
            end

            options[:contentView] = true
            options[:picasa] = false
            opts.on( '-p', '--picasa', 'Output picasa album file' ) do
                options[:picasa]   = true
                options[:contentView] = false
            end

            options[:overwrite] = false
            opts.on( '-o', '--overwrite', 'overwrite the output' ) do
                options[:overwrite]   = true
            end

            options[:rmagick] = 'auto'
            opts.on( '-r', '--rmagick [off|on|auto]', 'use rmagick' ) do | rmagick |
                options[:rmagick]  = rmagick
            end

            options[:quality] = 75
            opts.on( '-q', '--quality [quality]', 'output image quality' ) do | quality |
                options[:quality] = quality.to_i
            end

            options[:scale] = 0.20
            opts.on( '-s', '--scale [scale]', 'scale original image' ) do | scale |
                options[:scale] = scale.to_f
            end

            options[:title] = ''
            opts.on( '-t', '--title [title]', 'title for the album' ) do | title |
                options[:title] = title.to_s
            end

            options[:man] = false
            opts.on( '-m', '--man', 'Output man page' ) do
                options[:man] = true
            end

            opts.on( '-h', '--help', 'Display this screen' ) do
                puts opts
                puts "    from/to := date/time"
                puts "    date:      2011-01-18  or 01-18 (this year assumed)"
                puts "    time:      06:45"
                error=seterror(error,1,nil)
            end
        end
        optparse.parse!
    rescue
        error=seterror(error,3,"*** Syntax error",true)
    end

    ##
    # print the manual to stdout if requested
    if (error==0) && options[:man]
        error=seterror(error,1,nil)
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

    error=seterror(error,9,"*** RMagick is not available",false) if (!$bRMagick) && (options[:rmagick] == 'on')
    $bRMagick = false if (error==0) && (options[:rmagick] == 'off')

    ##
    # process what remains in ARGV
    if (error==0) && (1..4).include?(ARGV.length)
        any  = 'any'
        dir  = ARGV[0]
        name = ARGV.length >= 2 ? ARGV[1] : String.new(File.basename(dir))
        from = ARGV.length >= 3 ? ARGV[2] : String.new(any)
        to   = ARGV.length >= 4 ? ARGV[3] : String.new(from)

        error=seterror(error,8,"*** directory #{dir} does not exist",false) if !File.directory?(dir)
        from = '1900-01-01' if from == any
        to   = '2999-12-31' if to   == any

        from = fixdate(from,false)
        to   = fixdate(to  ,true)
        error=seterror(error,5,nil,true) if !from || !to
    else
        error=seterror(error,3,nil,true)
    end

    ##
    # build the files array
    if error==0
        title = options[:title]
        title = File.basename(name) if !title || title.length() == 0
        ##
        # swap from/to if necessary
        r = [from,to].sort
        from = r[0]
        to   = r[1]

        ##
        # announce ourselves
        graphics = $bRMagick ? "RMagick" : "builtin resize utility"
        puts '--------------------------------'
        puts "title         = #{title}"
        puts "dir           = #{dir}"
        puts "from .. to    = #{from} .. #{to}"
        puts "scale,quality = #{options[:scale]}, #{options[:quality]}"
        puts "graphics      = #{graphics}"
        puts '--------------------------------'

        ##
        # build a hash/dictionary date-time <-> path to file
        photos = {} 
        exts = [ 'jpg','tiff'] # extensions to match

        Find.find(dir) do |f|
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
        end # if File.directory?(dir)

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
    end
    error=seterror(error,4,"*** NO FILES in Album") if files.length == 0

    ##
    # write a picasa album for the files
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

    ##
    # write a "ContentView" album for the files
    if (error==0) && options[:contentView] 
        root        = File.join(File.dirname(__FILE__), "mkalbum")
        contentFlow = "ContentFlow"
        fancyZoom   = "FancyZoom"
        images      = "Images"
        template    = 'index.template'
        index       = 'index.html'

        FileUtils.rm_rf(name) if options[:overwrite] && File.directory?(name)
        FileUtils.rm(name)    if options[:overwrite] && File.exist?(name)

        error=seterror(error,7,"*** directory #{name} already exists!") if File.exist?(name) 
        error=seterror(error,7,"*** directory #{name} already exists!") if File.directory?(name) 

        # Make sure everything ready to be copied
        if error==0
            FileUtils.mkdir(name)
            FileUtils.mkdir(File.join(name,images))
            error=seterror(error,8,"*** directory #{name} doesn't exist!") if !File.directory?(name) 
            error=seterror(error,8,"*** directory #{root} doesn't exist!") if !File.directory?(root) 
        end

        if error==0
            bar     = ProgressBar.new("Scale images", files.length()) if !options[:verbose]
            images  = File.join(name,'Images')
            exclude = ["^\\."]
            dc = DaCopier.new(exclude,options[:verbose])
            dc.copy(File.join(root,contentFlow), File.join(name,contentFlow ))
            dc.copy(File.join(root,fancyZoom)  , File.join(name,fancyZoom   ))
            dc.copy(File.join(root,images)     , File.join(name,images      ))
            dc.copy(File.join(root,template)   , File.join(name,index)       )

            t_next = <<HERE
<img class="item" src="Images/__name__" title="__title__"/>
__NEXT__
HERE
            jpg=0
            for file in files
            #   unfortunately exifr cannot read Iptc meta-data (used by Picasa)
            #   t_title  = img.get_exif_by_entry['Iptc.Application2.Caption']
                t_title  = File.basename(file,File.extname(file))
                t_name   = "#{jpg}.jpg"

                s_next   = t_next ;
                s_next   = s_next.gsub('__name__',t_name)
                s_next   = s_next.gsub('__title__',t_title)
                templateUpdate(File.join(name,index),'__NEXT__'   ,s_next)

                puts "scaling image #{file}" if  options[:verbose]
                bar.inc(1)                   if !options[:verbose]
                t_name  = File.join(images,t_name)
                scale   = options[:scale]
                quality = options[:quality]

                if $bRMagick
                    img   = Magick::Image.read(file).first
                    t_img = img.scale(scale)
                    t_img.write(t_name) { self.quality = quality }
                    t_img = nil
                else
                    # the bin holds the resize utility to use instead of RMagick
                    bin    = File.join(root,'bin'   )
                    bin    = File.join(bin,'windows') if $bWindows 
                    bin    = File.join(bin,'linux'  ) if $bLinux 
                    bin    = File.join(bin,'mac'    ) if $bMac
                    resize = File.join(bin,'resize' )
                    file   = File.expand_path(file)
                    t_name = File.expand_path(t_name)

                    # execute resize in the directory in which he lives (to keep ming happy)
                    t_dir  = Dir.pwd
                    Dir.chdir(File.dirname(resize))
                    cmd    = "resize  \"#{file}\"  \"#{t_name}\"  #{scale}  #{quality}"
                    puts cmd         if  options[:verbose]
                    cmd   += ' 2>/dev/null >/dev/null' if (!$bWindows) && !options[:verbose]
                    system(cmd) 
                    Dir.chdir(t_dir)

                end
                img = nil
                GC.start
                jpg+=1
            end

            templateUpdate(File.join(name,index),'__TITLE__',spaceCap(title))
            templateUpdate(File.join(name,index),'__NEXT__','')
            File.chmod(0755, File.join(name,index)) 

            Dir.chdir(name)
            puts "\nopen \"#{File.join(name,index)}\""
            system "open #{index}"
        end
    end

    # return value
    error    
end
##

## run the puppy!
exit main

# That's all Folks!
##
