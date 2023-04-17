cmake_minimum_required(VERSION 3.20)
project(23-SDL-engine-shared CXX)

add_subdirectory(lib)

add_executable(23-SDL-engine-shared src/main.cxx)
target_compile_features(23-SDL-engine-shared PUBLIC cxx_std_20)

target_link_libraries(23-SDL-engine-shared PRIVATE engine-shared)
