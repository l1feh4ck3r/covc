
#include "Profile.h"
#include "Exception.h"
#include "Context.h"
#include "Command.h"

namespace CLxx
{

	cl_ulong Profile::commandQueued(boost::shared_ptr<Command>& cmd)
	{
		cl_ulong value;			

		cl_int errorCode = clGetEventProfilingInfo(cmd->getHandle(), CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &value, NULL );

		if ( errorCode != CL_SUCCESS )
			throw Exception(errorCode);

		return value;
	}

	cl_ulong Profile::commandSubmit(boost::shared_ptr<Command>& cmd)
	{
		cl_ulong value;			

		cl_int errorCode = clGetEventProfilingInfo(cmd->getHandle(), CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &value, NULL );

		if ( errorCode != CL_SUCCESS )
			throw Exception(errorCode);

		return value;
	}

	cl_ulong Profile::commandStart(boost::shared_ptr<Command>& cmd)
	{
		cl_ulong value;			

		cl_int errorCode = clGetEventProfilingInfo(cmd->getHandle(), CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &value, NULL );

		if ( errorCode != CL_SUCCESS )
			throw Exception(errorCode);

		return value;
	}

	cl_ulong Profile::commandEnd(boost::shared_ptr<Command>& cmd)
	{
		cl_ulong value;			

		cl_int errorCode = clGetEventProfilingInfo(cmd->getHandle(), CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &value, NULL );

		if ( errorCode != CL_SUCCESS )
			throw Exception(errorCode);

		return value;
	}

}
