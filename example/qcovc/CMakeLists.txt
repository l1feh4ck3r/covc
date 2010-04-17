project(qcovc)

cmake_minimum_required(VERSION 2.6)

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})

set (QCOVC_SRCS_CXX
  imageinfo.cpp
  imagepreview.cpp
  imagescene.cpp
  main.cpp
  mainwindow.cpp
)

set (QCOVC_MOC_SRCS
  imageinfo.h
  imagepreview.h
  imagescene.h
  mainwindow.h
)

set (QCOVC_UI
  mainwindow.ui
)

# build ui_XXX files from the XML-style .ui files
QT4_WRAP_UI(QCOVC_SRCS_CXX ${QCOVC_UI})
# this moc's the above variable and appends to the cxx sources
QT4_WRAP_CPP(QCOVC_SRCS_CXX ${QCOVC_MOC_SRCS})

if(UNIX)
  add_executable(qcovc ${QCOVC_SRCS_CXX})
elseif(APPLE)
  add_executable(qcovc MACOSX_BUNDLE ${QCOVC_SRCS_CXX})
elseif(WIN32)
  add_executable(qcovc WIN32 ${QCOVC_SRCS_CXX})
endif()

target_link_libraries(qcovc ${QT_LIBRARIES})
