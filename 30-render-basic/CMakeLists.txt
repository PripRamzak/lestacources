cmake_minimum_required(VERSION 3.20)
project(30-render-basic)

add_executable(00-canvas 00-canvas.hxx 00-canvas.cxx 00-canvas_main.cxx)
target_compile_features(00-canvas PUBLIC cxx_std_20)

add_executable(01-render-line 00-canvas.cxx 01-render-line.cxx 01-render-line.hxx 01-render-line_main.cxx)
target_compile_features(01-render-line PUBLIC cxx_std_20)

add_executable(02-render-triangle 00-canvas.cxx 01-render-line.cxx 02-render-triangle.hxx 02-render-triangle.cxx 02-render-triangle_main.cxx)
target_compile_features(02-render-triangle PUBLIC cxx_std_20)

add_executable(03-render-triangle-indexed 00-canvas.cxx 01-render-line.cxx 02-render-triangle.cxx 03-render-triangle-indexed.hxx 03-render-triangle-indexed.cxx 03-render-triangle-indexed_main.cxx)
target_compile_features(03-render-triangle-indexed PUBLIC cxx_std_20)

add_executable(04-render-triangle-interpolated 00-canvas.cxx 01-render-line.cxx 02-render-triangle.cxx 03-render-triangle-indexed.cxx 04-render-triangle-interpolated.hxx 04-render-triangle-interpolated.cxx 04-render-triangle-interpolated_main.cxx)
target_compile_features(04-render-triangle-interpolated PUBLIC cxx_std_20)

find_package(SDL3 REQUIRED)
add_executable(05-render-program 00-canvas.cxx 01-render-line.cxx 02-render-triangle.cxx 03-render-triangle-indexed.cxx 04-render-triangle-interpolated.cxx 05-render-program.hxx 05-render-program.cxx 05-render-program_main.cxx)
target_compile_features(05-render-program PUBLIC cxx_std_20)
target_link_libraries(05-render-program PRIVATE SDL3::SDL3-shared)
