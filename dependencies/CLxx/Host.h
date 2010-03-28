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

#ifndef HOST_H
#define HOST_H

#include "fwd.h"

namespace CLxx
{
    /// The Host class rappresent the host concept in OpenCL
    class Host
    {
    public:

        /// The list of platforms available
        static const PlatformList& getPlatformList();

        /// Allows the implementation to release the resources allocated by the OpenCL compiler. This is a
        /// hint from the application and does not guarantee that the compiler will not be used in the future
        /// or that the compiler will actually be unloaded by the implementation. Calls to buildProgram()
        /// after unloadCompiler() will reload the compiler, if necessary, to build the appropriate program executable
        static void unloadCompiler();

    protected:

        static void queryPlatforms();

    protected:

        Host(){}

    };
}

#endif
