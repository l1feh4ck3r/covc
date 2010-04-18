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

#ifndef KERNELCOMMANDS_H
#define KERNELCOMMANDS_H

#include "Command.h"

namespace CLxx
{
    // Base class to execute kernels on the device
    class KernelCommand : public Command
    {
    public:

        KernelCommand(boost::shared_ptr<Kernel>& kernel);

        boost::shared_ptr<Kernel> getKernel() const { return _kernel; }

    protected:

        boost::shared_ptr<Kernel> _kernel;
    };


    /// Base class for Range Commands
    ///
    /// Notes on Global and Local work size
    ///                   The total number of work-items in a work-group is
    ///                   computed as local_work_size[0] * … * local_work_size[N-1]. The total number of
    ///                   work-items in the work-group must be less than or equal to the
    ///                   Device::MaxWorkGroupSize() value and the number of workitems
    ///                   specified in local_work_size[0], … local_work_size[2] must be less than or equal to the 
    ///                   corresponding values specified by Device::MaxWorkItemSize()[0], …,Device::MaxWorkItemSize()[N-1].
    ///                   The explicitly specified local_work_size will be used to determine how
    ///                   to break the global work-items specified by global_work_size into appropriate
    ///                   work-group instances. If local_work_size is specified, the values specified in
    ///                   global_work_size[0], … global_work_size[2] must be evenly divisible
    ///                   by the corresponding values specified in local_work_size[0], … local_work_size[N-1].
    ///
    ///                   The work-group size to be used for kernel can also be specified in the program source using the
    ///                   __attribute__((reqd_work_group_size(X, Y, Z)))qualifier.
    ///                   In this case the size of work group specified by local_work_size must match the value
    ///                   specified by the reqd_work_group_size attribute qualifier
    ///
    ///                   local_work_size can also be 0 in which case the OpenCL implementation will
    ///                   determine how to be break the global work-items into appropriate work-group instances
    ///
    ///                   Each work-item is uniquely identified by a global identifier. The global ID, which can be read
    ///                   inside the kernel, is computed using the value given by global_work_size and
    ///                   global_work_offset. In OpenCL 1.0, the starting global ID is always (0, 0, … 0). In addition, a
    ///                   work-item is also identified within a work-group by a unique local ID. The local ID, which can
    ///                   also be read by the kernel, is computed using the value given by local_work_size. The starting
    ///                   local ID is always (0, 0, … 0).
    template< int N >
    class RangeNDCommand : public KernelCommand
    {
        size_t getGlobalWorkSize(unsigned int d) const { return _globalWorkSize[d]; }

        size_t getLocalWorkSize() const { return _localWorkSize; }

        virtual CommandType getCommandType() const { return NDRANGE_KERNEL; }

    protected:

        RangeNDCommand(boost::shared_ptr<Kernel>& kernel) : KernelCommand(kernel){}

        size_t _globalWorkSize[N];
        size_t _localWorkSize;
    };

    // Command to execute a kernel on a device working on a single dimension
    class Range1DCommand : public RangeNDCommand<1>
    {
    public:

        /// kernel          : is a valid kernel object. The OpenCL context associated with kernel and command-queue
        ///                   must be the same.
        /// size1D          : number of global work-items
        /// local_work_size : describe the number of work-items that make up a work-group
        ///                   (also referred to as the size of the work-group) 
        Range1DCommand(boost::shared_ptr<Kernel>& kernel, size_t size1D, size_t local_work_size = 0);

    protected:

        virtual void executePimp();
    };

    // Command to execute a kernel on a device working on a two dimensions
    class Range2DCommand : public RangeNDCommand<2>
    {
    public:

        /// kernel          : is a valid kernel object. The OpenCL context associated with kernel and command-queue
        ///                   must be the same.
        /// size1D,
        /// size2D          : number of global work-items for each dimension
        /// local_work_size : describe the number of work-items that make up a work-group
        ///                   (also referred to as the size of the work-group) 
        Range2DCommand(boost::shared_ptr<Kernel>& kernel, size_t size1D, size_t size2D, size_t local_work_size = 0);

    protected:

        virtual void executePimp();
    };

    // Command to execute a kernel on a device working on a three dimensions
    class Range3DCommand : public RangeNDCommand<3>
    {
    public:

        /// kernel          : is a valid kernel object. The OpenCL context associated with kernel and command-queue
        ///                   must be the same.
        /// size1D,
        /// size2D,
        /// size3D          : number of global work-items for each dimension
        /// local_work_size : describe the number of work-items that make up a work-group
        ///                   (also referred to as the size of the work-group) 
        Range3DCommand(boost::shared_ptr<Kernel>& kernel, size_t size1D, size_t size2D, size_t size3D, size_t local_work_size = 0);

    protected:

        virtual void executePimp();
    };


    /// Command to execute a kernel on a device.
    /// The kernel is executed using a single work-item
    class TaskCommand : public KernelCommand
    {
        TaskCommand(boost::shared_ptr<Kernel>& kernel) : KernelCommand(kernel){}

        virtual CommandType getCommandType() const { return TASK; }

    protected:

        virtual void executePimp();
    };

    /*
	// TODO
	/// command to execute a native C/C++ function not compiled using the OpenCL compiler
	class NativeKernel : public KernelCommand
	{
	    typedef void (*user_func)(void *) Func;

		NativeKernel(boost::shared_ptr<Kernel> kernel) : KernelCommand(kernel){}

		virtual CommandType getCommandType() const { return TASK; }		

	protected:

		virtual void executePimp();  		
	};
	*/
}

#endif
