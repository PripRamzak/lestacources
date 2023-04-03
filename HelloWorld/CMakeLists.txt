project(HelloWorld LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(HelloWorld main.cpp)

install(TARGETS HelloWorld
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
