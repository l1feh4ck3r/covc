# -------------------------------------------------
# Project created by QtCreator 2010-03-28T14:12:01
# -------------------------------------------------
QT -= core \
    gui
TARGET = covc
TEMPLATE = lib
CONFIG += staticlib
SOURCES += src/voxelcolorer.cpp \
    src/ocl.cpp
HEADERS += include/voxelcolorer.h \
    include/ocl.h \
    include/covc.h
OTHER_FILES += src/cl/colorer.cl
