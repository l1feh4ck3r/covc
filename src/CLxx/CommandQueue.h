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

#ifndef COMMANDQUEUE_H
#define COMMANDQUEUE_H

#include "fwd.h"

namespace CLxx
{
	/// A CommandQueue holds commands that will be executed on a specific device.
    /// The CommandQueue is created on a specific device in a context. Commands to a CommandQueue
    /// are queued in-order but may be executed in-order or out-of-order. 
    /// Refer to In-order Execution and Out-of-order Execution.
    ///
	/// Having multiple CommandQueues allows applications to queue multiple independent commands without
    /// requiring synchronization. Note that this should work as long as these objects are not being
    /// shared. Sharing of objects across multiple command-queues will require the application to
    /// perform appropriate synchronization.
    ///
	/// Note : It is possible that a device(s) becomes unavailable after a context and command-queues that use
    ///        this device(s) have been created and commands have been queued to command-queues. In this
    ///        case the behavior of OpenCL API calls that use this context (and command-queues) are
    ///        considered to be implementation-defined. The user callback function, present in Context,
	///        can be used to record appropriate information in the errinfo, private_info
    ///        arguments passed to the callback function when the device becomes unavailable

	class CommandQueue
	{

	public:

		typedef cl_command_queue Handle;

		typedef int Properties;

		enum PropertyEnum
		{
			/// If set, the commands in the command-queueare executed out-of-order.
            /// Otherwise, commands are executed in-order.
			OUT_OF_ORDER_EXEC_MODE_ENABLE = CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE,  

			/// Enable or disable profiling of commands in the command-queue
			PROFILING_ENABLE = CL_QUEUE_PROFILING_ENABLE                             
		};


		/// context  : must be a valid OpenCL context
		/// props    : specifies a list of properties for the command-queue. This is a bit-field and is
        ///            described in PropertyEnum

		static boost::shared_ptr<CommandQueue> createCommandQueue( boost::shared_ptr<Context>& context, Properties props = PROFILING_ENABLE, boost::shared_ptr<Device>& dev = boost::shared_ptr<Device>() );

	public:

		~CommandQueue()  
		{
			clReleaseCommandQueue(_handle);
		}

		const Handle& getHandle() const { return _handle; }

		boost::shared_ptr<Context>& getContext() { return _context; }

		const boost::shared_ptr<Context>& getContext() const { return _context; }

		boost::shared_ptr<Device>& getDevice() { return _device; }

		const boost::shared_ptr<Device>& getDevice() const { return _device; }

		/// Return the currently specified properties for the command-queue
		
		Properties getProperties() const { return _properties; }


		/// Return the command-queue reference count.
		
		cl_uint getReferenceCount() const;

		void enableProperty( Properties prop ) { enableProperty(prop, CL_TRUE ); }

		void disableProperty( Properties prop ) { enableProperty(prop, CL_FALSE ); }

		bool isPropertyEnabled( Properties prop ) const { return (_properties & prop) > 0; }

		/// enque commands on this command queue.
        /// A command can be blooking or not. If blooking this function 
		/// will not return until the Command is completed. 
		/// Otherwise the function will return immediatelly.

		void enque(boost::shared_ptr<Command> cmd);

		/// Issues all previously queued OpenCL commands in command_queue to the device associated
        /// with command_queue. Flush only guarantees that all queued commands to command_queue
        /// get issued to the appropriate device. There is no guarantee that they will be complete after
        /// flush returns.
		/// 
		/// To use Commands that are enqueued in a command-queue to wait on by
        /// Commands enqueued in a different command-queue, the application must call a
        /// flush or any blocking commands that perform an implicit flush of the command-queue 
        /// where the these commands are enqueued

		void flush();

		/// blocks until all previously queued OpenCL commands in command_queue are issued to the
        /// associated device and have completed. Finish does not return until all queued commands in
        /// command_queue have been processed and completed. Finish is also a synchronization point
		
		void finish();

	protected:

		friend class Context;

		CommandQueue(Handle h);

		void enableProperty( Properties prop, cl_bool val );

		Handle _handle;

		boost::shared_ptr<Context> _context;

		boost::shared_ptr<Device> _device;

		Properties _properties;

		boost::weak_ptr<CommandQueue> _this;
	};
}

#endif
