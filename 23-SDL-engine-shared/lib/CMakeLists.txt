project(engine-lib-shared)

find_package(SDL3 REQUIRED)

add_library(engine-shared SHARED include/engine.hxx src/engine.cxx)
target_compile_features(engine-shared PUBLIC cxx_std_20)

target_include_directories(engine-shared PUBLIC include)

target_link_libraries(engine-shared PRIVATE SDL3::SDL3-shared)
