cmake_minimum_required(VERSION 3.10)

project(52-matrices)

add_library(engine-52 SHARED engine.cxx engine.hxx glad/glad.c glad/glad.h KHR/khrplatform.h stb_image.h)
target_compile_features(engine-52 PUBLIC cxx_std_20)
target_include_directories(engine-52 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(engine-52 PRIVATE SDL3::SDL3-shared)

add_executable(game-52 game.cxx)
target_compile_features(game-52 PUBLIC cxx_std_20)
target_link_libraries(game-52 PRIVATE engine-52)
