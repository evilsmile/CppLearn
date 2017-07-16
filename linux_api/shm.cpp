#include <iostream>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct data {
    char num[100];
}data;

static const char *path = ".ian_shm";
static int g_shm_id = -1;
static data* p_data = NULL;

#define log_error(head,  msg) \
        std::cerr << "ERROR:[" << __FUNCTION__ << ":" << __LINE__ << "] => " << head << msg << std::endl

#define log_info(head, msg) \
        std::cout << __FUNCTION__ << __LINE__ << head << msg << std::endl

int create_shm()
{
    key_t shm_key = ftok(path, 1);

    if (shm_key < 0) {
        log_error("get shm key failed: ", strerror(errno));
        return -1;
    }

    g_shm_id = shmget(shm_key, sizeof(data), IPC_CREAT | 0666);
    if (g_shm_id < 0) {
        log_error("create shm failed: ", strerror(errno));
        return -1;
    }
    std::cout << "get shm id: " << g_shm_id << std::endl;

    return 0;
}

int test_shm()
{
    struct shmid_ds shm_info;
    if (shmctl(g_shm_id, IPC_STAT, &shm_info)) {
        log_error("shmctl failed:", strerror(errno));
        return -1;
    }

    return 0;
}

int write_shm()
{
    p_data = static_cast<data*>(shmat(g_shm_id, NULL, 0));
    if (reinterpret_cast<void*>(p_data) == reinterpret_cast<void*>(-1)) {
        log_error("attach shm failed: ", strerror(errno));
        return -1;
    }
    std::cout << "attach success" << std::endl;

    memset(p_data, 0x30, sizeof(data));

    sleep(2);
    if (shmdt(p_data) < 0) {
        log_error("detach failed", strerror(errno));
        return -1;
    }
    return 0;
}

int do_child()
{
    p_data = static_cast<data*>(shmat(g_shm_id, NULL, 0));
    if (reinterpret_cast<void*>(p_data) == reinterpret_cast<void*>(-1)) {
        log_error("child attach shm failed: ", strerror(errno));
        return -1;
    }
    std::cout << "child attach success" << std::endl;

    std::cout << "=== Read shared memory ===" << std::endl;
    for (int i = 0; i < sizeof(data); ++i) {
        std::cout << p_data->num[i];
    }
    std::cout << std::endl;

    sleep(2);
    if (shmdt(p_data) < 0) {
        log_error("detach failed", strerror(errno));
        return -1;
    }
    return 0;
}

int rm_shm()
{
    struct shmid_ds shm_info;
    if (shmctl(g_shm_id, IPC_RMID, &shm_info)) {
        log_error("shmctl failed:", strerror(errno));
        return -1;
    }

    return 0;
}

int main(int argc, char* argv[])
{
    int ret = 0;

    ret = create_shm();
    if (ret < 0) {
        return -1;
    }

    ret = test_shm();
    if (ret < 0) {
        return -1;
    }

    pid_t chld_pid =  fork();
    if (chld_pid == 0) {
        do_child();
        return 0;
    }

    ret = write_shm();
    if (ret < 0) {
        return -1;
    }

    int status;
    wait(&status);

    sleep(3);
    ret = rm_shm();
    if (ret < 0) {
        return -1;
    }
	return 0;
}
