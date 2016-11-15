#include <iostream>
#include <functional>
using namespace std;

std::function<size_t (const char*) > print_func;

size_t print(const char* name) { std::cout << "Say Hi:" << name << std::endl; }

class CxxPrint {
public:
    size_t operator()(const char*name) {
        std::cout << "Say Cxx Hi: " << name << std::endl;
        return 0;
    }
};

int main(int argc, char* argv[])
{
    print_func = print;
    print_func("Curry");

    CxxPrint cp;
    print_func = cp;
    print_func("Curry");
	return 0;
}

