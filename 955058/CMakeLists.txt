cmake_minimum_required(VERSION 3.17)
project(955058)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(955058
        area.cpp
        area.h
        areas.cpp
        areas.h
        bethyw.cpp
        bethyw.h
        datasets.h
        input.cpp
        input.h
        lib_catch.hpp
        lib_catch_main.cpp
        lib_cxxopts.hpp
        lib_cxxopts_argv.hpp
        lib_json.hpp
        main.cpp
        measure.cpp
        measure.h)
