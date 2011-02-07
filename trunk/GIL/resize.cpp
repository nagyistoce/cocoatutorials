/*
    Copyright 2005-2007 Adobe Systems Incorporated

    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).

    See http://opensource.adobe.com/gil for most recent version including documentation.
*/

/*************************************************************************************************/

/// \file
/// \brief Test file for resize_view() in the numeric extension
/// \author Lubomir Bourdev and Hailin Jin
/// \date February 27, 2007

///////////////////////
////  NOTE: This sample file uses the numeric extension, which does not come with the Boost distribution.
////  You may download it from http://opensource.adobe.com/gil
///////////////////////

#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/extension/io/jpeg_io.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>

#include <stdio.h>

// CXXFLAGS=-I/home/rmills/boost/boost_1_45_0/
// $ make LDFLAGS=-ljpeg resize
// > nmake CFLAGS=/EHsc "LDFLAGS=C:\Robin\Projects\IJGWin32\Release\IJGWin32.lib  /LIBPATH:C:\boost\boost_1_44\lib" resize
//
// better to use the MSVC project
//

static void sub(char* s,char c,char k)
{
    int l = strlen(s);
    for ( int  i  = 0 ; i < l ; i++ )
        if ( s[i] == c )
             s[i] = k;
}

int main(int argc,char* argv[])
{
    if ( argc != 4 && argc != 5 ) return printf("%s\n","syntax: resize from-file to-file scale [quality]");

    int    arg     = 1;
    char*  from    = strdup(argv[arg++]);
    char*  to      = strdup(argv[arg++]);
    double scale   = atof(argv[arg++]);
    int    quality = arg < argc ? atoi(argv[arg++]) : 75;
#ifdef WIN32
    sub(from,'/','\\');
    sub(to,'/','\\');
#endif
    using namespace boost::gil;

    // read the image
    rgb8_image_t img;
    jpeg_read_image(from,img);
    int W=(int)img.width();
    int H=(int)img.height();
//  printf("image %s -> %s size = %d,%d quality = %d scale = %6.3f\n",from,to,W,H,quality,scale);
    double w = W*scale + 0.5;
    double h = H*scale + 0.5;
    W        = (int) w      ;
    H        = (int) h      ;

    // Scale the image using bilinear resampling
    rgb8_image_t output(W,H);
    resize_view(const_view(img), view(output),bilinear_sampler()) ; // nearest_neighbor_sampler());

    // write the image
    jpeg_write_view(to,const_view(output),quality);

    return 0;
}
