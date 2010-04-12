#project name
project (covc)

#cmake minimum version
cmake_minimum_required(VERSION 2.6)

option(BUILD_EXAMPLES "Build all examples." ON)

add_subdirectory(covc)

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif (BUILD_EXAMPLES)

add_subdirectory(dependencies)

add_subdirectory(tools)
