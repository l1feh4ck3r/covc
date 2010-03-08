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

#ifndef FWD_H
#define FWD_H

#include <vector>
#include <CL/cl.h>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>

namespace CLxx
{
   class Platform;
   class Device;
   class Context;
   class Memory;
   class Image;
   class Image2D;
   class Image3D;
   class Buffer;
   class CommandQueue;
   class Sampler;
   class Command;
   class Program;
   class Kernel;

   typedef std::vector< boost::shared_ptr<Platform> > PlatformList;
   typedef std::vector< boost::shared_ptr<Device> >   DeviceList;
   typedef std::vector< boost::shared_ptr<Context> >   ContextList;

}

#endif
