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

int send_reply(int fd, const std::string& reply)
{
    int written = 0;
    while (written < reply.size()) {
        int n = write(fd, reply.c_str() + written, reply.size() - written);
        if (n < 0) {
            log_error("send reply failed. written[%d]", written);
            return -1;
        }
        written += n;
    }

    log_error("send reply [%d] bytes", written);

    close(fd);

    return 0;
}

int handle_request(ClientConn client_conn)
{
    std::string request = client_conn.getData();
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

    send_reply(client_conn.getFD(), "SUCC");

    return 0;
}

int main()
{
    init_log("user_manager");

    int error = 0;

    error = config_parser.loadFile("../../config/" + Const::CONFIG_FILENAME);
    if (error) {
        return -1;
    }

    error = mysqlApi.init(config_parser.getValueByPath("/es/mysql/ip"),
            atoi(config_parser.getValueByPath("/es/mysql/port").c_str()),
            config_parser.getValueByPath("/es/mysql/user"),
            config_parser.getValueByPath("/es/mysql/passwd"),
            config_parser.getValueByPath("/es/mysql/db")
            );
    if (error) {
        return -1;
    }

    Server server("0.0.0.0", Const::PORT, handle_request);

    server.start();

    return 0;
}
