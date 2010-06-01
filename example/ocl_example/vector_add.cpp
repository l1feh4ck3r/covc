#define __CL_ENABLE_EXCEPTIONS

#include "cl.hpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>

const char* code = 
        "__kernel void VectorAdd(__global const float* a, __global const float* b, __global float* c, int iNumElements)"
        "{"
        "    int iGID = get_global_id(0);"           // get index into global data array
        "    if (iGID >= iNumElements) { return; }"  // bound check (equivalent to the limit on a 'for' loop for standard/serial C code
        "    c[iGID] = a[iGID] + b[iGID];"           // add the vector elements
        "}";



int main(int argc, char * argv[])
{
    try
    {
        // init data

        const int numElements = 114432;
        const size_t localWorkSize = 256;

        const size_t globalGroupSize = numElements;

        std::vector<float> vecA(globalGroupSize);
        std::vector<float> vecB(globalGroupSize);
        std::vector<float> vecR(globalGroupSize);  // result

        const float fScale = 1.0f / (float)RAND_MAX;
        for (unsigned i = 0; i < globalGroupSize; ++i)
        {
            vecA[i] = fScale * rand();
            vecB[i] = fScale * rand();
        }


        // Steps :

        // 1) create context
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        cl_context_properties context_properties[] = {
            CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platforms[0]()),
            0
        };
        cl::Context ctx(CL_DEVICE_TYPE_GPU, &context_properties[0]);

        // 2) get list of devices
        std::vector<cl::Device> devices = ctx.getInfo<CL_CONTEXT_DEVICES>();

        // 3) create a program using source code
        cl::Program::Sources source(1, std::make_pair(code, strlen(code)));
        cl::Program prog = cl::Program(ctx, source);

        // 4) compile
        prog.build(devices, "-cl-mad-enable");

        // 5) allocate memory on the device to hold the vectors data
        cl::Buffer vecBufferA(ctx, CL_MEM_READ_ONLY, sizeof(float)*globalGroupSize);
        cl::Buffer vecBufferB(ctx, CL_MEM_READ_ONLY, sizeof(float)*globalGroupSize);
        cl::Buffer vecBufferR(ctx, CL_MEM_WRITE_ONLY, sizeof(float)*globalGroupSize);

        // 6) create the kernel to use and set the arguments
        cl::Kernel kernel(prog, "VectorAdd");
        kernel.setArg(0, vecBufferA);
        kernel.setArg(1, vecBufferB);
        kernel.setArg(2, vecBufferR);
        kernel.setArg(3, numElements);

        // 7) create a command queue
        cl::CommandQueue cmdQueue(ctx, devices[0], 0);

        // 8) enque the commands
        cmdQueue.enqueueWriteBuffer(vecBufferA, true, 0, sizeof(float)*globalGroupSize, &vecA[0]);
        cmdQueue.enqueueWriteBuffer(vecBufferB, true, 0, sizeof(float)*globalGroupSize, &vecB[0]);

        // 9) create the command to execute the kernel
        cl::KernelFunctor func = kernel.bind(
                cmdQueue,
                cl::NDRange(globalGroupSize),
                cl::NDRange(localWorkSize));

        func().wait();

        cmdQueue.enqueueReadBuffer(vecBufferR, true, 0, sizeof(float)*globalGroupSize, &vecR[0]);
    }
    catch(cl::Error ex)
    {
        std::cerr << "ERROR:" << ex.what() << "(" << ex.error_code() << ": " << ex.error() << ")" << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
