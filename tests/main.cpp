

#include <iostream>
#include "Allocator.h"

#include "CrashDump.h"
#include "boost/asio.hpp"

class Knight
{
public:
    Knight() { std::cout << "ctor" << '\n'; }
    ~Knight() { std::cout << "dtor" << '\n'; }

	void print() { std::cout << "print"<<hp << '\n'; }
    int hp{ 0 };
};

int main() {
	psh::Utils::UseDump(L"./Dump");

    std::cout << "애플리케이션이 정상적으로 실행됩니다.\n";

    int* ptr = nullptr;
    *ptr = 42;  // 강제로 크래시 발생


    return 0;
}