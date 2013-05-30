/*
 * remote.cpp
 */
#include "remote.hpp"

#if EXV_USE_CURL == 1
namespace Exiv2 {

    long remoteFileLength(CURL* curl, std::string path, long &length) {
        long returnCode;
        length = 0;

        curl_easy_reset(curl); // reset all options
        std::string response;
        curl_easy_setopt(curl, CURLOPT_URL, path.c_str());
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1); // HEAD
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriter);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); // debugging mode

        /* Perform the request, res will get the return code */
        CURLcode res = curl_easy_perform(curl);
        if(res != CURLE_OK) { // error happends
            throw Error(1, curl_easy_strerror(res));
        } else {
            // get return code
            curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &returnCode); // get code

            // get length
            double temp;
            curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &temp); // return -1 if unknown
            length = (long) temp;
        }

        return returnCode;
    }

    long remoteDataByRange(CURL* curl, std::string path, const char* range, std::string& response) {
        long code = -1;

        curl_easy_reset(curl); // reset all options
        curl_easy_setopt(curl, CURLOPT_URL, path.c_str());
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L); // no progress meter please
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriter);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); // debugging mode

        if (strcmp(range, ""))
            curl_easy_setopt(curl, CURLOPT_RANGE, range);

        /* Perform the request, res will get the return code */
        CURLcode res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            throw Error(1, curl_easy_strerror(res));
        } else {
            curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &code); // get code
        }

        return code;
    }

    size_t curlWriter(char *data, size_t size, size_t nmemb, std::string *writerData) {
        if (writerData == NULL) return 0;
        writerData->append(data, size*nmemb);
        return size * nmemb;
    }

}
#endif
// That's all Folks
