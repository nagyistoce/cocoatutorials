#!/usr/bin/env ruby

require 'rubygems'
require 'builder'

# http://www.ruby-forum.com/topic/164078
def genuuid
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


uuid  = genuuid
files = ["abc", "def"]
props = [ { 'name'   => 'uid'      , 'type'    => 'string'   , 'value'       => uuid             } \
        , { 'name'   => 'token'    , 'type'    => 'string'   , 'value'       => ']album:'+uuid   } \
        , { 'name'   => 'name'     , 'type'    => 'string'   , 'value'       => '2011'           } \
        , { 'name'   => 'date'     , 'type'    => 'real64'   , 'value'       => '40574.633530'   } \
        , { 'name'   => 'category' , 'type'    => 'num'      , 'value'       => '0'              } \
        ]

# http://www.xml.com/pub/a/2006/01/04/creating-xml-with-ruby-and-builder.html?page=2
xml = Builder::XmlMarkup.new( :target => $stdout, :indent => 2 )
xml.pacasa2album do
    xml.DBID    genuuid
    xml.AlbumID genuuid

    props.each do | prop |
      xml.property( 'name' => prop['name'] , 'value' => prop['value'] , 'type' => prop['type']) 
    end

    xml.files do
        files.each do | filename |
            xml.filename( filename )
        end
    end
end

# ----------------------------
#   <pacasa2album>
#     <DBID>72c4b509ddab1db21afff37c6e5baf94</DBID>
#     <AlbumID>54c225605555f590165debdaa539696e</AlbumID>
#     <property name="uid" type="string" value="72e32d11be4070c745f7df4691b07d9e"/>
#     <property name="token" type="string" value="]album:72e32d11be4070c745f7df4691b07d9e"/>
#     <property name="name" type="string" value="2011"/>
#     <property name="date" type="real64" value="40574.633530"/>
#     <property name="category" type="num" value="0"/>
#     <files>
#       <filename>abc</filename>
#       <filename>def</filename>
#     </files>
#   </pacasa2album>


# ----------------------------
#   <picasa2album>
#    <DBID>a3ec2c2f4279705a0a14ccf4fec8a065</DBID>
#    <AlbumID>0dce99456a8cd10dd72162f8a66075e4</AlbumID>
#    <property name="uid" type="string" value="0dce99456a8cd10dd72162f8a66075e4"/>
#    <property name="token" type="string" value="]album:0dce99456a8cd10dd72162f8a66075e4"/>
#    <property name="name" type="string" value="2011"/>
#    <property name="date" type="real64" value="40574.633530"/>
#    <property name="category" type="num" value="0"/>
#    <files>
#     <filename>$My Documents\Dropbox\Photos\2011\PaulsLunch\IMG_2523.jpg</filename>
#     <filename>$My Documents\Dropbox\Photos\2011\PaulsLunch\IMG_2524.jpg</filename>
#     <filename>$My Documents\Dropbox\Photos\2011\PaulsLunch\IMG_2526.jpg</filename>
#     <filename>$My Documents\Dropbox\Photos\2011\PaulsLunch\IMG_2527.jpg</filename>
#     <filename>$My Documents\Dropbox\Photos\2011\PaulsLunch\IMG_2528.jpg</filename>
#     <filename>$My Documents\Dropbox\Photos\2011\PaulsLunch\IMG_2529.jpg</filename>
#     <filename>$My Documents\Dropbox\Photos\2011\PaulsLunch\IMG_2530.jpg</filename>
#     <filename>$My Documents\Dropbox\Photos\2011\PaulsLunch\IMG_2532.jpg</filename>
#     <filename>$My Documents\Dropbox\Photos\2011\PaulsLunch\IMG_2533.jpg</filename>
#     <filename>$My Documents\Dropbox\Photos\2011\PaulsLunch\IMG_2534.jpg</filename>
#     <filename>$My Documents\Dropbox\Photos\2011\PaulsLunch\IMG_2535.jpg</filename>
#     <filename>$My Documents\Dropbox\Photos\2011\PaulsLunch\IMG_2536.jpg</filename>
#     <filename>$My Documents\Dropbox\Photos\2011\PaulsLunch\IMG_2537.jpg</filename>
#     <filename>$My Documents\Dropbox\Photos\2011\PaulsLunch\IMG_2538.jpg</filename>
#     <filename>$My Documents\Dropbox\Photos\2011\PaulsLunch\Gang.jpg</filename>
#    </files>
#   </picasa2album>

