cmake_minimum_required(VERSION 2.6)

project(covclib)

set(COVC_LIB_SRCS_CXX
    src/ocl.cpp
    src/voxelcolorer.cpp
    )

add_library(covclib ${COVC_LIB_SRCS_CXX})

