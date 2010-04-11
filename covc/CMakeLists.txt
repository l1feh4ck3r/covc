PROJECT(COVCLIB)

SET (COVC_LIB_SRCS_CXX
    src/ocl.cpp
    src/voxelcolorer.cpp
    )

ADD_LIBRARY (covclib ${COVC_LIB_SRCS_CXX})

