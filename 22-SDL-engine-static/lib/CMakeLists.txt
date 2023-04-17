project(engine_lib)

find_package(SDL3 REQUIRED)

add_library(engine STATIC include/engine.hxx src/engine.cxx)
target_compile_features(engine PUBLIC cxx_std_20)

target_include_directories(engine PUBLIC include)

target_link_libraries(engine PRIVATE SDL3::SDL3-static)
