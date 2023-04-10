cmake_minimum_required(VERSION 3.20)

project(11-SDL-version-static)
project(12-SDL-version-shared)

add_executable(11-SDL-version-static main.cxx)
target_compile_features(11-SDL-version-static PUBLIC cxx_std_20)
add_executable(12-SDL-version-shared main.cxx)
target_compile_features(11-SDL-version-static PUBLIC cxx_std_20)

find_package(SDL3 REQUIRED)

if(TARGET SDL3::SDL3-static)
    get_target_property(PATH_TO_SDL3_STATIC SDL3::SDL3-static LOCATION)
    message(STATUS "path to static libSDL3.a [${PATH_TO_SDL3_STATIC}]")
else()
    message(FATAL_ERROR "Error: find_library(...) did not find libSDL3.a")
endif()

if(TARGET SDL3::SDL3-shared)
    get_target_property(PATH_TO_SDL3_SHARED SDL3::SDL3-shared LOCATION)
    message(STATUS "path to shared libSDL3.so.0.0.0 [${PATH_TO_SDL3_SHARED}]")
else()
    message(FATAL_ERROR "Error: find_library(...) did not find libSDL3.a")
endif()

target_link_libraries(11-SDL-version-static PRIVATE SDL3::SDL3-static)
set_property(TARGET 11-SDL-version-static PROPERTY POSITION_INDEPENDENT_CODE TRUE)
target_link_libraries(12-SDL-version-shared PRIVATE SDL3::SDL3-shared)
set_property(TARGET 12-SDL-version-shared PROPERTY POSITION_INDEPENDENT_CODE TRUE)



