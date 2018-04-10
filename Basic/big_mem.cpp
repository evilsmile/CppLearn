#include <stdio.h>
#include <map>
#include <iostream>
#include <malloc.h>
#include <cstring>

static const int scg = 0x1234;
static int sg = 0x5677;
int g = 0x9987;
int ug;

void pause()
{
    char c;
    std::cin >> c;
}

int test1()
{
    malloc_stats();
    {
        std::map<int, std::string> mIs, mIs2;
        char c = 'a';
        for (int i = 0; i < 1024 * 1024 * 1; ++i)
        {
            if (c == 'z')
            {
                c = 'a';
            }

            mIs[i] = std::string(&c, 1);

            ++c;
        }
        std::cout << "memory alloc\n";
        malloc_stats();
        pause();

        mIs.clear();
        std::cout << "memory cleared. Now size() " << mIs.size() << std::endl;
        mIs.swap(mIs2);
    }
    malloc_stats();
    std::cout << "Press to alloc another.\n";
    pause();
    {
        std::map<int, int> mIs, mIs2;
        for (int i = 0; i < 1024 * 1024 * 20; ++i)
        {
            mIs[i] = i;
        }
        std::cout << "memory alloc\n";
        malloc_stats();
        pause();

        mIs.clear();
        std::cout << "memory cleared. Now size() " << mIs.size() << std::endl;
        mIs.swap(mIs2);
    }
    malloc_stats();
    mallinfo();
    pause();
    pause();

    return 0;
}

int test2()
{
    int bytes = 1024 * 1024 * 800;
    malloc_stats();

    char * ptr = (char*)malloc(bytes);
    if (ptr == NULL)
    {
        return -1;
    }

    memset(ptr, 0, bytes);

    printf("alloc memory of %d bytes. p:%p &p:%p\n", bytes, ptr, &ptr);
    malloc_stats();

    pause();

    printf("enter to free memory \n");
    free(ptr);
    malloc_stats();

    pause();

    return 0;
}

int main()
{
    int large[1024 * 1024 ] = {0};

    printf("scg:%p sg:%p g:%p ug:%p\n", &scg, &sg, &g, &ug);
    test1();
    //test2();

    return 0;
}
