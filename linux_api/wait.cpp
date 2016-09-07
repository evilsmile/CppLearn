#include <iostream>

#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

void old_child_job(int job_time)
{
    pid_t pid = getpid();
    std::cout << "child " << pid << " start" << std::endl;
    sleep(job_time);
    std::cout << "child  " << pid << " end" << std::endl;
}

void new_child_job(int job_time)
{
    pid_t pid = getpid();
    std::cout << "child " << pid << " start" << std::endl;
    sleep(job_time);
    std::cout << "child  " << pid << " end" << std::endl;
}

int main()
{

    pid_t old_child_pid = fork();

    // old brother
    if (old_child_pid == 0) {
        old_child_job(8);
        return 0;
    }

    // young brother
    pid_t new_child_pid = fork();
    if (new_child_pid == 0) {
        new_child_job(8);
        return 0;
    }

    int chld_status;
    int options = 0;

    sleep(2);
    kill(old_child_pid, SIGTERM);

#if 1
    options = WEXITED;
    siginfo_t si;
    // 使用siginfo_t 得到更详细的子进程退出信息
    if (waitid(P_PID, new_child_pid, &si, options) < 0) {
        std::cerr << "waitid error:" << strerror(errno) << std::endl;
        return -1;
    }

    std::cout << "[====== exit child ========]" << std::endl
        << " pid: " << si.si_pid << std::endl
        << " uid: " << si.si_uid << std::endl
        << " signo: " << si.si_signo << std::endl
        << " sig status: " << si.si_status << std::endl
        << " sig code: " << si.si_code << std::endl
        << "[----- exit child end ----]" << std::endl
        ;

#else
//    options |= WNOHANG;
    if (waitpid(-1, &chld_status, options) < 0) {
        std::cerr << "waitpid error:" << strerror(errno) << std::endl;
        return -1;
    }

    std::cout << "parent waits child success." << std::endl;
    if (WIFEXITED(chld_status) ) {
        std::cout << "normal exit." << std::endl;
    }
    if (WIFSIGNALED(chld_status)) {
        std::cout << "signal exit." << std::endl;
    }
    if (WTERMSIG(chld_status)) {
        std::cout << "term signal exit." << std::endl;
    }

#endif

    return 0;
}
