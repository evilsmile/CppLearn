#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#include <string>

#include <cgicc/Cgicc.h>

#include <fcgi_stdio.h>

#include "libs/log.h"
#include "config.h"

int main()
{	  
	init_log("test.cgi");
	while (FCGI_Accept() >= 0) { 
			do { 
					log_trace("Enter...");

					cgicc::Cgicc cgicc;
					const cgicc::CgiEnvironment& env = cgicc.getEnvironment();
					std::string query_string = env.getQueryString();

					int opt_val;
					socklen_t opt_len;
					struct sockaddr_in serv_addr;
					int open = 1;

					memset(&serv_addr, 0, sizeof(struct sockaddr_in));
					serv_addr.sin_family = AF_INET;
					serv_addr.sin_port = htons(Const::PORT);
					inet_pton(AF_INET, "localhost", &serv_addr.sin_addr.s_addr);

					int sk = socket(AF_INET, SOCK_STREAM, 0);
					if (sk < 0) {
							log_error("create socket error");
							break;
					}

					if (connect(sk, (const struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
							close(sk);
							log_error("connect socket failed");
							break;
					}
					log_trace("query_string: %s", query_string.c_str());
					int ret = write(sk, query_string.c_str(), query_string.size());
					log_trace("write %d bytes", ret);

					close(sk);
					
					FCGI_printf("Content-type: text/html; charset=utf-8\r\n\r\n");
			} while(0);
	}

	return 0;
}
