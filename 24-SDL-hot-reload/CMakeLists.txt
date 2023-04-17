cmake_minimum_required(VERSION 3.20)
project(24-SDL-hot-reload CXX)

add_executable(24-SDL-hot-reload engine.cxx engine.hxx)
target_compile_features(24-SDL-hot-reload PUBLIC cxx_std_20)

set_target_properties(
  24-SDL-hot-reload PROPERTIES
  ENABLE_EXPORTS 1
)

find_package(SDL3 REQUIRED)

target_link_libraries(24-SDL-hot-reload PRIVATE SDL3::SDL3-shared)

add_library(game SHARED game.cxx)

target_compile_features(game PUBLIC cxx_std_20)
target_link_libraries(game PRIVATE 24-SDL-hot-reload)

