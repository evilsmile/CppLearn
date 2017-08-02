#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <signal.h>

#include <iostream>
#include <string>

#define EXIT_MSG(msg) \
    std::cerr << msg << std::endl; \
    exit(-1)

pthread_t t_id;
pthread_t t_id2;
static bool stop = false;

void stop_thread(int arg)
{
    stop = true;
}

void* func(void *arg)
{
    sigset_t sigset;
    sigemptyset(&sigset);
    // 子线程接收处理INT信号 
    sigaddset(&sigset, SIGTERM);
    sigaddset(&sigset, SIGINT);
    pthread_sigmask(SIG_UNBLOCK, &sigset, NULL);

    signal(SIGINT, stop_thread);
    signal(SIGTERM, stop_thread);

    while (!stop) {
        std::cout << "thread [" << t_id << "] running ..  " << std::endl;

        sleep(1);
    }

    return NULL;
}

void* func2(void *arg)
{
#define TEST 2
    std::cout << "thread_id [" << t_id2 << "] ..." << std::endl;
    sleep(3);

#if TEST == 1
    if (pthread_cancel(t_id)) {
        EXIT_MSG("pthread_cancel");    
    }
    std::cout << "cancel " << t_id << std::endl;

#elif TEST == 2
    if (pthread_kill(t_id, SIGINT)) {
        EXIT_MSG("pthread_cancel");    
    }
    std::cout << "kill [" << t_id << "]" << std::endl;
#endif

    return NULL;
}

int main(int argc, char* argv[])
{
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGTERM);
    sigaddset(&sigset, SIGINT);
    // 主线程不接收处理INT信号
    pthread_sigmask(SIG_BLOCK, &sigset, NULL);

    if (pthread_create(&t_id, NULL, func, NULL)) {
        EXIT_MSG("pthread_create");
    }
    if (t_id < 0) {
        EXIT_MSG("invalid thread_id");
    }

    if (pthread_create(&t_id2, NULL, func2, NULL)) {
        EXIT_MSG("pthread_create");
    }
    if (t_id2 < 0) {
        EXIT_MSG("invalid thread_id");
    }

    void* ret;
    if (pthread_join(t_id, (void**)&ret)) {
        EXIT_MSG("pthread_join");
    }

    void* ret2;
    if (pthread_join(t_id2, (void**)&ret2)) {
        EXIT_MSG("pthread_join");
    }

    std::cout << "thread exit status: thread1[" << ret << "] thread2[" << ret2 << "]" << std::endl;

    return 0;
}
