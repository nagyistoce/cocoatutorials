// ***************************************************************** -*- C++ -*-
// exifprint.cpp, $Rev: 2286 $
// Sample program to print the Exif metadata in JSON format

#include <exiv2/exiv2.hpp>
#include <Jzon.h>

#include <iostream>
#include <iomanip>
#include <cassert>
#include <string>

typedef std::map<std::string,Jzon::Object*> jsonDict_t;
typedef jsonDict_t::iterator                jsonDict_i;

using namespace std;
using namespace Jzon;
using namespace Exiv2;

// http://stackoverflow.com/questions/236129/splitting-a-string-in-c
static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

Jzon::Object& objectForKey(std::string Key,std::string& name,Jzon::Object& root)
{
    std::vector<std::string> keys ;
    split(Key,'.',keys);
    size_t l = keys.size();
    name     = keys[keys.size()-1];
    
    static Jzon::Object object;
    if (  !root.Has(keys[0]) ) root.Add(keys[0],object);
    
    // this is horrible
    if ( l > 2 ) {
      size_t i = 0 ;
      Jzon::Object&                          r2 = (Jzon::Object&) root.Get(keys[i++]);
      if ( !r2.Has(keys[i]))                 r2.Add(keys[i],object);
      if ( l == i+2 ) return (Jzon::Object&) r2.Get(keys[i]) ;
      
      Jzon::Object&                          r3 = (Jzon::Object&) r2.Get(keys[i++]);
      if ( !r3.Has(keys[i]))                 r3.Add(keys[i],object);
      if ( l == i+2 ) return (Jzon::Object&) r3.Get(keys[i]) ;

      Jzon::Object&                          r4 = (Jzon::Object&) r3.Get(keys[i++]);
      if ( !r3.Has(keys[i]))                 r4.Add(keys[i],object);
      if ( l == i+2 ) return (Jzon::Object&) r4.Get(keys[i]) ;
    }
    return object;
}

// ExifData::const_iterator i
template <class T>
void push(Jzon::Object& json,const std::string& key,T i)
{
    std::string value = i->value().toString();

//  std::cout << key << " , " << i->typeId() << std::endl;

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
    root.Add("path", path);
    
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
