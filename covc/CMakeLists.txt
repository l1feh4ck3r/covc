cmake_minimum_required(VERSION 2.6)

project(covclib)

include_directories("include"
    "${deps_SOURCE_DIR}/matrix"
    "${deps_SOURCE_DIR}/ocl"
    )

set(COVC_LIB_SRCS_CXX
    src/voxelcolorer.cpp
    src/matrices_and_vectors.cpp
    )

add_library(covclib STATIC ${COVC_LIB_SRCS_CXX})
