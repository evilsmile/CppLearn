#include <httputil.h>
#include <log.h>

#include <string>

class MyProgressHandler : public HttpUtil::ProgressHandler {
    public:
        virtual int handle(int dlnow, int dltotal) {
            log_trace("(%d / %d (%g %%))", dlnow, dltotal, dlnow*100.0/dltotal);
        }
};

static void test_http()
{
    HttpUtil http_util;

    MyProgressHandler myPH;
    http_util.downloadPage("www.sina.com.cn", "outfile", true, &myPH);
  //  http_util.get("www.baidu.com");

    std::string post_data = " {"
        "\"from\":\"h5\","
            "\"to\":\"trade\","
            "\"info\":{},"
            "\"action\":\"pay\","
            "\"data\":{},"
            "\"pay\":{"
                "\"user_agent\":\"alipay\","
                "\"amount\":1,"
                "\"goods_name\":\"phone\","
                "\"mch_id\":\"160000000020939\","
                "\"mch_name\":\"evil\","
                "\"order_id\":\"123123124124124\","
                "\"term_id\":\"243\""
            "}"
       "}";

    HttpUtil::headers_t headers;
    headers.push_back("User-Agent: ssss");
#define TEST_CNT 1
    for(int i = 0; i < TEST_CNT; ++i) {
        if (http_util.post("http://qrpos.tfb8.com/cgi-bin/tfb_gate.cgi", post_data, headers, -1) == false) {
            log_error("post data failed.");
            return ;
        }
    }


}

static void test_ftp()
{
    HttpUtil http_util;

    static const std::string ftp_url = "ftp://119.23.49.157";
    static const std::string get_user_passwd = "admin:12345678";
    static const std::string put_user_passwd = "upload:12345678";

//    http_util.ftp_get(ftp_url, get_user_passwd, "tfb_succ.list.txt", "mydown");
    http_util.ftp_put(ftp_url, put_user_passwd, "Makefile");
}

void test_httputil()
{
    //test_http();

    test_ftp();
}
