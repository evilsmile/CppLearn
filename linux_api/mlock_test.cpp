#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <string>
#include <iostream>

const static int ONE_M = 1 << 20;
const static int ONE_G = 1 << 30;

int main()
{
    int alloc_size = ONE_G;
    char *test_p = (char*)malloc(alloc_size);
    if (test_p == NULL) {
        std::cerr << "Malloc failed." << std::endl;
        return -1;
    }

    int page_size = getpagesize();

    for (int i = 0; i < alloc_size; i += page_size) {
        *(test_p+i) = page_size;
    }

    mlock(test_p, alloc_size);

    munlock(test_p, alloc_size);

    mlockall(MCL_CURRENT | MCL_FUTURE);

    free(test_p);

    int x;
    std::cin >> x;

    return 0;
}
