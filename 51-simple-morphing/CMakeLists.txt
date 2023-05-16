cmake_minimum_required(VERSION 3.10)

project (51-simple-morphing)

add_library(engine-51 SHARED engine.cxx engine.hxx glad/glad.c glad/glad.h KHR/khrplatform.h)
target_compile_features(engine-51 PUBLIC cxx_std_20)
target_include_directories(engine-51 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(engine-51 PRIVATE SDL3::SDL3-shared)

add_executable(game-51 game.cxx)
target_compile_features(game-51 PUBLIC cxx_std_20)
target_link_libraries(game-51 PRIVATE engine-51)
