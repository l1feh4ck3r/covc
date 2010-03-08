
#include "Program.h"
#include "Exception.h"
#include "Context.h"
#include "Host.h"
#include "Device.h"
#include "Kernel.h"

#include <map>
#include <list>
#include <fstream>
#include <sstream>
#include <boost/shared_array.hpp>

namespace CLxx
{
	namespace
	{
		void contextErrorCallback(cl_program prog_id, void *user_data) 
		{
			Program* program = (Program*) user_data;
		    program->getBuildFinishedSignal()(*program);
		}
	}

	Program::Program(Handle h)
		:_handle(h) 
	{
	}

	boost::shared_ptr<Program> Program::createProgramFromFile(boost::shared_ptr<Context>& context, const char* path)
	{
		std::ifstream file(path);
		
		if ( !file )
			throw Exception(std::string("Error while opening ") + path);
		
		std::stringstream ss;	
		ss << file.rdbuf();
		file.close();
		
		return createProgram(context,ss.str().c_str());
	}

	boost::shared_ptr<Program> Program::createProgram(boost::shared_ptr<Context>& context, const char* source)
	{
		cl_int errorCode;			
	    
		cl_program handle = clCreateProgramWithSource(context->getHandle(), 1, &source , NULL, &errorCode );

		if ( errorCode != CL_SUCCESS )
			throw Exception(errorCode);

		boost::shared_ptr<Program> prog = boost::shared_ptr<Program>( new Program(handle) );
		prog->_context = context;
		prog->_source = source;
		prog->_this = prog;
        
		context->addProgram( prog ) ;

		//Host::add(handle,prog);

		return prog;
	}

	bool Program::buildProgram( const char* options )
	{
		std::vector< boost::shared_ptr<Device> > dev;
		return buildProgram(dev, options);
	}

	bool Program::buildProgram( const std::vector<boost::shared_ptr<Device> >& devs, const char* options)
	{
		_devices.insert(devs.begin(), devs.end());

		//_options = options;

		std::vector<cl_device_id> devices;

		for(std::vector<boost::shared_ptr<Device> >::const_iterator it = devs.begin(); it != devs.end(); it++ )
			devices.push_back((*it)->getHandle());

	    cl_int errorCode = clBuildProgram(_handle, _devices.size(), devs.empty() ? NULL : &devices[0], options, &contextErrorCallback, this);
		 
        if ( devs.empty() )
			_devices.insert(_context->getDevices().begin(), _context->getDevices().end());

		return errorCode == CL_SUCCESS;
	}

	cl_uint Program::getReferenceCount() const
	{
		cl_uint value;
		cl_int ciErrNum =  clGetProgramInfo(_handle, CL_PROGRAM_REFERENCE_COUNT, sizeof(cl_uint), &value, NULL);

		if(ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);

		return value;
	}

	Program::BuildStatus Program::getBuildStatus() const
	{
		BuildStatus value;
		cl_int ciErrNum =  clGetProgramInfo(_handle, CL_PROGRAM_BUILD_STATUS, sizeof(cl_build_status), &value, NULL);

		if(ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);

		return value;
	}

	std::string Program::getBuildLog() const
	{
		return getBuildLog(*(_devices.begin()));
	}

	std::string Program::getBuildLog(const boost::shared_ptr<Device>& dev) const
	{
        size_t ret_val_size;
		cl_int ciErrNum =  clGetProgramBuildInfo(_handle, dev->getHandle(), CL_PROGRAM_BUILD_LOG, 0, NULL, &ret_val_size);

		if(ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);

		boost::shared_array<char> value(new char[ ++ret_val_size] );

		ciErrNum =  clGetProgramBuildInfo(_handle, dev->getHandle(), CL_PROGRAM_BUILD_LOG, ret_val_size, value.get(), NULL);

		if(ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);

		return value.get();
	}

	boost::shared_ptr<Kernel> Program::createKernel(const char* kernel_name )
	{
		return Kernel::createKernel( _this.lock(), kernel_name);
	}

	boost::shared_ptr<Kernel> Program::getKernel( cl_kernel k ) const 
	{
		for( std::set<boost::shared_ptr<Kernel>>::const_iterator it = _kernels.begin(); it != _kernels.end(); it++)
		{
			if ( (*it)->getHandle() == k )
				return const_cast<boost::shared_ptr<Kernel>&>(*it);
		}

		return boost::shared_ptr<Kernel>();
	}

}
