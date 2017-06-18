#ifndef __HTTPUTIL_H__
#define __HTTPUTIL_H__

#include <curl/curl.h>
#include <curl/easy.h>
#include <string>
#include <vector>

class HttpUtil {
    public:
        class ProgressHandler {
            public:
                virtual int handle(int now, int total) = 0;
        };

    public:

        typedef std::vector<std::string> headers_t;
        HttpUtil();
        ~HttpUtil();

        bool get(const std::string& url);

        bool get(const std::string& url, const headers_t& headers, int timeout);

        bool post(const std::string& url, const std::string& data);

        bool post(const std::string& url, const std::string& data, const headers_t& headers, int timeout);

        bool downloadPage(const std::string& url, const std::string& outfile_name, bool show_progress = false, ProgressHandler* handler = NULL);

    private:
        static int do_write(void *buffer, size_t size, size_t nmemb, void *stream);
        static int progress_cb(void *data, double t, double d, double ultotal, double ulnow);

        void set_shared_handle(CURL *handle);

        CURLSH* _shared_curl;
    private:
        int _received;
        std::string _reply_data;

};

#endif
