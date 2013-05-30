#include "exv_conf.h"
#include <exiv2/exiv2.hpp>

#if EXV_USE_CURL == 1
#include <curl/curl.h>
#include <string>
namespace Exiv2 {
    
    EXIV2API long remoteFileLength(CURL* curl, std::string path, long &length);
    EXIV2API long remoteDataByRange(CURL* curl, std::string path, const char* range, std::string& response);
    /*!
     * @brief The callback function is called by libcurl to write the data
     */
    EXIV2API size_t curlWriter(char *data, size_t size, size_t nmemb, std::string *writerData);
}
#endif


