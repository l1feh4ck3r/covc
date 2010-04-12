cmake_minimum_required(VERSION 2.8)

#project name
project(oclc)

add_executable(oclc oclc.cpp)

target_link_libraries(oclc OpenCL)
