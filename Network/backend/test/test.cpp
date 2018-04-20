#include <json/json.h>
#include <log.h>
#include <boost/scoped_ptr.hpp>

#include <string>

#include <url_util.h>

extern void test_httputil();

void test_json()
{
    // reader test
    std::string test_json = "[{\"desc\":\"7\",\"mobile\":\"18664314681\",\"report_status\":\"FAIL\",\"sid\":\"549281e3-0d9a-4ceb-a847-ec7a069d976e\",\"uid\":\"1234567890\",\"user_receive_time\":\"2018-03-20 09:22:57\"},{\"desc\":\"7\",\"mobile\":\"18664314681\",\"report_status\":\"FAIL\",\"sid\":\"7fc726bd-2252-40db-b63c-6da55573490c\",\"uid\":\"1234567890\",\"user_receive_time\":\"2018-03-20 09:23:01\"},{\"desc\":\"7\",\"mobile\":\"18664314681\",\"report_status\":\"FAIL\",\"sid\":\"d1a80e76-fce3-4378-bd24-da7f4e3ca030\",\"uid\":\"1234567890\",\"user_receive_time\":\"2018-03-20 09:23:05\"}]";
//    std::string test_json = "[{\"desc\":\"7\",\"report_status\":\"FAIL\",\"sid\":\"549281e3-0d9a-4ceb-a847-ec7a069d976e\",\"uid\":\"1234567890\",\"user_receive_time\":\"2018-03-20 09:22:57\"},{\"desc\":\"7\",\"report_status\":\"FAIL\",\"sid\":\"7fc726bd-2252-40db-b63c-6da55573490c\",\"uid\":\"1234567890\",\"user_receive_time\":\"2018-03-20 09:23:01\"},{\"desc\":\"7\",\"report_status\":\"FAIL\",\"sid\":\"d1a80e76-fce3-4378-bd24-da7f4e3ca030\",\"uid\":\"1234567890\",\"user_receive_time\":\"2018-03-20 09:23:05\"}]";
    /*
    std::string test_json = "["
        "{"
          "\"name\": \"evilsmile\", "
          "\"info\" : "
               "{ \"intere\": \"basketball\", \"age\": 29}"
        "},"
        "{"
          "\"name\": \"evilsmile\", "
          "\"info\" : "
               "{ \"intere\": \"basketball\", \"age\": 29}"
          "}"
         "]";
         */

    Json::Reader reader;
    Json::Value jr_value;
    if (!reader.parse(test_json, jr_value)) {
        log_error("parse failed. not json: [%s]", test_json.c_str());
        return;
    }

    for (int i = 0; i < jr_value.size(); i++) {
        std::string name = jr_value[i]["mobile"].asString();
        Json::Value j_info = jr_value[i]["mobile"];
//        int age = j_info["mobile"].asInt();
        int age = 1;

        log_trace("parse json result: name[%s] age[%d]", name.c_str(), age);
    }

    // writer test

    std::string cars[] = { "Lamborghini", "Meserati", "GTR" };
    Json::StyledWriter writer;
    //Json::FastWriter writer;
    Json::Value jw_root;
    Json::Value jw_sub;
    for (int i = 0; i < sizeof(cars)/sizeof(cars[0]); i++) {
        jw_sub["brand"] = cars[i];
        jw_sub["indx"] = i;

        jw_root.append(jw_sub);
    }

    std::string new_json = writer.write(jw_root);
    log_trace("new_json: %s", new_json.c_str());
}


void test_url_endecode()
{
    std::string test = "123 45&*89";
    std::cout << url_encode(test) << std::endl;
    std::cout << url_decode(url_encode(test)) << std::endl;
}

int main()
{
     test_json();
    // test_httputil();
    //test_url_endecode();

    return 0;
}
