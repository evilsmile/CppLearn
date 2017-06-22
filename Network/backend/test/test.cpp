#include <json/json.h>
#include <log.h>
#include <boost/scoped_ptr.hpp>

#include <string>

#include <url_util.h>

extern void test_httputil();

void test_json()
{
    // reader test
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

    Json::Reader reader;
    Json::Value jr_value;
    if (!reader.parse(test_json, jr_value)) {
        log_error("parse failed. not json: [%s]", test_json.c_str());
        return;
    }

    for (int i = 0; i < jr_value.size(); i++) {
        std::string name = jr_value[i]["name"].asString();
        Json::Value j_info = jr_value[i]["info"];
        int age = j_info["age"].asInt();

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
    //    test_json();
    // test_httputil();
    test_url_endecode();

    return 0;
}
