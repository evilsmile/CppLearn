#ifndef __HTTPUTIL_H__
#define __HTTPUTIL_H__

#include <curl/curl.h>
#include <curl/easy.h>
#include <string>

class HttpUtil {
    public:
        HttpUtil();

        bool get(const std::string& url);

        bool post(const std::string& url, const std::string& data);

    private:
        static int dowrite(void *buffer, size_t size, size_t nmemb, void *stream);

    private:
        int _received;
        std::string _reply_data;

};

#endif
