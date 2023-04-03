#include <iostream>
#include <helloworld.hpp>

int HelloWorld()
{
    std::cout << "Hello, World" << std::endl;
    return std::cout.good() ? EXIT_SUCCESS : EXIT_FAILURE;
}
