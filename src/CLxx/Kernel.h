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

#ifndef KERNEL_H
#define KERNEL_H

#include "fwd.h"
#include "Buffer.h"
#include "Exception.h"

namespace CLxx
{

	template<size_t size, typename T>
	std::pair<T*,size_t> array_ptr(T* ptr)
	{
		return std::make_pair<T*,size_t>(ptr,size);
	}

	/// No changes to the program executable are
    /// allowed while there are kernel objects associated with a program object. This means that calls to
	/// buildProgram() generate an INVALID OPERATION exception if there are kernel objects attached to a
    /// program object. The OpenCL context associated with program will be the context associated
    /// with kernel. The list of devices associated with program are the devices associated with kernel.
    /// Devices associated with a program object for which a valid program executable has been built
    /// can be used to execute kernels declared in the program object
	///
	/// Argument setting ( see setArgument ) :
	/// The argument data pointed to by arg_value is copied and the arg_value pointer
    /// can therefore be reused by the application after setArgument returns. The argument value
    /// specified is the value used by all API calls that enqueue kernel (NDRangeCommand and
    /// TaskCommand) until the argument value is changed by a call to setArgument for kernel.
    /// If the argument is a memory object (buffer or image), the arg_value entry will be a pointer to the
    /// appropriate buffer or image object. The memory object must be created with the context
    /// associated with the kernel object. A NULL value can also be specified if the argument is a buffer
    /// object in which case a NULL value will be used as the value for the argument declared as a
    /// pointer to __global or __constant memory in the kernel. If the argument is declared with the
    /// __local qualifier, the arg_value entry must be NULL. If the argument is of type Sampler, the
    /// arg_value entry must be a pointer to the sampler object. For all other kernel arguments, the
    /// arg_value entry must be a pointer to the actual data to be used as argument value
	/// The memory object specified as argument value must be a buffer object (or NULL) if the
    /// argument is declared to be a pointer of a built-in or user defined type with the __global or
    /// __constant qualifier. If the argument is declared with the __constant qualifier, the size in bytes
	/// of the memory object cannot exceed Device::MaxConstantBufferSize and the
	/// number of arguments declared with the __constant qualifier cannot exceed Device::MaxConstantArgs.
    /// The memory object specified as argument value must be a 2D image object if the argument is
    /// declared to be of type Image2d. The memory object specified as argument value must be a 3D
    /// image object if argument is declared to be of type Image3d

	class Kernel
	{
	public:

		typedef cl_kernel Handle;

		/// program     : is a program object with a successfully built executable.
		/// kernel_name : is a function name in the program declared with the __kernel qualifier
		static boost::shared_ptr<Kernel> createKernel(boost::shared_ptr<Program> prog, const char* kernel_name );

		/// creates kernel objects for all kernel functions in program. Kernel objects are not created for any
        /// __kernel functions in program that do not have the same function definition across all devices
        /// for which a program executable has been successfully built
		///
        /// program  : is a program object with a successfully built executable.
		/// kernels  : list of kernels created
		static void createKernels(boost::shared_ptr<Program> prog, std::vector<boost::shared_ptr<Kernel>>& kernels );

	public:

		friend class Program;

		~Kernel()
		{
			clReleaseKernel(_handle);
		}

		const Handle& getHandle() const { return _handle; }

		boost::shared_ptr<Program> getProgram() const { return _program; }

        /// Return the kernel reference count
		cl_uint getReferenceCount() const;

		/// Return the kernel function name
		std::string getFunctionName() const;

		/// This provides a mechanism for the application to query the maximum
        /// work-group size that can be used to execute a kernel on a specific device
        /// given by device. The OpenCL implementation uses the resource requirements of
        /// the kernel (register usage etc.) to determine what this workgroup size should be
		size_t getWorkGroupSize(boost::shared_ptr<Device> device) const;

		/// Returns the work-group size specified by the
        /// __attribute__((reqd_work_group_size(X, Y, Z))) qualifier.
        /// If the work-group size is not specified
        /// using the above attribute qualifier (0, 0,0) is returned.
		void getCompileWorkGroupSize(boost::shared_ptr<Device> device, size_t[3]) const;

		/// Returns the amount of local memory in bytes being used by a kernel. This
        /// includes local memory that may be needed by an implementation to execute
        /// the kernel, variables declared inside the kernel with the __local address
        /// qualifier and local memory to be allocated for arguments to the kernel
        /// declared as pointers with the __local address qualifier and whose size is
        /// specified with setArgument. If the local memory size, for any pointer
        /// argument to the kernel declared with the __local address qualifier, is not
        /// specified, its size is assumed to be 0
		cl_ulong getLocamMemSize(boost::shared_ptr<Device> device) const;

		/// get number of arguments of the kernel 
		cl_uint getNumArgs() const;

		/// Is used to set the argument value for a specific argument of a kernel
		template<typename T>
		void setArgument(cl_uint arg_index, T& arg_value) { setArg(arg_index,value); }

		/// set the argument value 
		template<typename T0>
		void setArguments(T0& t0) { setArg(0,t0); }

		/// set the arguments values 
		template<typename T0, typename T1>
		void setArguments(T0& t0, T1& t1) { setArg(0,t0); setArg(1,t1);}

		/// set the arguments values 
		template<typename T0, typename T1, typename T2>
		void setArguments(T0& t0, T1& t1, T2& t2) { setArg(0,t0); setArg(1,t1); setArg(2,t2);}

		/// set the arguments values 
		template<typename T0, typename T1, typename T2, typename T3>
		void setArguments(T0& t0, T1& t1, T2& t2, T3& t3) { setArg(0,t0); setArg(1,t1); setArg(2,t2); setArg(3,t3);}

		/// set the arguments values 
		template<typename T0, typename T1, typename T2, typename T3, typename T4>
		void setArguments(T0& t0, T1& t1, T2& t2, T3& t3, T4& t4) { setArg(0,t0); setArg(1,t1); setArg(2,t2); setArg(3,t3); setArg(4,t4);}

		/// set the arguments values 
		template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
		void setArguments(T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T0& t5) { setArg(0,t0); setArg(1,t1); setArg(2,t2); setArg(3,t3); setArg(4,t4); setArg(5,t5);}

	protected:

		Kernel(Handle h);

		Handle _handle;

		boost::shared_ptr<Program> _program;

	protected:

		template<typename T>
		void setArg(cl_uint arg_index, T& value)
		{
			cl_int ciErrNum =  clSetKernelArg(_handle, arg_index, sizeof(value), &value);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
		}

		template<>
		void setArg(cl_uint arg_index, boost::shared_ptr<Buffer>& value)
		{
			cl_int ciErrNum =  clSetKernelArg(_handle, arg_index, sizeof(Memory::Handle), &(value->getHandle()) );

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
		}
/*
		template<typename T, size_t size>
		void setArg(cl_uint arg_index, T (&array)[size])
		{
			size_t s = sizeof(T) * size;
			cl_int ciErrNum =  clSetKernelArg(_handle, arg_index, sizeof(T) * size, array);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
		}
*/
		template<typename T>
		void setArg(cl_uint arg_index, std::pair<T*,size_t>& value)
		{
			cl_int ciErrNum =  clSetKernelArg(_handle, arg_index, value.second * sizeof(T), value.first);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
		}

		template<typename T>
		void setArg(cl_uint arg_index, T* value)
		{
			std::stringstream ss;
			ss << "Argument " << arg_index << " is a pointer. "
			   << "Need to provide size of the array pointed. " 
			   << "Use array_ptr, to pass the pointer";   
			throw Exception( ss.str() );
		}

		};
	}

#endif
