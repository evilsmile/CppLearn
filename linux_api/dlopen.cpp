#include <iostream>
#include <string>
#include <unistd.h>
#include <pthread.h>

#include <dlfcn.h>

static const std::string TEST_MATH_LIB = "libtest_math.so";

static void *handle = NULL;
static char *error = NULL;

typedef int (*math_cal_fn_t)(int, int);

void* do_job(void *arg)
{
    math_cal_fn_t add_fn = (math_cal_fn_t)dlsym(handle, "add");
    if ((error = dlerror()) != NULL) {
        std::cerr << "dlsym failed: " << error << std::endl;
        return NULL;
    }

    std::cout << "7+8=" << add_fn(7, 8) << std::endl;

    sleep(1);

    return NULL;
}

int main()
{
    handle = dlopen(TEST_MATH_LIB.c_str(), RTLD_LAZY);
    if (!handle) {
        std::cerr << "dlopen failed: " << dlerror() << std::endl;
        return -1;
    }

    dlerror();

    pthread_t tid;
    if (pthread_create(&tid, NULL, do_job, NULL)) {
        std::cerr << "pthread create failed." << std::endl;    
        return -1;
    }

    void *result;
    pthread_join(tid, &result);

    sleep(5);
    math_cal_fn_t sub_fn = (math_cal_fn_t)dlsym(handle, "sub");
    if ((error = dlerror()) != NULL) {
        std::cerr << "dlsym failed: " << error << std::endl;
        return NULL;
    }

    std::cout << "7-8=" << sub_fn(7, 8) << std::endl;

    dlclose(handle);

    char c;
    read(1, &c, 1);

    return 0;
}
