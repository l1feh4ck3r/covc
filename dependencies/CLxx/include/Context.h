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

#ifndef CONTEXT_H
#define CONTEXT_H

#include "Device.h"
#include "Buffer.h"
#include "CommandQueue.h"
#include <set>
#include <boost/signal.hpp>

namespace CLxx
{
    /// The environment within which the kernels execute and the domain in which
    /// synchronization and memory management is defined. The context includes a set of devices, the
    /// memory accessible to those devices, the corresponding memory properties and one or more
    /// command-queues used to schedule execution of a kernel(s) or operations on memory objects

    class Context
    {
    public:

        typedef cl_context Handle;

        enum PropertiesEnum
        {
            CONTEXT_PLATFORM = CL_CONTEXT_PLATFORM
                           };

        typedef cl_context_properties PropertyValue;

        typedef std::pair<PropertiesEnum, PropertyValue> Property;


        /// Creates an OpenCL context. An OpenCL context is created with one or more devices. Contexts
        /// are used by the OpenCL runtime for managing objects such as command-queues, memory,
        /// program and kernel objects and for executing kernels on one or more devices specified in the context.
        ///
        /// devices : list of unique devices5

        static boost::shared_ptr<Context> createContext( const std::vector<boost::shared_ptr<Device> >& devices)
        {
            return createContextImpl(devices, NULL);
        }

        /// Creates an OpenCL context. An OpenCL context is created with one or more devices. Contexts
        /// are used by the OpenCL runtime for managing objects such as command-queues, memory,
        /// program and kernel objects and for executing kernels on one or more devices specified in the context.
        ///
        /// devices    : list of unique devices5
        /// properties : specifies a list of context property names and their corresponding values.
        ///              Each property name is immediately followed by the corresponding desired value. 
        ///              The list of supported properties is described in PropertiesEnum. 
        ///              The property vector can be empty in which case the platform that is selected is implementation-defined

        static boost::shared_ptr<Context> createContext( const std::vector<boost::shared_ptr<Device> >& devices, const std::vector<Property>& properties )
        {
            return createContextImpl(devices, &properties);
        }

        /// Creates an OpenCL context from a device type that identifies the specific device(s) to use
        ///
        /// deviceType : is a bit-field that identifies the type of device and is described in Device::DeviceTypes

        static boost::shared_ptr<Context> createContext( Device::DeviceType deviceType )
        {
            return createContextImpl(deviceType, NULL);
        }

        /// Creates an OpenCL context from a device type that identifies the specific device(s) to use
        ///
        /// deviceType : is a bit-field that identifies the type of device and is described in Device::DeviceTypes
        /// properties : specifies a list of context property names and their corresponding values.
        ///              Each property name is immediately followed by the corresponding desired value. 
        ///              The list of supported properties is described in PropertiesEnum. 
        ///              The property vector can be empty in which case the platform that is selected is implementation-defined

        static boost::shared_ptr<Context> createContext( Device::DeviceType deviceType, const std::vector<Property>& properties)
        {
            return createContextImpl(deviceType,  &properties);
        }

    protected:

        static boost::shared_ptr<Context> createContextImpl( const std::vector<boost::shared_ptr<Device> >& devices, const std::vector<Property>* properties);

        static boost::shared_ptr<Context> createContextImpl( Device::DeviceType deviceType, const std::vector<Property>* properties);

    public:


        ~Context() { clReleaseContext(_handle);  }

        const Handle& getHandle() const { return _handle; }

        cl_uint getContextReferenceCount() const;

        const std::vector<boost::shared_ptr<Device> >& getDevices() const { return _devices; }

        const std::vector<Property>& getProperties() const { return _properties; }

        const std::set<boost::shared_ptr<CommandQueue> >& getCommandQueues() const { return _commandQueues; }

        const std::set<boost::shared_ptr<Program> >& getPrograms() const { return _programs; }

        /// Callbacks registered by the application. This signal
        /// will be used by the OpenCL implementation to report information on errors that occur in this
        /// context. This signal may be called asynchronously by the OpenCL implementation.
        /// It is the application’s responsibility to ensure that the callback function is thread-safe.

        typedef boost::signal<void (const char *errinfo, const void *private_info, size_t cb)>  ErrorCallback;

        ErrorCallback& getErrorSignal() { return _errorCallbacks; }

        boost::shared_ptr<CommandQueue> createCommandQueue( CommandQueue::Properties props = CommandQueue::PROFILING_ENABLE, boost::shared_ptr<Device> dev = boost::shared_ptr<Device>() );

        boost::shared_ptr<Program> createProgram(const char* source);

        boost::shared_ptr<Program> createProgramFromFile(const std::string & path);

        boost::shared_ptr<Buffer> createBuffer(Memory::Flags props, size_t mem_size, void* host_ptr = NULL);



    protected:

        friend class CommandQueue;
        friend class Program;

        Context(){}

        void addCommandQueue(boost::shared_ptr<CommandQueue>& cmd);

        void removeCommandQueue(boost::shared_ptr<CommandQueue>& cmd);

        void addProgram(boost::shared_ptr<Program>& cmd) { _programs.insert(cmd); }

        void removeProgram(boost::shared_ptr<Program>& cmd) { _programs.erase(cmd); }

        void retriveDevices(Device::DeviceType deviceType);

        Handle _handle;

        std::vector<Property> _properties;

        std::vector<boost::shared_ptr<Device> > _devices;

        std::set<boost::shared_ptr<CommandQueue> > _commandQueues;

        std::set<boost::shared_ptr<Program> > _programs;

        ErrorCallback _errorCallbacks;

        boost::weak_ptr<Context> _this;
    };
};


#endif
