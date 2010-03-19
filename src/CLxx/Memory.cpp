#include "Memory.h"
#include "Exception.h"

namespace CLxx
{
    Memory::Memory(Handle h)
        :_handle(h)
    {
    }


    size_t Memory::getMemSize() const
    {
        size_t value;
        cl_int ciErrNum =  clGetMemObjectInfo(_handle, CL_MEM_SIZE, sizeof(size_t), &value, NULL);

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);

        return value;
    }

    void* Memory::getMemHostPointer() const
    {
        void* value;
        cl_int ciErrNum =  clGetMemObjectInfo(_handle, CL_MEM_HOST_PTR, sizeof(void*), &value, NULL);

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);

        return value;
    }

    cl_uint Memory::getMapCount() const
    {
        cl_uint value;
        cl_int ciErrNum =  clGetMemObjectInfo(_handle, CL_MEM_MAP_COUNT, sizeof(cl_uint), &value, NULL);

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);

        return value;
    }

    cl_uint Memory::getReferenceCount() const
    {
        cl_uint value;
        cl_int ciErrNum =  clGetMemObjectInfo(_handle, CL_MEM_REFERENCE_COUNT, sizeof(cl_uint), &value, NULL);

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);

        return value;
    }

}
