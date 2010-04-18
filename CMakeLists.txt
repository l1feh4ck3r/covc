#project name
project (covc)

#cmake minimum version
cmake_minimum_required(VERSION 2.6)

option(BUILD_EXAMPLES "Build all examples." ON)

add_subdirectory(dependencies)

add_subdirectory(covc)

if (BUILD_EXAMPLES)
    add_subdirectory(example)
endif (BUILD_EXAMPLES)

add_subdirectory(tools)
