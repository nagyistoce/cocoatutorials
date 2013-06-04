// ***************************************************************** -*- C++ -*-
// exifprint.cpp, $Rev: 2286 $
// Sample program to print the Exif metadata of an image

#include <exiv2/exiv2.hpp>

#include <iostream>
#include <iomanip>
#include <cassert>

#ifdef _MSC_VER
# include "getopt_win32.h"
#else
# include <unistd.h>                     // for getopt_long()
# include <getopt.h>
#endif

static void exifprint(const char* path)
{
#if 1
    std::cout << "--- " << path << std::endl ;
#else    
    Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(path);
    assert(image.get() != 0);
    image->readMetadata();

    Exiv2::ExifData &exifData = image->exifData();
    if (exifData.empty()) {
        std::string error(argv[1]);
        error += ": No Exif data found in the file";
        throw Exiv2::Error(1, error);
    }
    Exiv2::ExifData::const_iterator end = exifData.end();
    for (Exiv2::ExifData::const_iterator i = exifData.begin(); i != end; ++i) {
        const char* tn = i->typeName();
        std::cout << std::setw(44) << std::setfill(' ') << std::left
                  << i->key() << " "
                  << "0x" << std::setw(4) << std::setfill('0') << std::right
                  << std::hex << i->tag() << " "
                  << std::setw(9) << std::setfill(' ') << std::left
                  << (tn ? tn : "Unknown") << " "
                  << std::dec << std::setw(3)
                  << std::setfill(' ') << std::right
                  << i->count() << "  "
                  << std::dec << i->value()
                  << "\n";
    }
#endif
}

int main(int argc, char* const argv[])
try {

    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " { option | file } ...\n";
        std::cout << "option: {--nocurl | --curl}\n";
        return 1;
    }
    
    static struct option longopts[] = 
    {	{ "curl"   ,   no_argument,      NULL,           'c' }
    ,   { "nocurl" ,   no_argument,      NULL,           'n' }
    ,   { NULL     ,   0          ,      NULL,            0  }
    };
    
    int arg = 1;
    int cmd;
    int index =0;
    opterr = 0 ;

    while(((cmd = getopt_long(argc, argv, "cn", longopts, &index)) != -1) && !opterr)
    {
    	arg++;
    	std::cout << "arg,optind,index,cmd = " << arg <<","<< optind <<","<< index <<","<< cmd << std::endl;
    	switch (cmd) {
        	case 'c':
            	 std::cout << "turn on curl" << std::endl;
            break;
        	case 'n':
            	 std::cout << "turn OFF curl" << std::endl;
            break;
            default: break; // keep the compiler quiet
    	}
    	// while (arg < optind ) exifprint(argv[arg++]); // process arguments we ignored
    }
   	if ( !opterr ) while (arg < argc) exifprint(argv[arg++]); // process remaining arguments

    return 0;
}
//catch (std::exception& e) {
//catch (Exiv2::AnyError& e) {
catch (Exiv2::Error& e) {
    std::cout << "Caught Exiv2 exception '" << e.what() << "'\n";
    return -1;
}