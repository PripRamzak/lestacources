cmake_minimum_required(VERSION 3.10)

project(53-move-knight)

add_library(engine-53 SHARED
        engine.cxx
        engine.hxx
        event.cxx
        event.hxx
        geometry.cxx
        geometry.hxx
        glad/glad.c
        glad/glad.h
        KHR/khrplatform.h
        stb_image.h shader_program.cxx shader_program.hxx)
target_compile_features(engine-53 PUBLIC cxx_std_20)
target_include_directories(engine-53 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(engine-53 PRIVATE SDL3::SDL3-shared)

add_executable(game-53 game.cxx)
target_compile_features(game-53 PUBLIC cxx_std_20)
target_link_libraries(game-53 PRIVATE engine-53)
