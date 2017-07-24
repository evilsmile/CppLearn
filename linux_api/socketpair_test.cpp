#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <error.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdlib.h>

#include <string>
#include <iostream>

#define BUF_SIZE 30

#define EXIT_WITH_MSG(msg) \
    fprintf(stderr, msg": %s\n", strerror(errno)); \
    exit(-1)

int main()
{
    int s[2];
    std::string str = "test string";
    std::string buf;
    buf.resize(BUF_SIZE, '\0');

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, s) == -1) {
        EXIT_WITH_MSG("socketpair failed");        
    }

    int r = -1;
    int w = -1;
    pid_t pid;
    if ((pid = fork()) > 0) {
        std::cout << "parent pid: " << getpid() << std::endl;
        // 父进程关闭s[1]，保留s[0]
        close(s[1]);

        // 父进程写
        if ((w = write(s[0], (char*)str.c_str(), (int)str.size())) == -1) {
            EXIT_WITH_MSG("write failed.");
        }

        // 父进程读
        if ((r = read(s[0], (char*)buf.c_str(), buf.size()) == -1)) {
            EXIT_WITH_MSG("read failed.");
        }
        std::cout << "pid[" << getpid() << "] read str : " << buf << std::endl;
    } else if (pid == 0) {
        std::cout << "child created. pid: " << getpid() << std::endl;
        // 子进程关闭s[0]，保留s[1]
        close(s[0]);
    
        // 子进程读
        if ((r = read(s[1], (char*)buf.c_str(), buf.size()) == -1)) {
            EXIT_WITH_MSG("read failed.");
        }
        std::cout << "pid[" << getpid() << "] read str : " << buf << std::endl;

        // 子进程写回
        if ((w = write(s[1], (char*)str.c_str(), (int)str.size())) == -1) {
            EXIT_WITH_MSG("write failed.");
        }
    } else {
        EXIT_WITH_MSG("fork failed.");
    }

    return 0;
}
