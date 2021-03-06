#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>

int do_accept(int sk)
{
    while (1) {
        struct sockaddr_in cli_addr;
        socklen_t cli_len;
        int cli_fd = accept(sk, (struct sockaddr*)&cli_addr, &cli_len);

        printf("pid %d accepted.\n", getpid());

        sleep(1);

        char buffer[1024] = {0};
        int ret = -1;
        ret = read(cli_fd, buffer, sizeof(buffer));
        if (ret != 5) {
            fprintf(stderr, "read client data error, code[%d]\n", ret);
//            return -1;
        }
        printf("recv client msg: %s\n", buffer);

        int total_nwrite = 0;
        char buf[1024] = {0};
        while (total_nwrite < 100) {
            int nwrite = write(cli_fd, buf, sizeof(buf));
            if (nwrite < 0 && (errno == EBUSY || errno == EAGAIN || errno == EWOULDBLOCK)) {
                printf("busy..sleep\n");
                sleep(2);
            }
            total_nwrite += nwrite;
            printf("write %d bytes\n", total_nwrite);
        }
        printf("close fd\n");
        //close(cli_fd);
        perror("what?");
    }

    return 0;
}

int main()
{
    int opt_val;
    socklen_t opt_len;
    struct sockaddr_in serv_addr;
    int open = 1;
    int i = 0;

    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8888);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    int sk = socket(AF_INET, SOCK_STREAM, 0);
    if (sk < 0) {
        perror("create socket error");
        return -1;
    }

    /*
    if (getsockopt(sk, SOL_SOCKET, SO_DEBUG, &opt_val, &opt_len) < 0) {
        close(sk);
        perror("get sock opt error");
        return -1;
    }
    printf("debug value: %d\n", opt_val);

    if (setsockopt(sk, SOL_SOCKET, SO_DEBUG, &open, opt_len) < 0) {
        close(sk);
        perror("set sock opt error");
        return -1;
    }
    */

    if (setsockopt(sk, SOL_SOCKET, SO_REUSEADDR, &open, sizeof(int)) < 0) {
        close(sk);
        perror("set sock opt error");
        return -1;
    }

    if (setsockopt(sk, SOL_SOCKET, SO_KEEPALIVE, &open, sizeof(int)) < 0) {
        close(sk);
        perror("set sock opt error");
        return -1;
    }

    if (setsockopt(sk, SOL_SOCKET, SO_OOBINLINE, &open, sizeof(int)) < 0) {
        close(sk);
        perror("set sock opt error");
        return -1;
    }

    if (setsockopt(sk, SOL_SOCKET, SO_DONTROUTE, &open, sizeof(int)) < 0) {
        close(sk);
        perror("set sock opt error");
        return -1;
    }

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100;
    if (setsockopt(sk, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
        close(sk);
        perror("set sock opt error");
        return -1;
    }


    int nSendBuf = 1 * 1024;
    if (setsockopt(sk, SOL_SOCKET, SO_SNDBUF, (const char*)&nSendBuf, sizeof(int)) < 0) {
        close(sk);
        perror("set sock opt error");
        return -1;
    }

    int nRcvBuf = 1149;// * 1024;
    if (setsockopt(sk, SOL_SOCKET, SO_RCVBUF, (const char*)&nRcvBuf, sizeof(int)) < 0) {
        close(sk);
        perror("set sock opt error");
        return -1;
    }




    // Close immediately
    struct linger so_linger;
    so_linger.l_onoff = 1;
    so_linger.l_linger = 0;
    /*
    // 这里将导致close时发送RST包
    if (setsockopt(sk, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger)) < 0) {
        close(sk);
        perror("set sock opt error");
        return -1;
    }
    */



    if (bind(sk, (const struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        close(sk);
        perror("bind socket failed");
        return -1;
    }
    if (listen(sk, 1) < 0) {
        close(sk);
        perror("listen failed");
        return -1;
    }

    // 创建子进程监听同一个端口，实际是按顺序依次调用各个子进程的，因为它们是按顺序挂载在这个监听socket的等待队列上。
    for (i = 0; i < 4; ++i) {
        int child_id = fork();
        if (child_id == 0) {
            do_accept(sk);
            return 0;
        } 
    }

    int *result;
    wait(&result);

    return 0;
}
