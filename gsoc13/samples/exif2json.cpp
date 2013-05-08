// ***************************************************************** -*- C++ -*-
// exifprint.cpp, $Rev: 2286 $
// Sample program to print the Exif metadata in JSON format

#include <exiv2/exiv2.hpp>
#include <jzon.h>

#include <iostream>
#include <iomanip>
#include <cassert>
#include <string>

typedef std::map<std::string,Jzon::Object*> jsonDict_t;
typedef jsonDict_t::iterator                jsonDict_i;

using namespace std;
using namespace Jzon;
using namespace Exiv2;

Jzon::Object* objectForKey(std::string Key,jsonDict_t& jsonDict,std::string& name,Jzon::Object* root)
{
    char p = '.';
    Jzon::Object* result = NULL;
    if ( Key.find(p) != string::npos ) {
        size_t      l = Key.find_last_of(p);
        std::string k = Key.substr(0,l);
        name          = Key.substr(l+1);
        if ( !jsonDict.count(k) ) {
            result = jsonDict[k]=new Jzon::Object;
            std::string   x;
            Jzon::Object* parent = objectForKey(k,jsonDict,x,root);
            if ( parent )
                parent->Add(x,jsonDict[k]);
            else
                root->Add(k,jsonDict[k]);
        }
        result = jsonDict[k];
    }
    // cout << "return Key,name = " << Key << "," << name << " result = " << result << endl;
    return result;
}

// ExifData::const_iterator i
template <class T>
void push(Jzon::Object* json,const std::string& key,T i)
{
    if ( !json )  return ;
    std::string value = i->value().toString();

//  std::cout << key << " , " << i->typeId() << std::endl;

    switch ( i->typeId() ) {
        case Exiv2::xmpText:
        case Exiv2::asciiString : 
        case Exiv2::string:
        case Exiv2::comment:
             json->Add(key,value);
        break;

        case Exiv2::unsignedByte:
        case Exiv2::unsignedShort:
        case Exiv2::unsignedLong:
        case Exiv2::signedByte:
        case Exiv2::signedShort:
        case Exiv2::signedLong:
             json->Add(key,(int)i->value().toLong());
        break;
        
        case Exiv2::tiffFloat:
        case Exiv2::tiffDouble:
             json->Add(key,i->value().toFloat());
        break;

        case Exiv2::unsignedRational:
        case Exiv2::signedRational: {
             Jzon::Array arr;
             Rational rat = i->value().toRational();
             arr.Add (rat.first );
             arr.Add (rat.second);
             json->Add(key,arr);
        } break;

        case Exiv2::date:
        case Exiv2::time:
             json->Add(key,i->value().toString());
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

             if ( key != "MakerNote") json->Add(key,value);
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
        

//  if (exifData.empty()) {
//        std::string error(argv[1]);
//        error += ": No Exif data found in the file";
//        throw Exiv2::Error(1, error);
//  }
        
    Jzon::Object root;
    root.Add("path", path);
    
    jsonDict_t jsonDict;

	Exiv2::ExifData &exifData = image->exifData();
    for ( ExifData::const_iterator i = exifData.begin(); i != exifData.end() ; ++i ) {
        //cout << "typeId = " << i->typeId() 
        //     << " "         << i->key()
        //     << ":"         << i->value() 
        //     << std::endl
        //     ;
        std::string key ;
        Jzon::Object* json = objectForKey(i->key(),jsonDict,key,&root);
        push(json,key,i);
    }


	Exiv2::IptcData &iptcData = image->iptcData();
    for (Exiv2::IptcData::const_iterator i = iptcData.begin(); i != iptcData.end(); ++i) {
        std::string key ;
        Jzon::Object* json = objectForKey(i->key(),jsonDict,key,&root);
        push(json,key,i);
    }

	Exiv2::XmpData  &xmpData  = image->xmpData();
    for (Exiv2::XmpData::const_iterator i = xmpData.begin(); i != xmpData.end(); ++i) {
        std::string key ;
        Jzon::Object* json = objectForKey(i->key(),jsonDict,key,&root);
        push(json,key,i);
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
