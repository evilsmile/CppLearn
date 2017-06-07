#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <map>

#include <config.h>
#include <common.h>

#include "libs/log.h"
#include "libs/stringutil.h"
#include "libs/api_mysql.h"

static MysqlAPI mysqlApi;

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

		char sql[512] = {0};
		snprintf(sql, sizeof(sql), "SELECT age FROM es.lovers WHERE name='%s'", name.c_str());
		MysqlRowSetPtr res = mysqlApi.query(sql);

		memset(sql, 0, sizeof(sql));
		snprintf(sql, sizeof(sql), "UPDATE es.lovers SET age=%s WHERE name='%s'",  age.c_str(), name.c_str());
		if (mysqlApi.update(sql) < 0) {
				log_error("update failed.");
				return -2;
		}
		
		return 0;
}

int config_socket(int sk)
{
    int open = 1;
    if (setsockopt(sk, SOL_SOCKET, SO_REUSEADDR, &open, sizeof(int)) < 0) {
        close(sk);
        perror("set sock opt error");
        return -1;
    }

    if (setsockopt(sk, SOL_SOCKET, SO_KEEPALIVE, &open, sizeof(int)) < 0) {
        close(sk);
        perror("set sock opt error");
        return -1;
    }

    if (setsockopt(sk, SOL_SOCKET, SO_OOBINLINE, &open, sizeof(int)) < 0) {
        close(sk);
        perror("set sock opt error");
        return -1;
    }

    if (setsockopt(sk, SOL_SOCKET, SO_DONTROUTE, &open, sizeof(int)) < 0) {
        close(sk);
        perror("set sock opt error");
        return -1;
    }

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100;
    if (setsockopt(sk, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
        close(sk);
        perror("set sock opt error");
        return -1;
    }


    int nSendBuf = 1 * 1024;
    if (setsockopt(sk, SOL_SOCKET, SO_SNDBUF, (const char*)&nSendBuf, sizeof(int)) < 0) {
        close(sk);
        perror("set sock opt error");
        return -1;
    }

    int nRcvBuf = 1149;// * 1024;
    if (setsockopt(sk, SOL_SOCKET, SO_RCVBUF, (const char*)&nRcvBuf, sizeof(int)) < 0) {
        close(sk);
        perror("set sock opt error");
        return -1;
    }


    // Close immediately
    struct linger so_linger;
    so_linger.l_onoff = 1;
    so_linger.l_linger = 0;
    /*
    // 这里将导致close时发送RST包
    if (setsockopt(sk, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger)) < 0) {
        close(sk);
        perror("set sock opt error");
        return -1;
    }
    */

	return 0;
}



int main()
{
	init_log("server");	
	log_trace("service start");

    int opt_val;
    socklen_t opt_len;
    struct sockaddr_in serv_addr;

    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(Const::PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    int sk = socket(AF_INET, SOCK_STREAM, 0);
    if (sk < 0) {
        perror("create socket error");
        return -1;
    }

	config_socket(sk);

    if (bind(sk, (const struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        close(sk);
        perror("bind socket failed");
        return -1;
    }
    if (listen(sk, 1) < 0) {
        close(sk);
        perror("listen failed");
        return -1;
    }

	mysqlApi.init(Const::MYSQL_HOST_IP,
					Const::MYSQL_HOST_PORT,
					Const::MYSQL_USER,
					Const::MYSQL_PASSWD,
					Const::MYSQL_DB
					);

    while (true) {
        struct sockaddr_in cli_addr;
        socklen_t cli_len;
        int cli_fd = accept(sk, (struct sockaddr*)&cli_addr, &cli_len);

		std::string request;
		request.resize(1024);
        int ret = -1;
        ret = read(cli_fd, (char*)request.c_str(), request.size());
        if (ret < 0) {
            log_error("read client data error, code[%d]", ret);
            continue;
        }
		request.resize(ret);
        log_trace("recv client msg: %s", request.c_str());

		handle_request(request);

        log_trace("close fd");
    }

    return 0;
}
