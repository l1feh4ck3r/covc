#include <CL/cl.h>
#include <iostream>

#include "Includes.h"

using namespace CLxx;


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

    const int numElements = 114447;	
    const size_t localWorkSize = 256;
    const int r = numElements % localWorkSize;

    const size_t globalGroupSize = r == 0 ? numElements : numElements + localWorkSize - r;

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
	boost::shared_ptr<Context> ctx = Context::createContext(Device::GPU); 

	// 2) create a program using source code
	boost::shared_ptr<Program> prog = ctx->createProgram(code);

	// 3) compile
	if ( !prog->buildProgram("-cl-mad-enable") )
	{
        std::cout << prog->getBuildLog() << std::endl;
        return 1;
	}


    // 4) allocate memory on the device to hold the vectors data
   	boost::shared_ptr<Buffer>  vecBufferA =  ctx->createBuffer(Memory::READ_ONLY, sizeof(float) * globalGroupSize);
	boost::shared_ptr<Buffer>  vecBufferB =  ctx->createBuffer(Memory::READ_ONLY, sizeof(float) * globalGroupSize);
	boost::shared_ptr<Buffer>  verBufferR =  ctx->createBuffer(Memory::WRITE_ONLY, sizeof(float) * globalGroupSize);

	// 5) create the commands to copy the data to and from the device
	boost::shared_ptr<WriteBufferCommand> wrtBufferA( new WriteBufferCommand(vecBufferA,&vecA[0]) );
	boost::shared_ptr<WriteBufferCommand> wrtBufferB( new WriteBufferCommand(vecBufferB,&vecB[0]) );
    boost::shared_ptr<ReadBufferCommand>  readBuffer( new ReadBufferCommand (verBufferR,&vecR[0]) );

	// 6) create the kernel to use and set the arguments 
	boost::shared_ptr<Kernel> kernel = prog->createKernel("VectorAdd");
	kernel->setArguments(vecBufferA,vecBufferB,verBufferR,numElements);


    // 7) create the command to execute the kernel
    boost::shared_ptr<Range1DCommand> calculateOnDataRange( new Range1DCommand(kernel,globalGroupSize,localWorkSize) );
    
	// 8) create a command queue
	boost::shared_ptr<CommandQueue> cmdQueue = ctx->createCommandQueue();

	// 9) enque the commands
	cmdQueue->enque(wrtBufferA);  // copy vecA to device
	cmdQueue->enque(wrtBufferB);  // copy vecB to device
	cmdQueue->enque(calculateOnDataRange); // execute kernel 
	cmdQueue->enque(readBuffer); // read the results

	}
	catch(Exception ex)
	{
        std::cout << ex.what() << std::endl;
        return 1;
	}
    
    return 0;
}
