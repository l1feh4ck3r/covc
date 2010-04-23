cmake_minimum_required(VERSION 2.6)

project(lvc)

set(LVC_SRCS
    lvc.cpp
    )

add_executable(lvc ${LVC_SRCS})

target_link_libraries(lvc jpeglib)

