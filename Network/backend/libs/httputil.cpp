#include <cstdlib>
#include <cstring>
#include <fstream>

#include "httputil.h"
#include "log.h"

static const int DEBUG_FLAG = 1;
static const int DEFAULT_TIMEOUT = 120;

HttpUtil::HttpUtil()
 : _received(0)
{
    curl_global_init(CURL_GLOBAL_ALL);

    _shared_curl = curl_share_init();
    curl_share_setopt(_shared_curl, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
}

HttpUtil::~HttpUtil()
{
    curl_global_cleanup();
}

void HttpUtil::set_shared_handle(CURL *handle) 
{
    curl_easy_setopt(handle, CURLOPT_SHARE, _shared_curl);
    curl_easy_setopt(handle, CURLOPT_DNS_CACHE_TIMEOUT, 60 * 6);
}

int HttpUtil::do_write(void *buffer, size_t size, size_t nmemb, void *data)
{
    HttpUtil *http_util = (HttpUtil*)data;

    std::string reply_data;

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
    headers_t headers;
    return get(url, headers, -1);
}

bool HttpUtil::get(const std::string& url, const headers_t& headers, int timeout)
{
    log_trace("Get url: %s", url.c_str());

    CURLcode res;
    CURL* curl = curl_easy_init();
    if (!curl) {
        log_error("curl init failed.");
        return false;
    }

    struct curl_slist *c_headers = NULL;
    for (int i = 0; i < headers.size(); ++i) {
        c_headers = curl_slist_append(c_headers, headers[i].c_str());
    }

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout>0?timeout:DEFAULT_TIMEOUT);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_VERBOSE, DEBUG_FLAG);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, do_write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
    if (c_headers) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, c_headers);
    }

    // 共享dns解析线程，加快速度
    set_shared_handle(curl);
    res = curl_easy_perform(curl);
    int status_code = 500;
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
    }
    curl_easy_cleanup(curl);
    if (c_headers) {
        curl_slist_free_all(c_headers);
    }

    if (res == CURLE_OK && status_code == 200) {
        log_trace("GET succ.");
    } else {
        log_error("Get failed: status_code[%d] errmsg[%s]", status_code, curl_easy_strerror(res));
        return false;
    }


    return true;
}

bool HttpUtil::post(const std::string& url, const std::string& data)
{
    headers_t headers;
    return post(url, data, headers, -1);
}

bool HttpUtil::post(const std::string& url, const std::string& data, const headers_t& headers, int timeout)
{
    log_trace("Post url: %s => %s", url.c_str(), data.c_str());

    CURLcode res;

    CURL* curl = curl_easy_init();
    if (!curl) {
        log_error("curl init failed.");
        return false;
    }

    struct curl_slist *c_headers = NULL;
    for (int i = 0; i < headers.size(); ++i) {
        c_headers = curl_slist_append(c_headers, headers[i].c_str());
    }

    if (c_headers) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, c_headers);
    }

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout>0?timeout:DEFAULT_TIMEOUT);
    curl_easy_setopt(curl, CURLOPT_ENCODING, "UTF-8");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(curl, CURLOPT_VERBOSE, DEBUG_FLAG);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, do_write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

    // 共享dns解析线程，加快速度
    set_shared_handle(curl);

    res = curl_easy_perform(curl);
    int status_code = 500;
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
    }
    curl_easy_cleanup(curl);
    if (c_headers) {
        curl_slist_free_all(c_headers);
    }

    if (res == CURLE_OK && status_code == 200) {
        log_trace("Post succ.");
    } else {
        log_error("Post failed: status_code[%d] errmsg[%s]", status_code, curl_easy_strerror(res));
        return false;
    }

    return true;
}

int HttpUtil::progress_cb(void *data, double dltotal, double dlnow, double ultotal, double ulnow)
{
    ProgressHandler *handler = (ProgressHandler*)data;
    handler->handle(dlnow, dltotal);

    return 0;
}

bool HttpUtil::downloadPage(const std::string& url, const std::string& outfile_name, bool show_progress, ProgressHandler* handler)
{
    CURLcode res;

    CURL* curl = curl_easy_init();

    if (!curl) {
        log_error("curl init failed.");
        return false;
    }

    FILE* out_fp = NULL;
    out_fp = fopen(outfile_name.c_str(), "w");
    if (!out_fp) {
        log_error("open file '%s' failed.", outfile_name.c_str());
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, out_fp);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_VERBOSE, DEBUG_FLAG);
    if (show_progress) {
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_cb);
        curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, handler);
    }

    set_shared_handle(curl);

    res = curl_easy_perform(curl);
    fclose(out_fp);

    int status_code = 500;
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
    }
    curl_easy_cleanup(curl);

    if (res == CURLE_OK && status_code == 200) {
        log_trace("download succ.");
    } else {
        log_error("Download failed: status_code[%d] errmsg[%s]", status_code, curl_easy_strerror(res));
        return false;
    }

    return true;
}
