cmake_minimum_required(VERSION 3.20)
project(30-render-basic)

add_executable(00-canvas 00-canvas.hxx 00-canvas.cxx 00-canvas_main.cxx)
target_compile_features(00-canvas PUBLIC cxx_std_20)

add_executable(01-render-line 00-canvas.cxx 01-render-line.cxx 01-render-line.hxx 01-render-line_main.cxx)
target_compile_features(01-render-line PUBLIC cxx_std_20)
