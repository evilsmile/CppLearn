#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>

#include <log.h>

#include "server.h"

Server::Server(const std::string& ip, int port)
    : _ip(ip), _port(port)
{

}

Server::Server(const std::string& ip, int port, handle_request_f handler)
    : _ip(ip), _port(port), _handler(handler)
{
}

void Server::setHandler(handle_request_f new_handler)
{
    _handler = new_handler;
}

int Server::start()
{
    log_trace("service start with [%s:%d]", _ip.c_str(), _port);
    int opt_val;
    socklen_t opt_len;
    struct sockaddr_in serv_addr;

    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(_port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    _sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sock_fd < 0) {
        log_error("create socket error");
        return -1;
    }

	config_socket();

    if (bind(_sock_fd, (const struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        close(_sock_fd);
        log_error("bind socket failed");
        return -1;
    }
    if (listen(_sock_fd, 1) < 0) {
        close(_sock_fd);
        log_error("listen failed");
        return -1;
    }

    log_trace("server fd init succ");
    log_trace("accepting...");
    while (true) {
        struct sockaddr_in cli_addr;
        socklen_t cli_len;
        int cli_fd = accept(_sock_fd, (struct sockaddr*)&cli_addr, &cli_len);
        if (cli_fd < 0) {
            log_error("invalid client fd: %d", cli_fd);
            continue;
        }

        std::string request;
        request.resize(1024);
        int ret = -1;
        log_trace("client fd: %d", cli_fd);
        ret = read(cli_fd, (char*)request.c_str(), request.size());
        if (ret < 0) {
            log_error("read client data error, code[%d]", ret);
            continue;
        }
        request.resize(ret);
        log_trace("recv client msg: %s", request.c_str());

        (*_handler)(request);

        log_trace("close fd");
    }
}

int Server::config_socket()
{
    int open = 1;
    if (setsockopt(_sock_fd, SOL_SOCKET, SO_REUSEADDR, &open, sizeof(int)) < 0) {
        close(_sock_fd);
        log_error("set sock opt error");
        return -1;
    }

    if (setsockopt(_sock_fd, SOL_SOCKET, SO_KEEPALIVE, &open, sizeof(int)) < 0) {
        close(_sock_fd);
        log_error("set sock opt error");
        return -1;
    }

    if (setsockopt(_sock_fd, SOL_SOCKET, SO_OOBINLINE, &open, sizeof(int)) < 0) {
        close(_sock_fd);
        log_error("set sock opt error");
        return -1;
    }

    if (setsockopt(_sock_fd, SOL_SOCKET, SO_DONTROUTE, &open, sizeof(int)) < 0) {
        close(_sock_fd);
        log_error("set sock opt error");
        return -1;
    }

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100;
    if (setsockopt(_sock_fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
        close(_sock_fd);
        log_error("set sock opt error");
        return -1;
    }


    int nSendBuf = 1 * 1024;
    if (setsockopt(_sock_fd, SOL_SOCKET, SO_SNDBUF, (const char*)&nSendBuf, sizeof(int)) < 0) {
        close(_sock_fd);
        log_error("set sock opt error");
        return -1;
    }

    int nRcvBuf = 1149;// * 1024;
    if (setsockopt(_sock_fd, SOL_SOCKET, SO_RCVBUF, (const char*)&nRcvBuf, sizeof(int)) < 0) {
        close(_sock_fd);
        log_error("set sock opt error");
        return -1;
    }


    // Close immediately
    struct linger so_linger;
    so_linger.l_onoff = 1;
    so_linger.l_linger = 0;

	return 0;
}


