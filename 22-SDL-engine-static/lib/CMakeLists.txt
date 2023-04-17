project(engine-lib-static)

find_package(SDL3 REQUIRED)

add_library(engine-static STATIC include/engine.hxx src/engine.cxx)
target_compile_features(engine-static PUBLIC cxx_std_20)

target_include_directories(engine-static PUBLIC include)

target_link_libraries(engine-static PRIVATE SDL3::SDL3-shared)
