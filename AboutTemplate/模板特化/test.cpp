#include <iostream>

template<typename T1, typename T2>
class Test 
{
public:
    Test(T1 i, T2 j) : a(i), b(j) { std::cout << "模板类" << std::endl; }
private:
    T1 a;
    T2 b;
};

template <>
class Test<int, char>
{
public:
    Test(int i, char j) : a(i), b(j) { std::cout << "全特化" << std::endl; }
private:
    int a;
    char b;
};

template <typename T2>
class Test<char, T2>
{
public:
    Test(char i, T2 j) : a(i), b(j) { std::cout << "偏特化" << std::endl; }
private:
    char a;
    T2 b;
};

template<typename T1, typename T2>
void fun(T1 a, T2 b) 
{
    std::cout << "模板函数" << std::endl;
}

template<>
void fun<int, char>(int a, char b)
{
    std::cout << "函数全特化" << std::endl;
}

/*
//模板偏特化是不允许的，因为函数重载就可以实现
template<typename T2>
void fun<char, T2>(char a, T2 b)
{
    std::cout << "函数偏特化" << std::endl;
}
*/

int main(int argc, char* argv[])
{
    Test<double, double> t1(0.1, 0.2);
    Test<int, char> t2(1, 'A');
    Test<char, bool> t3('A', true);

    fun(1, 1);
    fun(1, 'b');
//    fun('b', 3);

	return 0;
}

