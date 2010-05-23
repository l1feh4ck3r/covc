cmake_minimum_required(VERSION 2.6)

project(covclib)

include_directories("include"
    "${clxx_SOURCE_DIR}/include"
    "${matrix_tcl_lite_SOURCE_DIR}/include"
    )

set(COVC_LIB_SRCS_CXX
    src/voxelcolorer.cpp
    )

add_library(covclib ${COVC_LIB_SRCS_CXX})
