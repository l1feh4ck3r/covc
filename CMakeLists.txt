#project name
project (covc)

#cmake minimum version
cmake_minimum_required(VERSION 2.6)

option(BUILD_EXAMPLES "Build all examples." ON)

add_subdirectory(covc)

add_subdirectory(dependencies)

if (BUILD_EXAMPLES)
    add_subdirectory(example)
endif (BUILD_EXAMPLES)

add_subdirectory(tools)
