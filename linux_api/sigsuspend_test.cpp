/*   此文件用于测试sigprocmask、sigsuspend函数
 *   ------------------------
 *   1. 
 *   进程执行到sigsuspend时，sigsuspend并不会立刻返回，进程处于TASK_INTERRUPTIBLE状态并立刻放弃CPU，
 *   等待UNBLOCK（mask之外的）信号的唤醒。进程在接收到UNBLOCK（mask之外）信号后，
 *   调用处理函数，然后把现在的信号集还原为原来的，sigsuspend返回，进程恢复执行。
 *   ------------------------
 *   2. 
 *   为什么sigsuspend要与sigprocmask配合使用？
 *   是为了原子操作
 *   如果之前没有调用sigprocmask()屏蔽SIGQUIT信号，那么SIGQUIT信号随时都能发生
 *   假定恰恰在判断quitflag == 0之后，信号发生，调用信号处理程序，quitflag = 1
 *   从信号处理程序返回后，开始调用sigsuspend()..
 *   如果此后没有第二个SIGQUIT信号，那么程序将一直阻塞在sigsuspend()，虽然此时quitflag = 1
 *
 *   调用sigprocmask()屏蔽SIGQUIT信号之后，即使信号发生，也将延迟递交，直到sigsuspend()解除信号屏蔽。
 *   ------------------------
 *   3.
 *   sigsuspend的原子操作是：
 *   （1）设置新的mask阻塞当前进程(上面是用wait替换new，即阻塞SIGUSR1信号)
 *   （2）收到SIGUSR1信号，阻塞，程序继续挂起；收到其他信号，恢复原先的mask(即包含SIGINT信号的)。
 *   （3）调用该进程设置的信号处理函数(程序中如果先来SIGUSR1信号，然后过来SIGINT信号，则信号处理函数会调用两次，打印不同的内容。第一次打印SIGINT,第二次打印SIGUSR1，因为SIGUSR1是前面阻塞的)
 *   （4）待信号处理函数返回，sigsuspend返回了。(sigsuspend将捕捉信号和信号处理函数集成到一起了)
 *
 *
 *
 */
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <unistd.h>
#include <signal.h>

void func(int num)
{
    printf("num: %d\n", num);
}

int main()
{

    sigset_t set;
    sigset_t empty;

    sigemptyset(&set);
    sigemptyset(&empty);

    sigaddset(&set, SIGINT);
    signal(SIGINT, func);

    while (true) {
        // 屏蔽
        sigprocmask(SIG_BLOCK, &set, NULL);
        for (int i = 0; i < 5; ++i) {
            write(1, "* ", 2);
            sleep(1);
        }

        printf("\n");

        // 调用信号处理函数再返回
        sigsuspend(&empty);

        // 放开
        sigprocmask(SIG_UNBLOCK, &set, NULL);

        pause();
    }

    return 0;
}
