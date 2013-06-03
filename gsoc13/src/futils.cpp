// ***************************************************************** -*- C++ -*-
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
/*
  File:      futils.cpp
  Version:   $Rev: 2681 $
  Author(s): Andreas Huggel (ahu) <ahuggel@gmx.net>
  History:   08-Dec-03, ahu: created
             02-Apr-05, ahu: moved to Exiv2 namespace
 */
// *****************************************************************************
#include "rcsid_int.hpp"
EXIV2_RCSID("@(#) $Id: futils.cpp 2681 2012-03-22 15:19:35Z ahuggel $")

// *****************************************************************************
#include "exv_conf.h"
#include "futils.hpp"

// + standard includes
#include <sys/types.h>
#include <sys/stat.h>
#ifdef _MSC_VER
# define S_ISREG(m)      (((m) & S_IFMT) == S_IFREG)
#endif
#ifdef EXV_HAVE_UNISTD_H
# include <unistd.h>                     // for stat()
#endif

#include <cerrno>
#include <sstream>
#include <cstring>
#include <algorithm>

#if defined EXV_HAVE_STRERROR_R && !defined EXV_HAVE_DECL_STRERROR_R
# ifdef EXV_STRERROR_R_CHAR_P
extern char *strerror_r(int errnum, char *buf, size_t n);
# else
extern int strerror_r(int errnum, char *buf, size_t n);
# endif
#endif

namespace Exiv2 {

// *****************************************************************************
// free functions
    static const uint8_t map2[] = {
        0x3e, 0xff, 0xff, 0xff, 0x3f, 0x34, 0x35, 0x36,
        0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x01,
        0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
        0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11,
        0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1a, 0x1b,
        0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23,
        0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b,
        0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33
    };
    char* base64Encode(char* out, size_t out_size, const uint8_t* in, size_t in_size) {
        static const char b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        char *ret, *dst;
        unsigned i_bits = 0;
        size_t i_shift = 0;
        size_t bytes_remaining = in_size;

        if (in_size >= UINT_MAX / 4 || out_size < (in_size+2) / 3 * 4 + 1)
            return NULL;
        ret = dst = out;
        while (bytes_remaining) {
            i_bits = (i_bits << 8) + *in++;
            bytes_remaining--;
            i_shift += 8;
            do {
                *dst++ = b64[(i_bits << 6 >> i_shift) & 0x3f];
                i_shift -= 6;
            } while (i_shift > 6 || (bytes_remaining == 0 && i_shift > 0));
        }
        while ((dst - ret) & 3) *dst++ = '=';
        *dst = '\0';

        return ret;
    }

    Protocol fileProtocol(const std::string& path) {
        Protocol result = pFile ;
        static Exiv2::protDict_t protDict;
        if (!protDict.size()) {
             protDict["http://"]  = pHttp;
             protDict["ftp://"]   = pFtp;
             protDict["https://"] = pHttps;
             protDict["sftp://"]  = pSftp;
        }
        for (Exiv2::protDict_i it = protDict.begin(); it != protDict.end(); it++) {
            if (path.find(it->first) == 0)
                 result = it->second;
        }
        return result;
    }
#ifdef EXV_UNICODE_PATH
    Protocol fileProtocol(const std::wstring& wpath) {
        Protocol result = pFile ;
        static wprotDict_t protDict;
        if (!protDict.size()) {
             protDict[L"http://"]  = pHttp;
             protDict[L"ftp://"]   = pFtp;
             protDict[L"https://"] = pHttps;
             protDict[L"sftp://"]  = pSftp;
        }
        for (wprotDict_i it = protDict.begin(); it != protDict.end(); it++) {
            if (wpath.find(it->first) == 0) {
                 result = it->second;
                 break;
            }
        }
        return result;
    }
#endif
    bool fileExists(const std::string& path, bool ct)
    {
		// special case: accept "-" (means stdin)
        if (path.compare("-") == 0 || fileProtocol(path)) {
			return true;
        }

        struct stat buf;
		int ret = ::stat(path.c_str(), &buf);
        if (0 != ret)                    return false;
        if (ct && !S_ISREG(buf.st_mode)) return false;
        return true;
    } // fileExists

#ifdef EXV_UNICODE_PATH
    bool fileExists(const std::wstring& wpath, bool ct)
    {
		// special case: accept "-" (means stdin)
        if (wpath.compare(L"-") == 0 || fileProtocol(wpath)) {
			return true;
        }

        struct _stat buf;
        int ret = _wstat(wpath.c_str(), &buf);
        if (0 != ret)                    return false;
        if (ct && !S_ISREG(buf.st_mode)) return false;
        return true;
    } // fileExists

#endif
    std::string strError()
    {
        int error = errno;
        std::ostringstream os;
#ifdef EXV_HAVE_STRERROR_R
        const size_t n = 1024;
// _GNU_SOURCE: See Debian bug #485135
# if defined EXV_STRERROR_R_CHAR_P && defined _GNU_SOURCE
        char *buf = 0;
        char buf2[n];
        std::memset(buf2, 0x0, n);
        buf = strerror_r(error, buf2, n);
# else
        char buf[n];
        std::memset(buf, 0x0, n);
        strerror_r(error, buf, n);
# endif
        os << buf;
#else
        os << std::strerror(error);
#endif
        os << " (errno = " << error << ")";
        return os.str();
    } // strError
    // http://stackoverflow.com/questions/2616011/easy-way-to-parse-a-url-in-c-cross-platform
    Uri Uri::Parse(const std::string &uri)
    {
        Uri result;

        typedef std::string::const_iterator iterator_t;

        if ( !uri.length() )  return result;

        iterator_t uriEnd = uri.end();

        // get query start
        iterator_t queryStart = std::find(uri.begin(), uriEnd, '?');

        // protocol
        iterator_t protocolStart = uri.begin();
        iterator_t protocolEnd   = std::find(protocolStart, uriEnd, ':');            //"://");

        if (protocolEnd != uriEnd)
        {
            std::string prot = &*(protocolEnd);
            if ((prot.length() > 3) && (prot.substr(0, 3) == "://"))
            {
                result.Protocol = std::string(protocolStart, protocolEnd);
                protocolEnd += 3;   //      ://
            }
            else
                protocolEnd = uri.begin();  // no protocol
        }
        else
            protocolEnd = uri.begin();  // no protocol

        // host
        iterator_t hostStart = protocolEnd;
        iterator_t pathStart = std::find(hostStart, uriEnd, '/');  // get pathStart

        iterator_t hostEnd = std::find(protocolEnd,
            (pathStart != uriEnd) ? pathStart : queryStart,
            ':');  // check for port

        result.Host = std::string(hostStart, hostEnd);

        // port
        if ((hostEnd != uriEnd) && ((&*(hostEnd))[0] == ':'))  // we have a port
        {
            hostEnd++;
            iterator_t portEnd = (pathStart != uriEnd) ? pathStart : queryStart;
            result.Port = std::string(hostEnd, portEnd);
        }
        if ( !result.Port.length() && result.Protocol == "http" ) result.Port = "80";

        // path
        if (pathStart != uriEnd)
            result.Path = std::string(pathStart, queryStart);

        // query
        if (queryStart != uriEnd)
            result.QueryString = std::string(queryStart, uri.end());

        return result;
    }   // Parse
}                                       // namespace Exiv2
