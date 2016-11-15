#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

class A
{
public:
    bool operator()(int i) const { return i > 50; }
};

int main(int argc, char* argv[])
{
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(90);
    vec.push_back(80);

//    auto it = find_if(vec.begin(), vec.end(), [](int i) { return i > 50; });
  //  auto it = find_if(vec.begin(), vec.end(), A());
    auto it = find_if(vec.begin(), vec.end(), [=](int i) {
            return i > 10 && i < 100;
            });

    std::cout << *it << std::endl;

	return 0;
}

