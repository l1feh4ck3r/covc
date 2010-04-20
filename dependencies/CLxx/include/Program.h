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

#ifndef PROGRAM_H
#define PROGRAM_H

#include "fwd.h"
#include <set>
#include <map>
#include <boost/signal.hpp>

namespace CLxx
{
    /// An OpenCL program consists of a set of kernels that are identified as functions declared with
    /// the __kernel qualifier in the program source. OpenCL programs may also contain auxiliary
    /// functions and constant data that can be used by __kernel functions. The program executable
    /// can be generated online or offline by the OpenCL compiler for the appropriate target device(s).
    class Program
    {
    public:

        typedef cl_program Handle;


        enum BinaryStatus
        {
            VALID = CL_SUCCESS,
            INVALID_VALUE = CL_INVALID_VALUE,
            INVALID_BINARY = CL_INVALID_BINARY
                         };

        struct Binary
        {
            boost::shared_ptr<unsigned char> _binary;
            BinaryStatus _status;
        };

        typedef std::map<boost::shared_ptr<Device>, Binary> Binaries;


        /// creates a program object for a context, and loads the source code specified by the text string.
        /// The devices associated with the program object are the devices associated with context
        ///
        /// context : must be a valid OpenCL context
        /// source  : null-terminated character strings that make up the source code
        static boost::shared_ptr<Program> createProgram(boost::shared_ptr<Context> context, const char* source);

        static boost::shared_ptr<Program> createProgramFromFile(boost::shared_ptr<Context> context, const std::string & path);


        //static boost::shared_ptr<Program> createProgram(boost::shared_ptr<Context> context, std::vector<Device> devs, boost::shared_ptr<unsigned char> binary );


    public:

        ~Program()
        {
            clReleaseProgram(_handle);
        }

        typedef boost::signal<void (Program&) > BuildFinishedSignal;

        // Callback to signal when a program has finished building
        BuildFinishedSignal& getBuildFinishedSignal() { return _buildFinishedSignal; }

        enum BuildStatus
        {
            NONE = CL_BUILD_NONE,
            ERROR = CL_BUILD_ERROR,
            SUCCESS = CL_BUILD_SUCCESS,
            IN_PROGRESS = CL_BUILD_IN_PROGRESS
                      };

        /// builds (compiles & links) a program executable from the program source or binary for all the
        /// devices or a specific device(s) in the OpenCL context associated with program
        ///
        /// options     : is a pointer to a string that describes the build options to be used for building the program
        ///               executable. 
        ///
        /// return TRUE if build is successful, FALSE otherwise
        bool buildProgram( const char* options = NULL );

        /// builds (compiles & links) a program executable from the program source or binary for all the
        /// devices or a specific device(s) in the OpenCL context associated with program
        ///
        /// device_list : is a list of devices associated with program. If device_list is empty,
        ///               the program executable is built for all devices associated with program for which a source
        ///               or binary has been loaded. If device_list is not emty, the program executable is built
        ///               for devices specified in this list for which a source or binary has been loaded
        /// options     : is a pointer to a string that describes the build options to be used for building the program
        ///               executable. 
        ///
        /// return TRUE if build is successful, FALSE otherwise
        bool buildProgram( const std::vector<boost::shared_ptr<Device> >& devs, const char* options = NULL);



        /// Returns the build status of program for a specific device as given by device
        BuildStatus getBuildStatus() const;

        /// Return the build options
        const std::string& getBuildOptions() const { return _options; }

        /// Return the build log for the first device
        std::string getBuildLog() const;

        /// Return the build log for a given device
        std::string getBuildLog(const boost::shared_ptr<Device>& dev) const;

        const Handle& getHandle() const { return _handle; }

        boost::shared_ptr<Context> getContext() const { return _context; }


        cl_uint getReferenceCount() const;

        const std::set<boost::shared_ptr<Device> >& getDevices() const { return _devices; }

        const std::map<boost::shared_ptr<Device>, boost::shared_ptr<unsigned char> >& getBinaries() const { return _binaries; }

        const boost::shared_ptr<unsigned char>& getBinary(boost::shared_ptr<Device> dev) const;

        const std::string& getProgramSource() const { return _source; }

        const std::set<boost::shared_ptr<Kernel> >& getKernels() { return _kernels; }

        boost::shared_ptr<Kernel> createKernel(const char* kernel_name );

    protected:

        friend class Context;
        friend class Kernel;

        Program(Handle h);

        void addKernel( boost::shared_ptr<Kernel>& k ) { _kernels.insert( k ); }
        void removeKernel( boost::shared_ptr<Kernel>& k ) { _kernels.erase( k ); }
        boost::shared_ptr<Kernel> getKernel( cl_kernel k ) const;

        Handle _handle;

        std::set<boost::shared_ptr<Device> > _devices;
        std::map<boost::shared_ptr<Device>, boost::shared_ptr<unsigned char> > _binaries;

        std::string _source;

        std::string _options;

        boost::shared_ptr<Context> _context;

        std::set<boost::shared_ptr<Kernel> > _kernels;

        BuildFinishedSignal _buildFinishedSignal;

        boost::weak_ptr<Program> _this;
    };
}

#endif
