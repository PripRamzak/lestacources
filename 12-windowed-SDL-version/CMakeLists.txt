cmake_minimum_required(VERSION 3.20)

project(12-windowed-SDL-version)

add_executable(12-windowed-SDL-version main.cxx)
target_compile_features(12-windowed-SDL-version PUBLIC cxx_std_20)

find_package(SDL3 REQUIRED)
find_package(SDL3_ttf REQUIRED)

target_link_libraries(12-windowed-SDL-version PRIVATE SDL3::SDL3-shared)
target_link_libraries(12-windowed-SDL-version PRIVATE SDL3_ttf::SDL3_ttf)


