project(helloworld_lib)

add_library(helloworld_a STATIC include/helloworld.hpp src/helloworld.cpp)
target_include_directories(helloworld_a PUBLIC include)

add_library(helloworld_so SHARED include/helloworld.hpp src/helloworld.cpp)
target_include_directories(helloworld_so PUBLIC include)


