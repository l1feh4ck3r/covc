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

#ifndef PLATFORM_H
#define PLATFORM_H

#include <string>
#include "fwd.h"

namespace CLxx
{
    class Platform
    {

    public:

        typedef cl_platform_id Handle;

        const Handle& getHandle() const { return _handle; }

        std::string getName() const;
        std::string getProfile() const;
        std::string getVersion() const;
        std::string getVendor() const;
        std::string getExtensions() const;

        const DeviceList& getDeviceList();

    protected:

        friend class Host;

        Platform(Handle h);

        void queryDevices();

        Handle _handle;

        DeviceList  _deviceList;

    };
};


#endif
