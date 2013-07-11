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

// + standard includes
#include <sys/types.h>
#include <sys/stat.h>
#ifdef _MSC_VER
# define S_ISREG(m)      (((m) & S_IFMT) == S_IFREG)
#endif
#ifdef EXV_HAVE_UNISTD_H
# include <unistd.h>                     // for stat()
#endif

#include "futils.hpp"
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
    const char* ENVARDEF[] = {"exiv2.php"};
    const char* ENVARKEY[] = {"EXIV2_HTTP_POST"};
// *****************************************************************************
// free functions
    std::string getEnv(EnVar var) {
        return getenv(ENVARKEY[var]) ? getenv(ENVARKEY[var]) : ENVARDEF[var];
    }

    char to_hex(char code) {
        static char hex[] = "0123456789abcdef";
        return hex[code & 15];
    }

    char from_hex(char ch) {
        return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
    }

    char* urlencode(char* str) {
        char* pstr = str;
        char* buf  = (char*)malloc(strlen(str) * 3 + 1);
        char* pbuf = buf;
        while (*pstr) {
            if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~')
                *pbuf++ = *pstr;
            else if (*pstr == ' ')
                *pbuf++ = '+';
            else
                *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
            pstr++;
        }
        *pbuf = '\0';
        return buf;
    }

    char* urldecode(const char* str) {
        const char* pstr = str;
        char* buf  = (char*)malloc(strlen(str) + 1);
        char* pbuf = buf;
        while (*pstr) {
            if (*pstr == '%') {
                if (pstr[1] && pstr[2]) {
                    *pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
                    pstr += 2;
                }
            } else if (*pstr == '+') {
                *pbuf++ = ' ';
            } else {
                *pbuf++ = *pstr;
            }
            pstr++;
        }
        *pbuf = '\0';
        return buf;
    }

    int base64encode(const void* data_buf, size_t dataLength, char* result, size_t resultSize) {
        const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        const uint8_t* data = (const uint8_t*)data_buf;
        size_t resultIndex = 0;
        size_t x;
        uint32_t n = 0;
        size_t padCount = dataLength % 3;
        uint8_t n0, n1, n2, n3;

        /* increment over the length of the string, three characters at a time */
        for (x = 0; x < dataLength; x += 3)
        {
            /* these three 8-bit (ASCII) characters become one 24-bit number */
            n = data[x] << 16;

            if((x+1) < dataLength)
                n += data[x+1] << 8;

            if((x+2) < dataLength)
                n += data[x+2];

            /* this 24-bit number gets separated into four 6-bit numbers */
            n0 = (uint8_t)(n >> 18) & 63;
            n1 = (uint8_t)(n >> 12) & 63;
            n2 = (uint8_t)(n >> 6) & 63;
            n3 = (uint8_t)n & 63;

            /*
            * if we have one byte available, then its encoding is spread
            * out over two characters
            */
            if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
            result[resultIndex++] = base64chars[n0];
            if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
            result[resultIndex++] = base64chars[n1];

            /*
            * if we have only two bytes available, then their encoding is
            * spread out over three chars
            */
            if((x+1) < dataLength)
            {
                if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
                result[resultIndex++] = base64chars[n2];
            }

            /*
            * if we have all three bytes available, then their encoding is spread
            * out over four characters
            */
            if((x+2) < dataLength)
            {
                if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
                result[resultIndex++] = base64chars[n3];
            }
        }

        /*
        * create and add padding that is required if we did not have a multiple of 3
        * number of characters available
        */
        if (padCount > 0)
        {
            for (; padCount < 3; padCount++)
            {
                if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
                result[resultIndex++] = '=';
            }
        }
        if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
        result[resultIndex] = 0;
        return 1;   /* indicate success */
    }

    Protocol fileProtocol(const std::string& path) {
        Protocol result = pFile ;
        static Exiv2::protDict_t protDict;
        if (!protDict.size()) {
             protDict["http://"]  = pHttp;
             protDict["ftp://"]   = pFtp;
             protDict["https://"] = pHttps;
             protDict["sftp://"]  = pSftp;
             protDict["ssh://"]   = pSsh;
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
        // Issue# 908.
        // report strerror() if strerror_r() returns empty
        if ( !buf[0] ) os << strerror(error);
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

        //username & password
        iterator_t authStart = protocolEnd;
        iterator_t authEnd = std::find(protocolEnd, uriEnd, '@');
        if (authEnd != uriEnd) {
            iterator_t userStart = authStart;
            iterator_t userEnd   = std::find(authStart, authEnd, ':');
            if (userEnd != authEnd) {
                result.Username = std::string(userStart, userEnd);
                userEnd++;
                result.Password = std::string(userEnd, authEnd);
            } else {
                result.Username = std::string(authStart, authEnd);
            }
            authEnd++;
        } else {
          authEnd = protocolEnd;
        }

        // host
        iterator_t hostStart = authEnd;
        iterator_t pathStart = std::find(hostStart, uriEnd, '/');  // get pathStart

        iterator_t hostEnd = std::find(authEnd,
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