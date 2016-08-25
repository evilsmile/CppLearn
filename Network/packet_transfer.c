/*
 * 接收POS的数据并转发给联赢
 */
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>

#define BUFF_DEBUG 1

void debug_output_buff(char *buf, size_t size) 
{
    int i;
    for (i = 0; i < size; ++i) {
        printf("%02x", buf[i] &0xff);
    }
    printf("\n");
}

int send2lianyin(char *data, size_t size)
{
    int opt_val;
    socklen_t opt_len;
    struct sockaddr_in serv_addr;
    int open = 1;

    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    // LianYing的端口和IP
    serv_addr.sin_port = htons(1180);
    inet_pton(AF_INET, "123.58.32.139", &serv_addr.sin_addr.s_addr);

    int ly_sk = socket(AF_INET, SOCK_STREAM, 0);
    if (ly_sk < 0) {
        perror("create lianying socket error");
        return -1;
    }

    if (connect(ly_sk, (const struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        close(ly_sk);
        perror("connect socket failed");
        return -1;
    }

    int ret = -1;
    int pos = 0;
    while ((ret = write(ly_sk, data + pos, size - pos)) > 0) {
        pos += ret;
    }

    printf("write %d bytes to lianyin\n", pos);


    ret = -1;
    pos = 0;
    char rcv_buf[4096];

    unsigned short pkg_size = 0;
    ret = read(ly_sk, rcv_buf, 2);
    pkg_size = (rcv_buf[1] & 0xff) | (rcv_buf[0]&0xff << 4);
    pos += 2;
    printf("pkg len: %d\n", pkg_size);
    while (pos < pkg_size && (ret = read(ly_sk, rcv_buf + pos, sizeof(rcv_buf) - pos)) > 0) {
         pos += ret;
    }

    printf("received %d bytes from lianyin\n", pos);

    debug_output_buff(rcv_buf, pos);
}

int main()
{
    int opt_val;
    socklen_t opt_len;
    struct sockaddr_in serv_addr;
    int open = 1;

    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8889);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    int sk = socket(AF_INET, SOCK_STREAM, 0);
    if (sk < 0) {
        perror("create socket error");
        return -1;
    }

    if (setsockopt(sk, SOL_SOCKET, SO_REUSEADDR, &open, sizeof(int)) < 0) {
        close(sk);
        perror("set sock opt error");
        return -1;
    }

    if (bind(sk, (const struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        close(sk);
        perror("bind socket failed");
        return -1;
    }
    if (listen(sk, 3) < 0) {
        close(sk);
        perror("listen failed");
        return -1;
    }

    while (1) {
        struct sockaddr_in cli_addr;
        socklen_t cli_len;
        int cli_fd = accept(sk, (struct sockaddr*)&cli_addr, &cli_len);

        char buffer[4096] = {0};
        int pos = 0;
        int ret = -1;
        
        unsigned short pkg_size = 0;
        ret = read(cli_fd, buffer, 2);
        pkg_size = (buffer[1] & 0xff) | (buffer[0]&0xff << 4);
        pos += 2;
        printf("pkg len: %d\n", pkg_size);
        while ( pos < pkg_size && ( ret = read(cli_fd, &buffer[pos], sizeof(buffer) - pos) ) > 0) {
           pos += ret; 
        }

        printf("recv len: %d\n", pos);

#ifdef BUFF_DEBUG
        debug_output_buff(buffer, pos);
#endif

        send2lianyin(buffer, pos);
        
    }

    return 0;
}
