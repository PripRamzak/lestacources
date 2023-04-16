cmake_minimum_required(VERSION 3.20)

project(21-SDL-loop)

add_executable(21-SDL-loop main.cxx)
target_compile_features(21-SDL-loop PUBLIC cxx_std_17)

find_package(SDL3 REQUIRED)

target_link_libraries(21-SDL-loop PRIVATE SDL3::SDL3-shared)
