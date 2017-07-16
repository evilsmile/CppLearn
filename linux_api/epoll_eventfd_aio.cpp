/*
 * 该示例代码展示了异步IO、EPOLL和 eventfd的关联使用方式。
 *
 *     eventfd ---创建----添加----->  epoll
 *         \                          /|\
 *          \                          |
 *       通过结构体成员赋值            触发
 * 使用异步io与eventfd关联起来         |
 *          \|/                        |
 *           aio  -----------有事件后--
 */
#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <string>

#include <libaio.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>

void cf(io_context_t ctx, struct iocb *iocb, long res, long res2);

int main(int argc, char *argv[])
{
    int epfd;           // epoll fd
    int evfd;           // eventfd fd
    int fd;             // file fd

    void *buf;           // 异步IO读取出来的内容存放地
    struct epoll_event ev_event;  // 存放evfd
    struct epoll_event *events_list;
    struct iocb cb;            // 一个异步IO读取文件事件
    struct iocb *pcbs[1];       // io_submit 参数，包含上面的cb
    int n;
    uint64_t ready;             // 已完成的异步IO事件

    struct io_event *events_ret;     // io_getevents参数

    epfd = epoll_create(10);
    if (epfd < 0) {
        perror("epoll_create");
        exit(4);
    }

    // 初始化eventfd, 并将其描述符加入epoll
    evfd = eventfd(0, 0);
    if (evfd < 0) {
        perror("evfd");
        exit(5);
    }

    ev_event.events = EPOLLIN | EPOLLET;
    ev_event.data.ptr = NULL;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, evfd, &ev_event) != 0) {
        perror("epoll_ctl");
        exit(6);
    }

    std::string filename = argv[1];       // 要异步IO处理的文件

    io_context_t ctx = 0;        // 异步IO fd
    // 初始化异步IO上下文
    if (io_setup(1024, &ctx) != 0) {
        perror("io_setup");
        exit(3);
    }
    
    fd = open(filename.c_str(), O_RDONLY | O_CREAT, 0644);
    if (fd < 0) {
        perror("open");
        exit(7);
    }

    posix_memalign(&buf, 512, 1024);
    memset(&cb, '\0', sizeof(struct iocb));
    io_prep_pread(&cb, fd, buf, 1024, 0);
    // 关联触发fd。epoll相当于定义后续处理，aio定义事件
    io_set_eventfd(&cb, evfd);
    io_set_callback(&cb, cf);
    pcbs[0] = &cb;
    
    // 添加异步IO事件
    if (io_submit(ctx, 1, pcbs) != 1) {
        perror("io_submit");
        exit(8);
    }

    events_list = (struct epoll_event *)malloc(sizeof(struct epoll_event) * 32);

    while (true) {
        // 等待
        n = epoll_wait(epfd, events_list, 32, -1);
        if (n <= 0) {
            if (errno != EINTR) {
                perror("epoll_wait");
                exit(9);
            }
        } else {
            break;
        }
    }

    // 读取已完成的IO事件数量
    n = read(evfd, &ready, sizeof(ready));
    if (n != 8) {
        perror("read error");
        exit(10);
    }

    // 取出完成的异步IO事件并处理
    struct timespec ts;         // io_getevents 定时器
    ts.tv_sec = 0;
    ts.tv_nsec = 0;
    events_ret = (struct io_event*)malloc(sizeof(struct io_event) * 32);
    n = io_getevents(ctx, 1, 32, events_ret, &ts);

    printf("log: %ld events are ready; get %d events\n", ready, n);

    ((io_callback_t)(events_ret[0].data))(ctx, events_ret[0].obj, events_ret[0].res, events_ret[0].res2);

    io_destroy(ctx);
    free(buf);
    close(epfd);
    close(fd);
    close(evfd);

    return 0;
}

void cf(io_context_t ctx, struct iocb *iocb, long res, long res2)
{
    printf("can read %ld bytes, and in fact has read %ld bytes\n", iocb->u.c.nbytes, res);
    printf("the content is \n:%s",  (char*)iocb->u.c.buf);
}
