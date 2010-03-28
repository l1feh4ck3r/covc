#include "Kernel.h"
#include "Exception.h"
#include "Program.h"
#include "Device.h"
#include "Host.h"

namespace CLxx
{
    Kernel::Kernel(Handle h)
        :_handle(h)
    {
    }


    boost::shared_ptr<Kernel> Kernel::createKernel(boost::shared_ptr<Program> prog, const char* kernel_name )
    {
        cl_int errorCode;

        cl_kernel handle = clCreateKernel(prog->getHandle(), kernel_name, &errorCode );

        if ( errorCode != CL_SUCCESS )
            throw Exception(errorCode);

        boost::shared_ptr<Kernel> kernel = boost::shared_ptr<Kernel>( new Kernel(handle) );
        kernel->_program = prog;
        prog->addKernel( kernel );
        //Host::add(handle,queue);

        return kernel;
    }

    void Kernel::createKernels(boost::shared_ptr<Program> prog, std::vector<boost::shared_ptr<Kernel> >& kernels )
    {
        cl_uint numKerns;
        cl_kernel  kerns[1000];

        cl_int errorCode = clCreateKernelsInProgram(prog->getHandle(), sizeof(kerns), kerns, &numKerns );

        if ( errorCode != CL_SUCCESS )
            throw Exception(errorCode);

        for(size_t i = 0; i < numKerns; i++ )
        {
            boost::shared_ptr<Kernel> kernel = prog->getKernel(kerns[i]);

            if ( !kernel.get() )
            {
                kernel = boost::shared_ptr<Kernel>( new Kernel(kerns[i]) );
                kernel->_program = prog;
                //Host::add(kerns[i],kernel);
            }

            kernels.push_back(kernel);
        }
    }

    cl_uint Kernel::getReferenceCount() const
    {
        cl_uint value;
        cl_int ciErrNum =  clGetKernelInfo(_handle, CL_KERNEL_REFERENCE_COUNT, sizeof(cl_uint), &value, NULL);

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);

        return value;
    }

    cl_uint Kernel::getNumArgs() const
    {
        cl_uint value;
        cl_int ciErrNum =  clGetKernelInfo(_handle, CL_KERNEL_NUM_ARGS, sizeof(cl_uint), &value, NULL);

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);

        return value;
    }

    std::string Kernel::getFunctionName() const
    {
        char value[256];
        cl_int ciErrNum =  clGetKernelInfo(_handle, CL_KERNEL_FUNCTION_NAME, sizeof(value), &value, NULL);

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);

        return value;
    }

    size_t Kernel::getWorkGroupSize(boost::shared_ptr<Device> device) const
    {
        size_t value;
        cl_int ciErrNum =  clGetKernelWorkGroupInfo(_handle, device->getHandle(), CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &value, NULL);

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);

        return value;
    }

    void Kernel::getCompileWorkGroupSize(boost::shared_ptr<Device> device, size_t value[3]) const
    {
        //size_t value[3];
        cl_int ciErrNum =  clGetKernelWorkGroupInfo(_handle, device->getHandle(), CL_KERNEL_COMPILE_WORK_GROUP_SIZE, sizeof(value), &value, NULL);

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);
    }

    cl_ulong Kernel::getLocamMemSize(boost::shared_ptr<Device> device) const
    {
        cl_ulong value;
        cl_int ciErrNum =  clGetKernelWorkGroupInfo(_handle, device->getHandle(), CL_KERNEL_LOCAL_MEM_SIZE, sizeof(cl_ulong), &value, NULL);

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);

        return value;
    }

    template<>
    void Kernel::setArg(cl_uint arg_index, boost::shared_ptr<Buffer>& value)
    {
        cl_int ciErrNum =  clSetKernelArg(_handle, arg_index, sizeof(Memory::Handle), &(value->getHandle()) );

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);
    }

}
