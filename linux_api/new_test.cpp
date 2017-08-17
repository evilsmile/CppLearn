#include <iostream>
#include <string>
#include <new>

#include <stdio.h>

class A{
    public:
        A() { std::cout << "+A" << std::endl; }
        ~A() { std::cout << "~A" << std::endl; }
};

int main()
{
    char *p = NULL;
    // plain new
    // 标准C++ plain new失败后抛出标准异常std::bad_alloc而非返回NULL，
    // 因此检查返回值是否为NULL判断分配是否成功是徒劳的。
    try {
        p = new char[888888888888888];
    } catch(const std::bad_alloc &ex) {
        fprintf(stderr, "plain new *p failed. [%s] \n", ex.what());
    }

    // nothrow new
    // nothrow new/delete不抛出异常的运算符new的形式，new失败时返回NULL。
    try {
        p = new(std::nothrow) char[888888888888888];
        if (p == NULL) {
            fprintf(stderr, "nothrow new *p failed. p is NULL\n");
        }

    } catch(...) {
        fprintf(stderr, "nothrow exception?\n");
    }

    // placement new
    // placement new/delete 主要用途是: 
    // 反复使用一块较大的动态分配成功的内存来构造不同类型的对象或者它们的数组。
    // 例如可以先申请一个足够大的字符数组，然后当需要时在它上面构造不同类型的对象或数组。
    // placement new不用担心内存分配失败，因为它根本不分配内存，它只是调用对象的构造函数
    p = new (std::nothrow) char[1024];
    A *q = new (p) A;
    printf("placement new p:%p q: %p\n", p, q);

    // 从placement中分配的不需要delete，直接调用析构
    q->A::~A();

    // 需要释放
    delete[] p;
    p = NULL;

    return 0;
}
