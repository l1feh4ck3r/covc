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

#ifndef SYNCCOMMANDS_H
#define SYNCCOMMANDS_H

#include "Command.h"

namespace CLxx
{
    /// MarkerCommand can be used to queue and wait on this marker event
    /// i.e. wait for all commands queued before the marker command to complete.

    class MarkerCommand : public Command
    {

    public:

        MarkerCommand(){}

        virtual CommandType getCommandType() const { return MARKER; }

    protected:

        virtual void executePimp();

    };


    /// WaitForCommand waits for a specific command or a list of commands to complete before any future commands
    /// queued in the command-queue are executed.
    /// The list of commands is specified in CommandWaitingList

    class WaitForCommand : public Command
    {
        /// Waits on the host thread for commands present in CommandWaitingList to complete.
        /// A command is considered complete if its execution status is COMPLETE or a negative value.
        /// The commands specified in CommandWaitingList act as synchronization points
        static void toFinish(const CommandWaitingList& cmd);

        /// Waits on the host thread for the command to complete.
        /// A command is considered complete if its execution status is COMPLETE or a negative value.
        /// The commands specified act as synchronization point
        static void toFinish(const boost::shared_ptr<Command>& cmd);


    public:

        WaitForCommand(){}

        virtual CommandType getCommandType() const { return SYNC; }

    protected:

        virtual void executePimp();

    };

    /// Create a barrier operation. The BarrierCommand command ensures that all queued commands
    /// in CommandQueue have finished execution before the next batch of commands can begin execution. 
    /// BarrierCommand is a synchronization point.

    class BarrierCommand : public Command
    {
    public:

        BarrierCommand(){}

        virtual CommandType getCommandType() const { return SYNC; }

    protected:

        virtual void executePimp();

    };

}

#endif
