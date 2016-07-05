#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>

#include <string>
#include <iostream>

static const std::string IPADDR = "127.0.0.1";
static const int PORT = 8787;
static const int LISTENQ = 5;

class Server {
public:
	Server(const std::string& ip, int port) : _ip(ip), _port(port){};
	~Server();
	int init();
	int create_server_proc();
	void handle_client_proc(int srvfd);

private:
	static const int MAXLINE = 1024;
	static const int SIZE = 10;

private:
	class ServerContext{
	public:
		ServerContext() : cli_cnt(0), maxfd(0) { 
			for (int i = 0; i < SIZE; ++i) {
				clifds[i] = -1;
			}
		}
	public:
		int cli_cnt;
		int clifds[SIZE];
		fd_set allfds;
		int maxfd;
	} ;

private:
	int accept_client_proc(int srvfd);
	int handle_client_msg(int fd, char *buf);
	void recv_client_msg(fd_set* readfds);

private:
	std::string _ip;
	int _port;
	ServerContext* s_srv_ctx;
};

Server::~Server()
{
	if (s_srv_ctx) {
		delete s_srv_ctx;
		s_srv_ctx = NULL;
	}
}

int Server::init()
{
	s_srv_ctx = new ServerContext();
	if (s_srv_ctx == NULL) {
		return -1;
	}

	return 0;
} 

int Server::create_server_proc()
{
	int fd;
	struct sockaddr_in servaddr;
	fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1) {
		std::cerr << "create socket fd failed! [ " << strerror(errno) << "]" << std::endl;
		return -1;
	}

	int yes = 1;
	if (::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			return -1;
	}

	int reuse = -1;
	if (::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
			return -1;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, _ip.c_str(), &servaddr.sin_addr);
	servaddr.sin_port = htons(_port);

	if (::bind(fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
		std::cerr << "bind socket failed! [" << strerror(errno) << "]" << std::endl;
		return -2;
	}
	::listen(fd, LISTENQ);

	return fd;
}

int Server::accept_client_proc(int srvfd)
{
	struct sockaddr_in cliaddr;
	socklen_t cliaddrlen;
	cliaddrlen = sizeof(cliaddr);
	int clifd = -1;

	std::cout << "Accept client proc is called!" << std::endl;

ACCEPT:
	clifd = ::accept(srvfd, (struct sockaddr*)&cliaddr, &cliaddrlen);

	if (clifd == -1) {
		if (errno == EINTR) {
			goto ACCEPT;
		} else {
			std::cerr << "accept failed![ " << strerror(errno) << "]" << std::endl;
			return -3;
		}
	}

	std::cout << "accept a new client: " << inet_ntoa(cliaddr.sin_addr) << ":" << cliaddr.sin_port << std::endl;

	int i = 0;
	for (i = 0; i < SIZE; ++i) {
		// 收到新的连接，找一个空闲的clifds[]位置填入
		if (s_srv_ctx->clifds[i] < 0) {
			s_srv_ctx->clifds[i] = clifd;
			//增加客户端数量
			++s_srv_ctx->cli_cnt;
			break;
		}
	}
	if (i == SIZE) {
		std::cerr << "Client over too much!" << std::endl;
		return -4;
	}
}

int Server::handle_client_msg(int fd, char *buf) 
{
	assert(buf);
	std::cout << "recv buf is : " << buf << std::endl;
	// 回发客户端
	write(fd, buf, strlen(buf) + 1);

	return 0;
}

void Server::recv_client_msg(fd_set* readfds)
{
	int i = 0, n = 0;
	int clifd;
	char buf[MAXLINE] = {0};
	// 遍历所有连接的客户端描述符
	for (i = 0; i <= s_srv_ctx->cli_cnt; ++i) {
		clifd = s_srv_ctx->clifds[i];
		if (clifd < 0) {
			continue;
		}

		// clifd对应的readfds中的相应位被置位，则表示该客户端有数据
		if (FD_ISSET(clifd, readfds)) {
			n = read(clifd, buf, MAXLINE);
			if (n <= 0) {
				// n<=0 表示连接关闭或异常，清掉
				FD_CLR(clifd, &s_srv_ctx->allfds);
				// 发送FIN给客户端正常关闭
				close(clifd);
				//重置
				s_srv_ctx->clifds[i] = -1;
				continue;
			}
			handle_client_msg(clifd, buf);
		}
	}
}

void Server::handle_client_proc(int srvfd)
{
	int clifd = -1;
	int retval = 0;
	fd_set *readfds = &s_srv_ctx->allfds;
	struct timeval tv;
	int i = 0;

	while(true) {
		FD_ZERO(readfds);
		FD_SET(srvfd, readfds);
		s_srv_ctx->maxfd = srvfd;

		tv.tv_sec = 30;
		tv.tv_usec = 0;

		for (i = 0; i < s_srv_ctx->cli_cnt; ++i) {
			clifd = s_srv_ctx->clifds[i];
			FD_SET(clifd, readfds);
			s_srv_ctx->maxfd = (clifd > s_srv_ctx->maxfd ? clifd : s_srv_ctx->maxfd);
		}

		//!!!!
		retval = select(s_srv_ctx->maxfd + 1, readfds, NULL, NULL, &tv);

		if (retval == -1) {
			std::cerr << "select error![" << strerror(errno) << "]" << std::endl;
			return;
		}

		if (retval == 0) {
			std::cout << "select timeout!" << std::endl;
			return;
		}

		// 如果是srvfd，则表示有客户端连接，调用accept处理函数
		if (FD_ISSET(srvfd, readfds)) {
			accept_client_proc(srvfd);
		} else {
			recv_client_msg(readfds);
		}
	}
}

int main(int argc, char* argv[])
{
	Server server(IPADDR, PORT);
	if (server.init() != 0) {
		return -1;		
	}
	int srvfd;
	srvfd = server.create_server_proc();
	if (srvfd < 0) {
		return -2;
	}
	server.handle_client_proc(srvfd);

	return 0;
}
