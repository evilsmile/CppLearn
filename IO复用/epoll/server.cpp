#include <iostream>
#include <string>

#include <sys/epoll.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

static int create_and_bind(const std::string& port)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int s, sfd;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    s = getaddrinfo(NULL, const_cast<char*>(port.c_str()), &hints, &result);
    if (s != 0) {
        std::cerr << "get address info failed: " << strerror(errno) << std::endl;
        return -1;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) {
            continue;
        }

        s = ::bind(sfd, rp->ai_addr, rp->ai_addrlen);
        if (s == 0) {
            break;
        }
        close(sfd);
    }
    if (rp == NULL) {
        std::cerr << "Bind failed!" << std::endl;
        return -2;
    }
    freeaddrinfo(result);

    return sfd;
}

static int make_socket_non_blocking(int sfd)
{
    int flags, s;

    flags = ::fcntl(sfd, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "fcntl get flag failed!" << std::endl;
        return -1;
    }

    flags |= O_NONBLOCK;
    s = fcntl(sfd, F_SETFL, flags);
    if (s == -1) {
        std::cerr << "fcntl set flag failed!" << std::endl;
        return -1;
    }

    return 0;
}

#define MAXEVENTS 64
static const std::string port = "8880";

int main()
{
    int sfd, s;
    int efd;
    struct epoll_event event;
    struct epoll_event *events;

    sfd = create_and_bind(port);
    if (sfd < 0) {
        return -1;
    }

    s = make_socket_non_blocking(sfd);
    if (s < 0) {
        return -1;
    }

    s = listen(sfd, SOMAXCONN);
    if (s == -1) {
        std::cerr << "listen failed" << std::endl;
        return -1;
    }

    efd = epoll_create1(0);
    if (efd < 0) {
        std::cerr << "epoll_create1 failed" << std::endl;
        return -1;
    }

    event.data.fd = sfd;
    event.events = EPOLLIN | EPOLLET;
    s = epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &event);
    if (s < 0) {
        std::cerr << "epoll_ctl failed" << std::endl;
        return -1;
    }

    events = reinterpret_cast<struct epoll_event*>(calloc(MAXEVENTS, sizeof event));

    while (true) {
        int n, i;
        n = epoll_wait(efd, events, MAXEVENTS, -1);
        for (i = 0; i < n; ++i) {
            if ((events[i].events & EPOLLERR) ||
                    (events[i].events & EPOLLHUP) ||
                    (!(events[i].events & EPOLLIN))) {
                std::cerr << "epoll wait error." << std::endl;
                close(events[i].data.fd);
                continue;
            }
            if (sfd == events[i].data.fd) {
                while (true) {
                    struct sockaddr in_addr;
                    socklen_t in_len;
                    int infd;
                    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
                    infd = accept(sfd, &in_addr, &in_len);
                    if (infd < 0) {
                        if ((errno == EAGAIN) || errno == EWOULDBLOCK) {
                            std::cout << "WARN: EAGAIN/EWOULDBLOCK" << std::endl;
                            break;
                        }
                        else {
                            std::cerr << "accept error." << strerror(errno) << std::endl;
                            break;
                        }
                    }

                    s = getnameinfo(&in_addr, in_len, hbuf, sizeof hbuf, 
                            sbuf, sizeof sbuf, 
                            NI_NUMERICHOST | NI_NUMERICSERV);

                    if (s == 0) {
                        std::cout << "Accept connection (host: " << std::string(hbuf) << ", port: " << std::string(sbuf) << std::endl;
                    }

                    s = make_socket_non_blocking(infd);
                    if (s == -1) {
                        return -1;
                    }

                    event.data.fd = infd;
                    event.events = EPOLLIN | EPOLLET;
                    s = epoll_ctl(efd, EPOLL_CTL_ADD, infd, &event);
                    if (s == -1) {
                        std::cerr << "epoll_ctl add client failed." << std::endl;
                        return -1;
                    }
                }
                continue;
            }
            else {
                int done = 0;
                while (true) {
                    ssize_t count;
                    char buf[512];

                    count = read(events[i].data.fd, buf, sizeof buf);
                    if (count == -1) {
                        if (errno != EAGAIN) {
                            std::cerr << "read data from client error." << std::endl;
                            done = 1;
                            break;
                        }
                    }
                    else if (count == 0) {
                        done = 1;
                        break;
                    }

                    s = write(1, buf, count);
                }

                if (done) {
                    std::cout << "Close connection on fd: " << events[i].data.fd << std::endl;
                    close(events[i].data.fd);
                }
            }
        }
    }

    free(events);
    close(sfd);

    return 0;
}
