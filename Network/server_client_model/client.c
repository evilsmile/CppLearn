#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>

int main()
{
    int opt_val;
    socklen_t opt_len;
    struct sockaddr_in serv_addr;
    int open = 1;

    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8888);
    inet_pton(PF_INET, "192.168.206.130", &serv_addr.sin_addr.s_addr);
    //inet_pton(AF_INET, "localhost", &serv_addr.sin_addr.s_addr);

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

    /*
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
    if (setsockopt(sk, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger)) < 0) {
        close(sk);
        perror("set sock opt error");
        return -1;
    }
    */



    if (connect(sk, (const struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        close(sk);
        perror("connect socket failed");
        return -1;
    }
    int ret = write(sk, "hello", 5);
    printf("write %d bytes\n", ret);

    sleep(880);

    /*
    char buf[1024];
    int nread = read(sk, buf, 1024);
    while (nread > 0) {
        nread = read(sk, buf, 1024);
        printf("read data %d\n", nread);
    }
    printf("read result: %d\n", nread);
    perror("read");
    */
    close(sk);

    return 0;
}
