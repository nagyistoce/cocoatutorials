// ***************************************************************** -*- C++ -*-
// exifprint.cpp, $Rev: 2286 $
// Sample program to print the Exif metadata in JSON format

#include <exiv2/exiv2.hpp>

#include <iostream>
#include <iomanip>
#include <cassert>

#include <jzon.h>
// #include <jzon.cpp>

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
        
        Exiv2::XmpData  &xmpData  = image->xmpData();
        Exiv2::IptcData &iptcData = image->iptcData();
        Exiv2::ExifData &exifData = image->exifData();
        if (exifData.empty()) {
            std::string error(argv[1]);
            error += ": No Exif data found in the file";
            throw Exiv2::Error(1, error);
        }
        
        Jzon::Object root;
#if 1
        root.Add("path", path);
        Jzon::Object     iptc;
        Jzon::Object     exif;
        Jzon::Object     xmp;
        root.Add("iptc",&iptc);
        root.Add("exif",&exif);
        root.Add("xmp" ,&xmp);
        for (Exiv2::ExifData::const_iterator i = exifData.begin(); i != exifData.end(); ++i) {
            exif.Add(i->key(),i->value().toString());
        }
        for (Exiv2::IptcData::const_iterator i = iptcData.begin(); i != iptcData.end(); ++i) {
            iptc.Add(i->key(),i->value().toString());
        }
        for (Exiv2::XmpData::const_iterator i = xmpData.begin(); i != xmpData.end(); ++i) {
            xmp.Add(i->key(),i->value().toString());
        }
#else
        root.Add("name", "value");
        root.Add("number", 20);
        root.Add("anothernumber", 15.3);
        root.Add("stuff", true);
        
        Jzon::Array listOfStuff;
        listOfStuff.Add("json");
        listOfStuff.Add("more stuff");
        listOfStuff.Add(Jzon::null);
        listOfStuff.Add(false);
        root.Add("listOfStuff", listOfStuff);
        
        root.Add("age",Jzon::Value(Jzon::Value::VT_NUMBER,"62.4"));
        
        Jzon::Object iptc;
        Jzon::Object exif;
        root.Add("iptc",&iptc);
        root.Add("exif",&exif);
        
        iptc.Add("Comment","you know");
        exif.Add("Exif.Image","very good image");
        exif.Add("Exif.Resolution",200);
#endif
        
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
