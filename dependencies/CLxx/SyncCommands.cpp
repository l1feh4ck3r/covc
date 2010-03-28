#include "SyncCommands.h"
#include "CommandQueue.h"
#include "Context.h"
#include "Exception.h"

namespace CLxx
{

    void MarkerCommand::executePimp()
    {
        cl_int ciErrNum =  clEnqueueMarker(_cmdQueue->getHandle(), &_handle );

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);
    }

    void WaitForCommand::toFinish(const boost::shared_ptr<Command>& cmd)
    {
        cl_int ciErrNum =  clWaitForEvents(1, &cmd->getHandle());

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);
    }

    void WaitForCommand::toFinish(const CommandWaitingList& cmd)
    {
        std::vector<Command::Handle> commands;

        for(CommandWaitingList::const_iterator it = cmd.begin(); it != cmd.end(); it++)
            commands.push_back((*it)->getHandle());

        cl_int ciErrNum =  clWaitForEvents(cmd.size(), &commands[0]);

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);

    }

    void WaitForCommand::executePimp()
    {
        cl_int ciErrNum =  clEnqueueWaitForEvents(_cmdQueue->getHandle(),
                                                  _waitingListHandles.size(),
                                                  &_waitingListHandles[0] );

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);

        _enquedSuccessfully = true;
    }

    void BarrierCommand::executePimp()
    {
        cl_int ciErrNum =  clEnqueueBarrier(_cmdQueue->getHandle());

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);

        _enquedSuccessfully = true;
    }

}
