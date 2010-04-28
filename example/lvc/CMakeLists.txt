cmake_minimum_required(VERSION 2.6)

project(lvc)

include_directories("${matrix_tcl_lite_SOURCE_DIR}/include")

set(LVC_SRCS
    lvc.cpp
    pictureinfo.cpp
    )

add_executable(lvc ${LVC_SRCS})

target_link_libraries(lvc jpeg)

