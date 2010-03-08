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

#ifndef SAMPLER_H
#define SAMPLER_H

#include "fwd.h"

namespace CLxx
{
   /// A sampler object describes how to sample an image when the image is read in the kernel. 
   class Sampler
   {
   public:

	      typedef cl_sampler Handle;

		  enum AddressingMode
		  {
		  	 REPEAT = CL_ADDRESS_REPEAT,
			 CLAMP_TO_EDGE = CL_ADDRESS_CLAMP_TO_EDGE,
			 CLAMP = CL_ADDRESS_CLAMP,
			 NONE = CL_ADDRESS_NONE
		  };

		  enum FilteringMode
		  {
		  	 NEAREST = CL_FILTER_NEAREST,
			 LINEAR = CL_FILTER_LINEAR
		  };

		  /// context           : must be a valid OpenCL context.
		  /// normalized_coords : determines if the image coordinates specified are normalized ( if TRUE or not if FALSE )
		  /// addr_mode         : specifies how out-of-range image coordinates are handled when reading from an image.  
		  /// filter_mode       : specifies the type of filter that must be applied when reading an image
		  static boost::shared_ptr<Sampler> createSampler(boost::shared_ptr<Context>& context, bool normalized_coords = true, AddressingMode addr_mode = NONE, FilteringMode filter_mode = NEAREST );


   public:

		  ~Sampler()
		  {
		  	  clReleaseSampler(_handle);
		  }

		  const Handle& getHandle() const { return _handle; }

		  /// Return the context specified when the sampler is created.
 		  boost::shared_ptr<Context> getContext() const { return _context; }

		  /// Return the sampler reference count
    	  cl_uint getReferenceCount() const;

		  /// return the AddressingMode specified when the sample was created
		  AddressingMode getAddressingMode() const;

		  /// return the FilteringMode specified when the sample was created
		  FilteringMode getFilteringMode() const;

		  /// return if the coords are normalized 
		  bool isNormilizedCoords() const;

   protected:

	      Sampler(Handle h);

	      Handle _handle;

		  boost::shared_ptr<Context> _context;

   };
}

#endif
