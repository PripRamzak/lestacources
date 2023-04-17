cmake_minimum_required(VERSION 3.20)
project(22-SDL-engine-static CXX)

add_subdirectory(lib)

add_executable(22-SDL-engine-static src/game.cxx)
target_compile_features(22-SDL-engine-static PUBLIC cxx_std_20)

target_link_libraries(22-SDL-engine-static PRIVATE engine)
