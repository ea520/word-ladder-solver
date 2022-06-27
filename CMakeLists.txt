cmake_minimum_required(VERSION 3.14.0)
project(proj VERSION 0.1.0)

add_executable(weaver weaver.cpp main.cpp)
set_target_properties(weaver PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})
set_target_properties(weaver PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})
set_property(TARGET weaver PROPERTY CXX_STANDARD 17)
