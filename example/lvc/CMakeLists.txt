cmake_minimum_required(VERSION 2.6)

project(lvc)

include_directories("${deps_SOURCE_DIR}/matrix")

set(LVC_SRCS
    bounding_volume_calculation.cpp
    lvc.cpp
    pictureinfo.cpp
    )

add_executable(lvc ${LVC_SRCS})

target_link_libraries(lvc jpeg)

