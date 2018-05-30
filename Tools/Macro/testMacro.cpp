#include <stdio.h>

// to 'x'
#define STR(x) #x

// '#' 变成字符串
#define WARN_IF(exp) \
    do { \
        if (!(exp)) {  \
            printf("WARNING: " #exp "!!!\n"); \
        } \
    } while(0)

// 没有对变参起名，使用默认的__VA_ARGS__来代替
#define myprintf(format, ...) \
    fprintf(stderr, format, __VA_ARGS__) 

// 显式命名变参为args
#define myprintf2(format, args...) \
    fprintf(stderr, format, args)

struct command {
    const char *name;
    void (*f)();
};

void quit_command()
{
}

void open_command()
{
}

// ## 用于拼接
#define COMMAND(NAME) { \
    #NAME,   \
    NAME ## _command \
}

struct command commands[] = {
    COMMAND(quit),
    COMMAND(open),
};

bool check()
{
    return false;
}

int main()
{
    printf("%s\n", STR(IT));
    WARN_IF(check());
    myprintf("what is %s\n", "love");
    myprintf2("what is %s\n", "love");

    return 0;
}
