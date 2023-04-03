project (helloworldwithlibs)
set (CMAKE_CXX_STANDART 20)

add_subdirectory(lib)

add_executable(helloworld_static src/main.cpp)
add_executable(helloworld_shared src/main.cpp)

target_link_libraries(helloworld_static LINK_PUBLIC helloworld_a)
target_link_libraries(helloworld_shared LINK_PUBLIC helloworld_so)
