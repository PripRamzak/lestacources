cmake_minimum_required(VERSION 3.10)

project (41-opengl)

add_library(engine-41 SHARED engine.cxx engine.hxx glad/glad.c glad/glad.h KHR/khrplatform.h)
target_compile_features(engine-41 PUBLIC cxx_std_20)
target_include_directories(engine-41 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(engine-41 PRIVATE SDL3::SDL3-shared)

add_executable(game-41 game.cxx)
target_compile_features(game-41 PUBLIC cxx_std_20)
target_link_libraries(game-41 PRIVATE engine-41)
