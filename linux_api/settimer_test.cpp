/*   此文件用于测试 setitimer() 函数
 *   #include <sys/time.h> 
 *    
 *   int setitimer(int which, const struct itimerval *value, struct itimerval *ovalue); 
 *   
 *   struct itimerval { 
 *      struct timerval it_interval; 
 *      struct timerval it_value; 
 *   
 *   }; 
 *   
 *   struct timeval { 
 *      long tv_sec; 
 *      long tv_usec; 
 *   };
 *                                                                                                                                                      
 *   当setitimer()所执行的timer时间到了，会呼叫SIGALRM signal，
 *   itimerval.it_value设定第一次执行function所延迟的秒数，
 *   itimerval.it_interval设定以后每几秒执行function，
 *   所以若只想延迟一段时间执行function，只要设定 itimerval.it_value即可，
 *   若要设定间格一段时间就执行function，则it_value和it_interval都要设定，
 *   否则 funtion的第一次无法执行，就别说以后的间隔执行了。
 *   ITIMER_REAL，表示以real-time方式减少timer，在timeout时会送出SIGALRM signal。
 *
 */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

void doprint(int num)
{
    printf("signal: %d\n", num);
}

int main()
{
    int res = 0;

    signal(SIGALRM, doprint);

    struct itimerval tick;

    memset(&tick, 0, sizeof(struct itimerval));

    tick.it_value.tv_sec = 2;
    tick.it_value.tv_usec = 0;

    tick.it_interval.tv_sec = 1;
    tick.it_interval.tv_usec = 0;

    res = setitimer(ITIMER_REAL, &tick, NULL);
    if (res) {
        fprintf(stderr, "setitimer failed.\n");
        return -1;
    }

    // -- start ---
    // 这一块代码保证信号处理不会有遗漏，如在这之前的信号 
    sigset_t set;
    sigset_t empty;
    sigemptyset(&set);
    sigemptyset(&empty);

    sigaddset(&set, SIGALRM);
    sigprocmask(SIG_BLOCK, &set, NULL);

    while (true) {
        sigsuspend(&empty);
    }

    return 0;
}
