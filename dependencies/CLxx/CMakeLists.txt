cmake_minimum_required(VERSION 2.6)

project(clxx)

set(CLXX_SRCS_CXX
    BufferCommands.cpp
    Buffer.cpp
    Command.cpp
    CommandQueue.cpp
    Context.cpp
    Device.cpp
    Exception.cpp
    Host.cpp
    ImageCommands.cpp
    Image.cpp
    KernelCommands.cpp
    Kernel.cpp
    Memory.cpp
    Platform.cpp
    Profile.cpp
    Program.cpp
    Sampler.cpp
    SyncCommands.cpp
    )

add_library(clxx ${CLXX_SRCS_CXX})
