// ***************************************************************** -*- C++ -*-
// exifprint.cpp, $Id$
// Sample program to print the Exif metadata in JSON format

#include <exiv2/exiv2.hpp>
#include <Jzon.h>

#include <iostream>
#include <iomanip>
#include <cassert>
#include <string>

#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;
using namespace Jzon;
using namespace Exiv2;

// http://stackoverflow.com/questions/236129/splitting-a-string-in-c
static size_t split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems.size();
}

Jzon::Object& objectForKey(std::string Key,std::string& name,Jzon::Object& root)
{
    static Jzon::Object object;
    std::vector<std::string> keys ;
    size_t l = split(Key,'.',keys);
    if   ( l == 0 || l > 50 ) return object; // maybe we should throw
    
    name     = keys[l-1];
    size_t w = l-2; // how far to walk
    
    if ( !root.Has(keys[0]) )          root.Add(keys[0],object);
    Jzon::Object& r2 = (Jzon::Object&) root.Get(keys[0]);
    
    // walk the path of keys
    size_t  k = 1 ;
    while ( k < w ) {
        if ( !r2.Has(keys[k])) r2.Add(keys[k],object);
        r2 = (Jzon::Object&)   r2.Get(keys[k++]);
    }

    return r2 ;
}

// ExifData::const_iterator i
template <class T>
void push(Jzon::Object& json,const std::string& key,T i)
{
    std::string value = i->value().toString();

    switch ( i->typeId() ) {
        case Exiv2::xmpText:
        case Exiv2::asciiString : 
        case Exiv2::string:
        case Exiv2::comment:
             json.Add(key,value);
        break;

        case Exiv2::unsignedByte:
        case Exiv2::unsignedShort:
        case Exiv2::unsignedLong:
        case Exiv2::signedByte:
        case Exiv2::signedShort:
        case Exiv2::signedLong:
             json.Add(key,(int)i->value().toLong());
        break;
        
        case Exiv2::tiffFloat:
        case Exiv2::tiffDouble:
             json.Add(key,i->value().toFloat());
        break;

        case Exiv2::unsignedRational:
        case Exiv2::signedRational: {
             Jzon::Array arr;
             Rational rat = i->value().toRational();
             arr.Add (rat.first );
             arr.Add (rat.second);
             json.Add(key,arr);
        } break;

        case Exiv2::date:
        case Exiv2::time:
             json.Add(key,i->value().toString());
        break;

        default:
        case Exiv2::undefined:
        case Exiv2::tiffIfd:
        case Exiv2::directory:
        case Exiv2::xmpAlt:
        case Exiv2::xmpBag:
        case Exiv2::xmpSeq:
        case Exiv2::langAlt:
             // http://dev.exiv2.org/boards/3/topics/1367#message-1373
             if ( key == "UserComment" ) {
                size_t pos  = value.find('\0') ;
             	if (   pos != string::npos )
             	    value = value.substr(0,pos);
             }

             if ( key != "MakerNote") json.Add(key,value);
        break;
    }
}

void fileSystemPush(const char* path,Jzon::Node& nfs)
{
    Jzon::Object& fs = (Jzon::Object&) nfs;
    fs.Add("path",path);

#if defined(__linux__) || defined(__APPLE__)
    char resolved_path[PATH_MAX];
    fs.Add("realpath",realpath(path,resolved_path));
    
    struct stat buf;
    memset(&buf,0,sizeof(buf));
    stat(path,&buf);
    
    fs.Add("st_dev"    ,(int) buf.st_dev    ); /* ID of device containing file    */
    fs.Add("st_ino"    ,(int) buf.st_ino    ); /* inode number                    */
    fs.Add("st_mode"   ,(int) buf.st_mode   ); /* protection                      */
    fs.Add("st_nlink"  ,(int) buf.st_nlink  ); /* number of hard links            */
    fs.Add("st_uid"    ,(int) buf.st_uid    ); /* user ID of owner                */
    fs.Add("st_gid"    ,(int) buf.st_gid    ); /* group ID of owner               */
    fs.Add("st_rdev"   ,(int) buf.st_rdev   ); /* device ID (if special file)     */
    fs.Add("st_size"   ,(int) buf.st_size   ); /* total size, in bytes            */
    fs.Add("st_blksize",(int) buf.st_blksize); /* blocksize for file system I/O   */
    fs.Add("st_blocks" ,(int) buf.st_blocks ); /* number of 512B blocks allocated */
    fs.Add("st_atime"  ,(int) buf.st_atime  ); /* time of last access             */
    fs.Add("st_mtime"  ,(int) buf.st_mtime  ); /* time of last modification       */
    fs.Add("st_ctime"  ,(int) buf.st_ctime  ); /* time of last status change      */
#endif

}

int main(int argc, char* const argv[])
try {
    
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " file\n";
        return 1;
    }
    
    const char* path=argv[1];
        
    Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(path);
    assert(image.get() != 0);
    image->readMetadata();
        
    Jzon::Object root;

    const char*    FS="FS";
    Jzon::Object      fs  ;
    root.Add      (FS,fs) ;
    fileSystemPush(path,root.Get(FS));
    
	Exiv2::ExifData &exifData = image->exifData();
    for ( ExifData::const_iterator i = exifData.begin(); i != exifData.end() ; ++i ) {
        std::string   key ;
        push(objectForKey(i->key(),key,root),key,i);
    }

	Exiv2::IptcData &iptcData = image->iptcData();
    for (Exiv2::IptcData::const_iterator i = iptcData.begin(); i != iptcData.end(); ++i) {
        std::string key ;
        push(objectForKey(i->key(),key,root),key,i);
    }

	Exiv2::XmpData  &xmpData  = image->xmpData();
    for (Exiv2::XmpData::const_iterator i = xmpData.begin(); i != xmpData.end(); ++i) {
        std::string key ;
        push(objectForKey(i->key(),key,root),key,i);
    }
        
    Jzon::Writer writer(root,Jzon::StandardFormat);
    writer.Write();
    std::cout << writer.GetResult() << std::endl;
    return 0;
}
        
//catch (std::exception& e) {
//catch (Exiv2::AnyError& e) {
catch (Exiv2::Error& e) {
    std::cout << "Caught Exiv2 exception '" << e.what() << "'\n";
    return -1;
}
