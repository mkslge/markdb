#include <iostream>
#include "DiskManager.h"
int main() {

    const char* str1{"Mark"};
    const char* str2{"Mark"};

    if (str1 == str2) {
        std::cout << "Same address" << std::endl;
    } else {
        std::cout << "Not the same address" << std::endl;
    }

    char a{'P'};
    std::cout << &a << std::endl;







    return 0;
}