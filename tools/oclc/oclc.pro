#-------------------------------------------------
#
# Project created by QtCreator 2010-02-14T21:04:11
#
#-------------------------------------------------

QT       -= core gui

TARGET    = oclc
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE  = app

SOURCES  += oclc.cpp

unix:LIBS += -lOpenCL
