#include <cstdlib>
#include <cstring>

#include "httputil.h"
#include "log.h"

HttpUtil::HttpUtil()
 : _curl(NULL), _received(0)
{
}

bool HttpUtil::init()
{
    _curl = curl_easy_init();
    if (!_curl) {
        log_error("curl init failed");
        return false;
    }

    return true;
}


int HttpUtil::dowrite(void *buffer, size_t size, size_t nmemb, void *data)
{
    HttpUtil *http_util = (HttpUtil*)data;

    std::string reply_data;
    int received = http_util->_received;

    size_t data_len = size * nmemb;
    reply_data.resize(data_len);

    memcpy((char*)reply_data.c_str(), (char*)buffer, data_len);
    http_util->_received += data_len;
    http_util->_reply_data += reply_data;

    log_trace("reply_data: %s", http_util->_reply_data.c_str());

    return data_len;
}

bool HttpUtil::get(const std::string& url)
{
    log_trace("Get url: %s", url.c_str());

    CURLcode res;

    if (_curl == NULL) {
        if (!init()) {
            return false;
        }
    }

    curl_easy_setopt(_curl, CURLOPT_TIMEOUT, 120);
    curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, dowrite);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, this);

    res = curl_easy_perform(_curl);
    int status_code = 500;
    if (res == CURLE_OK) {
        curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &status_code);
    }
    curl_easy_cleanup(_curl);

    if (res == CURLE_OK && status_code == 200) {
        log_trace("access url succ");
    } else {
        log_error("access url failed: status_code[%d] errmsg[%s]", status_code, curl_easy_strerror(res));
        return false;
    }


    return true;
}
