# -------------------------------------------------
# Project created by QtCreator 2010-01-31T17:18:40
# -------------------------------------------------
QT += opengl
TARGET = covc
TEMPLATE = app
SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/imagepreview.cpp \
    src/imagescene.cpp \
    src/imageinfo.cpp \
    src/CLxx/SyncCommands.cpp \
    src/CLxx/Sampler.cpp \
    src/CLxx/Program.cpp \
    src/CLxx/Profile.cpp \
    src/CLxx/Platform.cpp \
    src/CLxx/Memory.cpp \
    src/CLxx/Kernel.cpp \
    src/CLxx/KernelCommands.cpp \
    src/CLxx/Image.cpp \
    src/CLxx/ImageCommands.cpp \
    src/CLxx/Host.cpp \
    src/CLxx/Exception.cpp \
    src/CLxx/Device.cpp \
    src/CLxx/Context.cpp \
    src/CLxx/CommandQueue.cpp \
    src/CLxx/Command.cpp \
    src/CLxx/Buffer.cpp \
    src/CLxx/BufferCommands.cpp \
    src/colorer.cpp
HEADERS += src/mainwindow.h \
    src/matrix/include/matrix.h \
    src/imagepreview.h \
    src/imagescene.h \
    src/imageinfo.h \
    src/CLxx/SyncCommands.h \
    src/CLxx/Sampler.h \
    src/CLxx/Program.h \
    src/CLxx/Profile.h \
    src/CLxx/Platform.h \
    src/CLxx/Memory.h \
    src/CLxx/Kernel.h \
    src/CLxx/KernelCommands.h \
    src/CLxx/Includes.h \
    src/CLxx/Image.h \
    src/CLxx/ImageCommands.h \
    src/CLxx/Host.h \
    src/CLxx/fwd.h \
    src/CLxx/Exception.h \
    src/CLxx/Device.h \
    src/CLxx/Context.h \
    src/CLxx/CommandQueue.h \
    src/CLxx/Command.h \
    src/CLxx/Buffer.h \
    src/CLxx/BufferCommands.h \
    src/colorer.h
unix:LIBS += -lOpenCL \
    -lboost_signals

# win32:LIBS += c:/mylibs/math.lib
FORMS += src/mainwindow.ui
OTHER_FILES += .gitignore \
    TODO \
    README \
    src/colorer.cl \
    LICENSE \
    LICENCE_RUS
