cmake_minimum_required(VERSION 2.6)

project(ocl_example)

include_directories("${deps_SOURCE_DIR}/ocl")

set(OCL_EXAMPLE_SRCS
    vector_add.cpp
    )

add_executable(ocl_example ${OCL_EXAMPLE_SRCS})

target_link_libraries(ocl_example OpenCL)

