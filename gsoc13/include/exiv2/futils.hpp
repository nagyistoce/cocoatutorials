// ********************************************************* -*- C++ -*-
/*
 * Copyright (C) 2004-2012 Andreas Huggel <ahuggel@gmx.net>
 *
 * This program is part of the Exiv2 distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301 USA.
 */
/*!
  @file    futils.hpp
  @brief   Basic file utility functions required by Exiv2
  @version $Rev: 2681 $
  @author  Andreas Huggel (ahu)
           <a href="mailto:ahuggel@gmx.net">ahuggel@gmx.net</a>
  @date    12-Dec-03, ahu: created<BR>
           02-Apr-05, ahu: moved to Exiv2 namespace
 */
#ifndef FUTILS_HPP_
#define FUTILS_HPP_

// *********************************************************************
// included header files
#ifdef _MSC_VER
# include "exv_msvc.h"
# include <types.hpp>
#else
# include "exv_conf.h"
#endif

#ifdef EXV_HAVE_STDINT_H
# include <stdint.h>                     // for uint8_t
#endif
#include <limits.h>						 // for UINT_MAX

// + standard includes
#include <string>
#include <map>

// *********************************************************************
// namespace extensions
namespace Exiv2 {

    /*!
       @brief The Protocol enum and the map to hold the strings
     */
    enum Protocol { pFile = 0, pHttp, pFtp, pHttps, pSftp};
    typedef std::map<std::string,Protocol>           protDict_t;
    typedef std::map<std::string,Protocol>::iterator protDict_i;
#ifdef EXV_UNICODE_PATH
    typedef std::map<std::wstring,Protocol> wprotDict_t;
    typedef std::map<std::wstring,Protocol>::iterator wprotDict_i;
#endif
// *********************************************************************
// free functions
    /*!
      @brief Encodes in base64 the data in in and puts the resulting string in out.
      @param out_size size in bytes of the out string, it should be at least ((in_size + 2) / 3) * 4 + 1
      @param in_size  size in bytes of the in buffer
      @return the string containing the encoded data, or NULL in case of error.

      @note copy from http://ffmpeg.org/doxygen/0.6/base64_8c-source.html
      */
    EXIV2API char* base64Encode(char* out, size_t out_size, const uint8_t* in, size_t in_size);
    /*!
      @brief Return the protocol of the path
      @param path the path of file to detect the protocol
      @return the protocol of the path
     */
    EXIV2API Protocol fileProtocol(const std::string& path);
#ifdef EXV_UNICODE_PATH
    /*!
      @brief Like fileProtocol() but accepts a unicode path in an std::wstring.
      @note This function is only available on Windows.
     */
    EXIV2API Protocol fileProtocol(const std::wstring& wpath);
#endif
    /*!
      @brief Test if a file exists.

      @param  path Name of file to verify.
      @param  ct   Flag to check if <i>path</i> is a regular file.
      @return true if <i>path</i> exists and, if <i>ct</i> is set,
      is a regular file, else false.

      @note The function calls <b>stat()</b> test for <i>path</i>
      and its type, see stat(2). <b>errno</b> is left unchanged
      in case of an error.
     */
    EXIV2API bool fileExists(const std::string& path, bool ct =false);
#ifdef EXV_UNICODE_PATH
    /*!
      @brief Like fileExists() but accepts a unicode path in an std::wstring.
      @note This function is only available on Windows.
     */
    EXIV2API bool fileExists(const std::wstring& wpath, bool ct =false);
#endif
    /*!
      @brief Return a system error message and the error code (errno).
             See %strerror(3).
     */
    EXIV2API std::string strError();

    // http://stackoverflow.com/questions/2616011/easy-way-to-parse-a-url-in-c-cross-platform
    class Uri
    {
    public:
        std::string QueryString;
        std::string Path;
        std::string Protocol;
        std::string Host;
        std::string Port;

        static Uri EXIV2API Parse(const std::string &uri);
    };

}                                       // namespace Exiv2

#endif                                  // #ifndef FUTILS_HPP_
