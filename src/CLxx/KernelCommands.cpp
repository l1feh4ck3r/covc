#include "KernelCommands.h"
#include "CommandQueue.h"
#include "Context.h"
#include "Kernel.h"
#include "Exception.h"

namespace CLxx
{
    KernelCommand::KernelCommand(boost::shared_ptr<Kernel>& kernel)
    {
        _kernel = kernel;
    }

    Range1DCommand::Range1DCommand(boost::shared_ptr<Kernel>& kernel, size_t size1D, size_t local_work_size)
        :RangeNDCommand<1>(kernel)
    {
        _globalWorkSize[0] = size1D;
        _localWorkSize = local_work_size;
    }

    void Range1DCommand::executePimp()
    {
        cl_int ciErrNum;

        size_t* local_work_size = (_localWorkSize > 0)  ?  &_localWorkSize : NULL;

        if ( _waitingListHandles.empty() )
            ciErrNum =  clEnqueueNDRangeKernel(_cmdQueue->getHandle(),
                                               _kernel->getHandle(),
                                               1,
                                               NULL,
                                               _globalWorkSize,
                                               local_work_size,
                                               0,
                                               NULL,
                                               &_handle );
        else
            ciErrNum =  clEnqueueNDRangeKernel(_cmdQueue->getHandle(),
                                               _kernel->getHandle(),
                                               1,
                                               NULL,
                                               _globalWorkSize,
                                               local_work_size,
                                               _waitingListHandles.size(),
                                               &_waitingListHandles[0],
                                               &_handle );

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);

        _enquedSuccessfully = true;
    }


    Range2DCommand::Range2DCommand(boost::shared_ptr<Kernel>& kernel, size_t size1D, size_t size2D, size_t local_work_size)
        :RangeNDCommand<2>(kernel)
    {
        _globalWorkSize[0] = size1D;
        _globalWorkSize[1] = size2D;
        _localWorkSize = local_work_size;
    }

    void Range2DCommand::executePimp()
    {
        cl_int ciErrNum;

        const size_t* local_work_size = (_localWorkSize > 0)  ?  &_localWorkSize : NULL;

        if ( _waitingListHandles.empty() )
            ciErrNum =  clEnqueueNDRangeKernel(_cmdQueue->getHandle(),
                                               _kernel->getHandle(),
                                               2,
                                               NULL,
                                               _globalWorkSize,
                                               local_work_size,
                                               0,
                                               NULL,
                                               &_handle );
        else
            ciErrNum =  clEnqueueNDRangeKernel(_cmdQueue->getHandle(),
                                               _kernel->getHandle(),
                                               2,
                                               NULL,
                                               _globalWorkSize,
                                               local_work_size,
                                               _waitingListHandles.size(),
                                               &_waitingListHandles[0],
                                               &_handle );

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);

        _enquedSuccessfully = true;
    }

    Range3DCommand::Range3DCommand(boost::shared_ptr<Kernel>& kernel, size_t size1D, size_t size2D, size_t size3D, size_t local_work_size)
        :RangeNDCommand<3>(kernel)
    {
        _globalWorkSize[0] = size1D;
        _globalWorkSize[1] = size2D;
        _globalWorkSize[2] = size3D;
        _localWorkSize = local_work_size;
    }

    void Range3DCommand::executePimp()
    {
        cl_int ciErrNum;

        const size_t* local_work_size = (_localWorkSize > 0)  ?  &_localWorkSize : NULL;

        if ( _waitingListHandles.empty() )
            ciErrNum =  clEnqueueNDRangeKernel(_cmdQueue->getHandle(),
                                               _kernel->getHandle(),
                                               3,
                                               NULL,
                                               _globalWorkSize,
                                               local_work_size,
                                               0,
                                               NULL,
                                               &_handle );
        else
            ciErrNum =  clEnqueueNDRangeKernel(_cmdQueue->getHandle(),
                                               _kernel->getHandle(),
                                               3,
                                               NULL,
                                               _globalWorkSize,
                                               local_work_size,
                                               _waitingListHandles.size(),
                                               &_waitingListHandles[0],
                                               &_handle );

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);

        _enquedSuccessfully = true;
    }

    void TaskCommand::executePimp()
    {
        cl_int ciErrNum;

        if ( _waitingListHandles.empty() )
            ciErrNum =  clEnqueueTask(_cmdQueue->getHandle(),
                                      _kernel->getHandle(),
                                      0,
                                      NULL,
                                      &_handle );
        else
            ciErrNum =  clEnqueueTask(_cmdQueue->getHandle(),
                                      _kernel->getHandle(),
                                      _waitingListHandles.size(),
                                      &_waitingListHandles[0],
                                      &_handle );

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);

        _enquedSuccessfully = true;
    }

}
