#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <iostream>
#include <map>

#include <config.h>
#include <common.h>

#include "libs/log.h"
#include "libs/stringutil.h"
#include "libs/api_mysql.h"
#include "libs/server.h"
#include "libs/xmlparser.h"

static MysqlAPI mysqlApi;
XmlParser config_parser;

int handle_request(const std::string& request)
{
    key_value_t params;

    splitStringToMap(request, '&', '=', params);

    key_value_t::iterator iter = params.find("name");
    if (iter == params.end()) 
    {
        log_error("param error: miss 'name'");
        return -1;
    }
    iter = params.find("age");
    if (iter == params.end()) 
    {
        log_error("param error: miss 'age'");
        return -1;
    }

    std::string name = params["name"];
    std::string age = params["age"];

    MysqlRowSetPtr res = mysqlApi.query("SELECT age FROM es.lovers WHERE name='%s'", name.c_str());

    if (mysqlApi.update("UPDATE es.lovers SET age=%s WHERE name='%s'",  age.c_str(), name.c_str()) < 0) {
        log_error("update failed.");
        return -2;
    }

    return 0;
}

int main()
{
    init_log("server-1");	
    log_trace("service-1 start");

    config_parser.loadFile("../../" + Const::CONFIG_FILENAME);

    mysqlApi.init(config_parser.getValueByPath("/es/mysql/ip"),
            atoi(config_parser.getValueByPath("/es/mysql/port").c_str()),
            config_parser.getValueByPath("/es/mysql/user"),
            config_parser.getValueByPath("/es/mysql/passwd"),
            config_parser.getValueByPath("/es/mysql/db")
            );

    Server server("", Const::PORT, handle_request);

    server.start();

    return 0;
}
