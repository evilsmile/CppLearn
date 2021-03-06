/* 
 * 连接服务端，传递打开文件的描述符给服务端，由服务端进行文件写
 */
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
#define OPEN_FILE "test"

int main(int argc, char* argv[])
{
    int clifd;
    struct sockaddr_un servaddr;
    int ret;
    struct msghdr msg;
    struct iovec iov[1];
    char buf[100];

    union {
        struct cmsghdr cm;
        char control[CMSG_SPACE(sizeof(int))];
    }control_un;
    struct cmsghdr *pcmsg;
    int fd;

    clifd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (clifd < 0) 
    {
        printf("socket failed\n");
        return -1;
    }
    fd = open(OPEN_FILE, O_CREAT|O_RDWR, 0777);
    if (fd < 0) 
    {
        printf("open test file failed.\n");
        return -2;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sun_family = AF_UNIX;
    strcpy(servaddr.sun_path, UNIXSTR_PATH);

    ret = connect(clifd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if (ret < 0)
    {
        printf("connect failed.\n");
        return -3;
    }
    printf("connect with fd:%d\n", clifd);

    // UDP才要用到的
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    iov[0].iov_base = buf;
    iov[0].iov_len = 100;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    //设置缓冲区和长度
    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);

    // 通过CMSG_FIRSTHDR取得附属数据
    pcmsg = CMSG_FIRSTHDR(&msg);
    pcmsg->cmsg_len = CMSG_LEN(sizeof(int));
    pcmsg->cmsg_level = SOL_SOCKET;
    // 指明要发送的描述符
    pcmsg->cmsg_type = SCM_RIGHTS;
    // 写入要传递的描述符
    *((int*)CMSG_DATA(pcmsg)) = fd;

    ret = sendmsg(clifd, &msg, 0);
    if (ret < 0) 
    {
        printf("sendmsg failed: %s\n", strerror(errno));
        return -4;
    }
    printf("ret = %d, fd = %d\n", ret, fd);

    return 0;
}
