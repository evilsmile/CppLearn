#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <errno.h>

#include <iostream>
#include <cstdio>
#include <cstring>

static int efd = -1;

#define PERR(msg)   \
            std::cerr << (msg) << strerror(errno) << std::endl

void *read_thread(void *dummy)
{
    int ret = 0;
    uint64_t count = 0;
    int ep_fd = -1;
    struct epoll_event events[10];

    if (efd < 0) {
        PERR("efd not initialized!");
        goto fail;
    }

    ep_fd = epoll_create(1024);
    if (ep_fd < 0) {
        PERR("epoll_create fail. ");
        goto fail;
    }

    {
        struct epoll_event read_event;
        read_event.events = EPOLLHUP | EPOLLERR | EPOLLIN;
        read_event.data.fd = efd;

        ret = epoll_ctl(ep_fd, EPOLL_CTL_ADD, efd, &read_event);
        if (ret < 0) {
            PERR("epoll_ctl ADD fail. ");
            goto fail;
        }
    }
    while (true) {
        ret = epoll_wait(ep_fd, &events[0], 10, 5000);
        if (ret > 0) {
            for (int i = 0; i < ret; ++i) {
                if (events[i].events & EPOLLHUP) {
                    PERR("epoll eventfd hup. ");
                    goto fail;
                } else if (events[i].events & EPOLLERR) {
                    PERR("epoll eventfd error. ");
                    goto fail;
                } else if (events[i].events & EPOLLIN) {
                    int event_fd = events[i].data.fd;
                    ret = read(event_fd, &count, sizeof(count));
                    if (ret < 0) {
                        PERR("read failed.");
                        goto fail;
                    } else {
                        struct timeval tv;
                        gettimeofday(&tv, NULL);
                        printf("success read from efd, read %d bytes(%ld) at %lds %ldus\n",
                                ret, count, tv.tv_sec, tv.tv_usec);
                    }
                }
            }
        } else if (ret == 0) {
            std::cout << "epoll wait timeout" << std::endl;
            break;
        } else {
            PERR("epoll wait error.");
            goto fail;
        } 
    }

fail:
    if (ep_fd >= 0) {
        close(ep_fd);
        ep_fd = -1;
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t pid = 0;
    uint64_t count = 0;
    int ret = 0;
    do {
        efd = eventfd(0, 0);
        if (efd < 0) {
            PERR("event fd failed");
            break;
        }

        ret = pthread_create(&pid, NULL, read_thread, NULL);
        if (ret < 0) {
            PERR("pthread create failed.");
            break;
        }
        for (int i = 0; i < 5; ++i) {
            count = 4;
            ret = write(efd, &count, sizeof(count));
            if (ret < 0) {
                PERR("write eventfd failed.");
                goto failed;
            } else {
                struct timeval tv;
                gettimeofday(&tv, NULL);
                printf("success write to efd, write %d bytes(%lu) at %lds %ldus\n",
                        ret, count, tv.tv_sec, tv.tv_usec);
            }
            sleep(1);
        }

    } while(0);

failed:
        if (0 != pid) {
            pthread_join(pid, NULL);
            pid = 0;
        }

        if (efd >= 0) {
            close(efd);
            efd = -1;
        }

    return 0;
}
