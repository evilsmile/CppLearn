#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>

#define UNIXSTR_PATH  "foo.socket"

int main(int argc, char* argv[])
{
    int clifd, listenfd;
    struct sockaddr_un servaddr, cliaddr;
    int ret;
    socklen_t clilen;
    struct msghdr msg;
    struct iovec iov[1];
    char buf[100];
    char *testmsg = "test msg.\n";

    union {
        struct cmsghdr cm;
        char control[CMSG_SPACE(sizeof(int))];
    }control_un;

    struct cmsghdr *pcmsg;

    int recvfd;

    listenfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        printf("socket failed.\n");
        return -1;
    }

    unlink(UNIXSTR_PATH);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sun_family = AF_UNIX;
    strcpy(servaddr.sun_path, UNIXSTR_PATH);

    ret = bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if (ret < 0) 
    {
        printf("bind failed. %s", strerror(errno));
        close(listenfd);
        return -1;
    }

    listen(listenfd, 5);

    while (1) {
        clilen = sizeof(cliaddr);
        clifd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);
        if (clifd < 0)
        {
            printf("accept failed.\n");
            continue;
        }

        msg.msg_name = NULL;
        msg.msg_namelen = 0;

        iov[0].iov_base = buf;
        iov[0].iov_len = 100;
        msg.msg_iov = iov;
        msg.msg_iovlen = 1;
        msg.msg_control = control_un.control;
        msg.msg_controllen = sizeof(control_un.control);

        ret = recvmsg(clifd, &msg, 0);
        if (ret <= 0) 
        {
            printf("recvmsg failed\n");
            return ret;
        }

        // 检查是否收到合法的附属数据
        if ((pcmsg = CMSG_FIRSTHDR(&msg)) != NULL && (pcmsg->cmsg_len == CMSG_LEN(sizeof(int))))
        {
            if (pcmsg->cmsg_level != SOL_SOCKET)
            {
                printf("cmsg_level is not SOL_SOCKET.\n");
                continue;
            }

            if (pcmsg->cmsg_type != SCM_RIGHTS)
            {
                printf("cmsg_type is not SCM_RIGHTS.\n");
                continue;
            }

            // 取出接收到的描述符
            recvfd = *((int*)CMSG_DATA(pcmsg));
            printf("recv fd = %d\n", recvfd);

            write(recvfd, testmsg, strlen(testmsg));
            close(recvfd);
        }
    }
    return 0;
}

