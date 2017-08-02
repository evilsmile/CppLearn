#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>

#include <iostream>
#include <string>

#define EPSIZ 256

#define EXIT_MSG(msg) \
    std::cerr << "Error: " << msg << ". [" << strerror(errno) << "]" << std::endl; \
    exit(-1);

void usage()
{
    std::cerr << "Usage: filename" << std::endl;
    exit(-1);
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        usage();
    }

    std::string filename = argv[1];

    int e_fd = epoll_create(EPSIZ);
    if (e_fd < 0) {
        EXIT_MSG("epoll create failed.");
    }
    std::cout << "epoll fd: " << e_fd << std::endl;

    struct epoll_event f_event;

    int f_fd = open(filename.c_str(), O_RDONLY);
    if (f_fd < 0) {
        EXIT_MSG("open file failed.");
    }
    std::cout << "file fd: " << f_fd << std::endl;

    memset(&f_event, 0, sizeof(struct epoll_event));

    f_event.data.fd = f_fd;
    f_event.events = EPOLLIN | EPOLLET;

    if (epoll_ctl(e_fd, EPOLL_CTL_ADD, f_fd, &f_event)) {
        EXIT_MSG("add f_fd failed.");        
    }

    size_t events_size = EPSIZ * sizeof(struct epoll_event);
    struct epoll_event* events = (struct epoll_event*)malloc(events_size);
    memset(events, events_size, 0);

    while (true) {
        int n = epoll_wait(e_fd, events, EPSIZ, 2);

        for (int i = 0; i < n; i++) {
            struct epoll_event e = events[i];
            int fd = e.data.fd;
            std::cout << "get fd: " << fd << std::endl;
        }

    }

    return 0;
}
