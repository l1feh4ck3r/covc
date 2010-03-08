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

#ifndef BUFFER_H
#define BUFFER_H

#include "Memory.h"

namespace CLxx
{
 	
	/// A memory object that stores a linear collection of bytes
	///	Elements of a buffer object can be a scalar data type (such as an int, float), vector data type, or a
	/// user-defined structure.
   class Buffer : public Memory
   {
      public:

		  /// Create a buffer object 
		  /// context  : is a valid OpenCL context used to create the buffer object
		  /// flags    : is a bit-field that is used to specify allocation and usage information such as the memory
          ///            arena that should be used to allocate the buffer object and how it will be used.
		  ///            See Memory::FlagEnum for the possible values of flags 
		  /// mem_size : is the size in bytes of the buffer memory object to be allocated
		  /// host_ptr : is a pointer to the buffer data that may already be allocated by the application. The size
          ///            of the buffer that host_ptr points to must be >= size bytes
		  static boost::shared_ptr<Buffer> createBuffer( boost::shared_ptr<Context>& context, Flags flags, size_t mem_size, void* host_ptr = NULL);             

      protected:

		  Buffer(Handle h) 
			  : Memory(h){}
   };
}

#endif
