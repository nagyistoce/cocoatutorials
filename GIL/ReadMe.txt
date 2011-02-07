Resize Utility
--------------

This code builds a utility to resize images:

$ resize from-image-path to-image-path scale quality

This utility was pulled together for use by the mkalbum.rb script which requires the RMagick gem.
I found RMagick to be difficult to build on the Mac, so I investigated this alternative.

The code uses GIL
- the Generic Image Library available from Adobe's Opensource Website.
- GIL in turn uses IPG (libjpeg.lib in UNIX).

I have included a DevStudio project to build IPG and resize.exe
numeric should be copied to %BOOST_ROOT%/gil/exension/numeric

Prior work:  IJGWin32:    http://eng.neologica.it/download/downloadIJGWin32.html
Adobe code:  resize.cpp:  http://opensource.adobe.com/wiki/display/gil/Generic+Image+Library

Building on Mac/Linux
$ make -B LDFLAGS=-ljpeg "CXXFLAGS=-I$BOOST_ROOT" resize

// typically BOOST_ROOT=/home/rmills/boost/boost_1_45_0/

Changes to IJGWin32
1) Modified to use static (/MT and /MTd C run-time library)
2) All C++ is compiled as C
3) Added extern "C" { .... } to jpeglib.h
4) IJGWin32.sln replaced with GIL.sln

Changes to Adobe code
1) Small functional changes to resize.cpp (to implement the command-line options)
2) Small changes to numeric/sampler.hpp to compile on Linux and Mac.

2011-02-06  19:19             1,496 GIL.sln         Solution to build library
2011-02-06  19:35             1,208 ReadMe.txt      This file of course
2011-02-06  19:10    <DIR>          Resize          Resize project
2011-02-06  19:27             2,257 resize.cpp      Resize source
2011-02-06  19:10    <DIR>          IJGWin32        JPEGlib code and project
2011-02-06  19:05    <DIR>          numeric         boost/GIL code


Robin Mills
robin@clanmills.com
http://clanmills.com

