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
    src/ocl.cpp \
    src/imageinfo.cpp
HEADERS += src/mainwindow.h \
    src/matrix/include/matrix.h \
    src/imagepreview.h \
    src/imagescene.h \
    src/ocl.h \
    src/imageinfo.h
unix:LIBS += -lOpenCL

# win32:LIBS += c:/mylibs/math.lib
FORMS += src/mainwindow.ui
OTHER_FILES += .gitignore \
    TODO \
    README \
    src/colorer.cl
