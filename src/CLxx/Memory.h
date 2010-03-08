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

#ifndef MEMORY_H
#define MEMORY_H

#include "fwd.h"

namespace CLxx
{
	/// Memory objects are categorized into two types: buffer objects, and image objects. A buffer
    /// object stores a one-dimensional collection of elements whereas an image object is used to store a
    /// two- or three- dimensional texture, frame-buffer or image.
	/// 
	/// The fundamental differences between a buffer and an image object are :
    /// · Elements in a buffer are stored in sequential fashion and can be accessed using a pointer
    ///   by a kernel executing on a device. Elements of an image are stored in a format that is
    ///   opaque to the user and cannot be directly accessed using a pointer. Built-in functions are
    ///   provided by the OpenCL C programming language to allow a kernel to read from or write to an image
	///
	/// · For a buffer object, the data is stored in the same format as it is accessed by the kernel,
    ///   but in the case of an image object the data format used to store the image elements may
    ///   not be the same as the data format used inside the kernel. Image elements are always a 4-
    ///   component vector (each component can be a float or signed/unsigned integer) in a kernel.
    ///   The built-in function to read from an image converts image element from the format it is
    ///   stored into a 4-component vector. Similarly, the built-in function to write to an image
    ///   converts the image element from a 4-component vector to the appropriate image format
    ///   specified such as 4 8-bit elements, for example
	///
	/// The Memory class is a base class for Buffer and Image
   class Memory
   {
   public:

	      typedef cl_mem Handle;

	      enum FlagEnum
		  {
		  	  READ_WRITE = CL_MEM_READ_WRITE,
			  WRITE_ONLY = CL_MEM_WRITE_ONLY,
			  READ_ONLY = CL_MEM_READ_ONLY,
			  USE_HOST_PTR = CL_MEM_USE_HOST_PTR,
			  ALLOC_HOST_PTR = CL_MEM_ALLOC_HOST_PTR,
			  COPY_HOST_PTR = CL_MEM_COPY_HOST_PTR
		  };

		  enum MemoryType
		  {
		     BUFFER = CL_MEM_OBJECT_BUFFER,
			 IMAGE2D = CL_MEM_OBJECT_IMAGE2D,
			 IMAGE3D = CL_MEM_OBJECT_IMAGE3D
		  };

		  typedef int Flags;

		  ~Memory()
		  {
		  	  clReleaseMemObject(_handle);
		  }

		  const Handle& getHandle() const { return _handle; }

 		  boost::shared_ptr<Context> getContext() const { return _context; }

		  MemoryType getMemoryType() const { return _memType; }

		  Flags getFlags() const { return _flags; }

		  size_t getMemSize() const;

		  void* getMemHostPointer() const;

		  cl_uint getMapCount() const;

		  cl_uint getReferenceCount() const;

   protected:

	      Memory(Handle h);

	      Handle _handle;

		  MemoryType _memType;

		  Flags _flags;

		  boost::shared_ptr<Context> _context;

   };
}

#endif
