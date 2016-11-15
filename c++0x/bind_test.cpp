#include <iostream>
#include <functional>

int print_sum(int x, int y) 
{
    std::cout << "result: " <<  x + y << std::endl;

    return 0;
}

class A {
public:
    int print_sum(int x, int y){
        std::cout << "A says result is: " << x + y << std::endl;
        return 0;
    }
};

int main(int argc, char* argv[])
{
    auto bf1 = std::bind(print_sum, std::placeholders::_1, 10);

    int i = 22;
    bf1(i);

    A a;
    auto bf2 = std::bind(&A::print_sum, a, std::placeholders::_1, std::placeholders::_2);
    bf2(i, i);

    std::function<int()> bf3 = std::bind(print_sum, 30, 30);
    bf3();
    
	return 0;
}
