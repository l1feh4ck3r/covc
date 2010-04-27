#include "Exception.h"
#include <string>
#include <sstream>

namespace CLxx
{
    const char* Exception::contextErrorCodes( cl_int errorCode )
    {
        switch( errorCode )
        {
        case CL_BUILD_PROGRAM_FAILURE : return "BUILD PROGRAM FAILURE";
        case CL_COMPILER_NOT_AVAILABLE : return "COMPILER NOT AVAILABLE";
        case CL_DEVICE_NOT_AVAILABLE : return "DEVICE NOT AVAILABLE";
        case CL_DEVICE_NOT_FOUND : return "DEVICE NOT FOUND";
        case CL_IMAGE_FORMAT_MISMATCH : return "IMAGE FORMAT MISMATCH";
        case CL_IMAGE_FORMAT_NOT_SUPPORTED : return "IMAGE FORMAT NOT SUPPORTED";
        case CL_INVALID_ARG_INDEX : return "INVALID ARG INDEX";
        case CL_INVALID_ARG_SIZE : return "INVALID ARG SIZE";
        case CL_INVALID_ARG_VALUE : return "INVALID ARG VALUE";
        case CL_INVALID_BINARY : return "INVALID BINARY";
        case CL_INVALID_BUFFER_SIZE : return "INVALID BUFFER SIZE";
        case CL_INVALID_BUILD_OPTIONS : return "INVALID BUILD OPTIONS";
        case CL_INVALID_COMMAND_QUEUE : return "INVALID COMMAND QUEUE";
        case CL_INVALID_CONTEXT : return "INVALID CONTEXT";
        case CL_INVALID_DEVICE   : return "INVALID DEVICE";
        case CL_INVALID_DEVICE_TYPE : return "INVALID DEVICE TYPE";
        case CL_INVALID_EVENT : return "INVALID EVENT";
        case CL_INVALID_EVENT_WAIT_LIST : return "INVALID EVENT WAIT LIST";
        case CL_INVALID_GL_OBJECT : return "INVALID GL OBJECT";
        case CL_INVALID_GLOBAL_OFFSET : return "INVALID GLOBAL OFFSET";
        case CL_INVALID_HOST_PTR : return "INVALID HOST PTR";
        case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR : return "INVALID IMAGE FORMAT DESCRIPTOR";
        case CL_INVALID_IMAGE_SIZE : return "INVALID IMAGE SIZE";
        case CL_INVALID_KERNEL_NAME : return "INVALID KERNEL NAME";
        case CL_INVALID_KERNEL : return "INVALID KERNEL";
        case CL_INVALID_KERNEL_DEFINITION : return "INVALID KERNEL DEFINITION";
        case CL_INVALID_KERNEL_ARGS : return "INVALID KERNEL ARGS";
        case CL_INVALID_MEM_OBJECT : return "INVALID MEM OBJECT";
        case CL_INVALID_OPERATION : return "INVALID OPERATION";
        case CL_INVALID_PLATFORM : return "INVALID PLATFORM";
        case CL_INVALID_PROGRAM : return "INVALID PROGRAM";
        case CL_INVALID_PROGRAM_EXECUTABLE : return "INVALID PROGRAM EXECUTABLE";
        case CL_INVALID_QUEUE_PROPERTIES : return "INVALID QUEUE PROPERTIES";
        case CL_INVALID_SAMPLER : return "INVALID SAMPLER";
        case CL_INVALID_VALUE : return "INVALID VALUE";
        case CL_INVALID_WORK_DIMENSION : return "INVALID WORK DIMENSION";
        case CL_INVALID_WORK_GROUP_SIZE : return "INVALID WORK GROUP SIZE";
        case CL_INVALID_WORK_ITEM_SIZE : return "INVALID WORK ITEM SIZE";
        case CL_MAP_FAILURE : return "MAP FAILURE";
        case CL_MEM_OBJECT_ALLOCATION_FAILURE : return "MEM OBJECT ALLOCATION FAILURE";
        case CL_MEM_COPY_OVERLAP : return "MEM COPY OVERLAP";
        case CL_OUT_OF_HOST_MEMORY : return "OUT OF HOST MEMORY";
        case CL_OUT_OF_RESOURCES : return "OUT OF RESOURCES";
        case CL_PROFILING_INFO_NOT_AVAILABLE : return "PROFILING INFO NOT AVAILABLE";

        default :
                std::stringstream ss;
        ss << "ERROR not recognized - code : " << errorCode;
        return ss.str().c_str();
    }
    }
}
