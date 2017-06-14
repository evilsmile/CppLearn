#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <cstdlib>

#include <errno.h>

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

	_config_socket();

    if (bind(_sock_fd, (const struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        close(_sock_fd);
        log_error("bind socket failed");
        return -1;
    }
    if (listen(_sock_fd, SOMAXCONN) < 0) {
        close(_sock_fd);
        log_error("listen failed");
        return -1;
    }

    int ep_fd = epoll_create1(0);
    if (ep_fd < 0) {
        log_error("epoll_create1 failed");
        return -1;
    }

    struct epoll_event event;
    struct epoll_event *events;

    event.data.fd = _sock_fd;
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(ep_fd, EPOLL_CTL_ADD, _sock_fd, &event) < 0) {
        log_error("add listen socket to epoll failed");
        return -1;
    }

#define MAXEVENTS 64
    events = reinterpret_cast<struct epoll_event*>(calloc(MAXEVENTS, sizeof event));

    log_trace("start epolling...");

    while (true) {
        int n = epoll_wait(ep_fd, events, MAXEVENTS, -1);
        for (int i = 0; i < n; ++i) {
            if ((events[i].events & EPOLLERR) ||
                    (events[i].events & EPOLLHUP) ||
                    (!(events[i].events & EPOLLIN))) {
                    log_error("epoll_wait on fd[%d] failed", events[i].data.fd);
                    continue;
            }

            if (_sock_fd == events[i].data.fd) {
                struct sockaddr cli_addr;
                socklen_t cli_len;
                char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
                int cli_fd = accept(_sock_fd, (struct sockaddr*)&cli_addr, &cli_len);
                if (cli_fd < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        log_error("WARNING: EAGAIN/EWOULDBLOCK");
                        break;
                    } else {
                        log_error("accept error: [%s]", strerror(errno));
                        break;
                    }
                }
                log_trace("new client fd: %d", cli_fd);

                if (_set_nonblock(cli_fd)) {
                    log_error("set client fd non-block faile. [%d]", cli_fd);
                    break;
                }

                event.data.fd = cli_fd;
                event.events = EPOLLIN | EPOLLET;
                if (epoll_ctl(ep_fd, EPOLL_CTL_ADD, cli_fd, &event) < 0) {
                    log_error("add client fd [%d] to epoll failed. [%s]", cli_fd, strerror(errno));
                    break;
                }
                log_trace("new connection on client fd [%d]", cli_fd);
            } else {
                bool done = false;
                ssize_t readed_n = 0;
                std::string request;
                request.resize(1024);

                int cli_fd = events[i].data.fd;

                while (!done) {
                    int count = read(cli_fd, (char*)request.c_str() + readed_n, request.size() - readed_n);
                    if (count < 0) {
                        log_error("read client data error, code[%d:%s]", count, strerror(errno));
                        break;
                    } else if (count == 0) {
                        done = true;
                    } else {
                        readed_n += count;
                    }
                }
                request.resize(readed_n);
                log_trace("recv client msg: %s", request.c_str());

                ClientConn client_conn(cli_fd, request);
                (*_handler)(client_conn);
            }
        }
    }
}

int Server::_config_socket()
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

int Server::_set_nonblock(int fd)
{
    int flags, s;

    flags = ::fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        log_error("fcntl get flag failed!");
        return -1;
    }

    flags |= O_NONBLOCK;
    s = fcntl(fd, F_SETFL, flags);
    if (s == -1) {
        log_error("fcntl set flag failed!");
        return -1;
    }

    return 0;
}
