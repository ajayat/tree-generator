# CMakeList.txt : projet CMake pour GenTree
cmake_minimum_required (VERSION 3.8)

project ("TreeGenerator")

file(GLOB_RECURSE sources 
                  src/*
                  libs/*)

file(COPY src/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(TreeGenerator
               ${sources})

target_compile_features(TreeGenerator PUBLIC cxx_std_20)

include_directories(libs/jsoncpp)
