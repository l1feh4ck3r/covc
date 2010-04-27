cmake_minimum_required(VERSION 2.6)

project(clxx_example)

include_directories("${clxx_SOURCE_DIR}/include")

set(CLXX_EXAMPLE_SRCS
    vector_add.cpp
    )

add_executable(clxx_example ${CLXX_EXAMPLE_SRCS})

target_link_libraries(clxx_example clxx OpenCL boost_signals)
