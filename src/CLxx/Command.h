/********************************************************************************

Copyright (c) 2010 Domenico Mangieri

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

**********************************************************************************/

#ifndef COMMAND_H
#define COMMAND_H

#include "fwd.h"
#include <list>

namespace CLxx
{
    /// Base class for any cl commands. This class wraps the concept of event in cl.
    /// Command objects can be used to refer to a kernel execution command
    /// or read, write, map and copy commands on memory objects
    ///
    /// Note : If the execution of a command is terminated, the command-queue associated with this
    ///        terminated command, and the associated context (and all other command-queues in this context)
    ///        may no longer be available. The behavior of OpenCL API calls that use this context (and
    ///        command-queues associated with this context) are now considered to be implementationdefined.
    ///        The user registered callback function specified in the Context class can be used to
    ///        report appropriate error information.

    class Command
    {
    public:

        typedef cl_event Handle;

        Command( bool blocking = true );

        ~Command();

        // return if the command is blocking

        bool isBlocking() const { return _isBlocking; }

        /// CommandWaitingList specify commands that need to complete before this
        /// particular command can be executed. If CommandWaitingList is empty, then this particular command
        /// does not wait on any other command to complete.
        /// The commands specified in CommandWaitingList act as synchronization points.
        /// The context associated with commands in CommandWaitingList and CommandQueue must be the same

        typedef std::list<boost::shared_ptr<Command> > CommandWaitingList;

        void addToWaitingList( boost::shared_ptr<Command>& cmd );

        void removeFromWaitingList( boost::shared_ptr<Command>& cmd );

        const CommandWaitingList& getWaitingList() const { return _waitingList; }


        enum CommandType
        {
            NONE,
            SYNC,
            NDRANGE_KERNEL = CL_COMMAND_NDRANGE_KERNEL,
            TASK = CL_COMMAND_TASK,
            NATIVE_KERNEL = CL_COMMAND_NATIVE_KERNEL,
            READ_BUFFER = CL_COMMAND_READ_BUFFER,
            WRITE_BUFFER = CL_COMMAND_WRITE_BUFFER,
            COPY_BUFFER = CL_COMMAND_COPY_BUFFER,
            READ_IMAGE = CL_COMMAND_READ_IMAGE,
            WRITE_IMAGE = CL_COMMAND_WRITE_IMAGE,
            COPY_IMAGE = CL_COMMAND_COPY_IMAGE,
            COPY_BUFFER_TO_IMAGE = CL_COMMAND_COPY_BUFFER_TO_IMAGE,
            COPY_IMAGE_TO_BUFFER = CL_COMMAND_COPY_IMAGE_TO_BUFFER,
            MAP_BUFFER = CL_COMMAND_MAP_BUFFER,
            MAP_IMAGE = CL_COMMAND_MAP_IMAGE,
            UNMAP_MEM_OBJECT = CL_COMMAND_UNMAP_MEM_OBJECT,
            MARKER = CL_COMMAND_MARKER,
            ACQUIRE_GL_OBJECTS = CL_COMMAND_ACQUIRE_GL_OBJECTS,
            RELEASE_GL_OBJECTS = CL_COMMAND_RELEASE_GL_OBJECTS
                             };

        virtual CommandType getCommandType() const = 0;

        enum Status
        {
            /// command has been enqueued in the command-queue
            QUEUED = CL_QUEUED,

            /// enqueued command has been submitted by the host to the device associated with the command-queue
            SUBMITTED = CL_SUBMITTED,

            /// device is currently executing this command
            RUNNING = CL_RUNNING,

            /// command has successfully completed
            COMPLETE = CL_COMPLETE
                   };

        /// Tracks the execution status of a command, once the command object has been passed to CommandQueue::enque()
        Status getStatus() const;

        const Handle& getHandle() const { return _handle; }

        boost::shared_ptr<CommandQueue>& getCommandQueue() { return _cmdQueue; }

    protected:

        friend class CommandQueue;

        void setCommandQueue( boost::shared_ptr<CommandQueue> cmd )  {  _cmdQueue = cmd; }

        void execute();

        virtual void executePimp() = 0;

        void synchWaitingList();

        Handle _handle;

        bool _isBlocking;

        bool _waitingListDirty;

        bool _enquedSuccessfully;

        CommandWaitingList _waitingList;

        std::vector< Handle > _waitingListHandles;

        boost::shared_ptr<CommandQueue> _cmdQueue;
    };
}

#endif
