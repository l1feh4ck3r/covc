cmake_minimum_required(VERSION 2.6)

project(clxx)

include_directories(include/)

set(CLXX_SRCS_CXX
    src/BufferCommands.cpp
    src/Buffer.cpp
    src/Command.cpp
    src/CommandQueue.cpp
    src/Context.cpp
    src/Device.cpp
    src/Exception.cpp
    src/Host.cpp
    src/ImageCommands.cpp
    src/Image.cpp
    src/KernelCommands.cpp
    src/Kernel.cpp
    src/Memory.cpp
    src/Platform.cpp
    src/Profile.cpp
    src/Program.cpp
    src/Sampler.cpp
    src/SyncCommands.cpp
    )

add_library(clxx ${CLXX_SRCS_CXX})
