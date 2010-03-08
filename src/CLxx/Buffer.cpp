
#include "Buffer.h"
#include "Context.h"
#include "Device.h"
#include "Host.h"
#include "Exception.h"

namespace CLxx
{
	boost::shared_ptr<Buffer> Buffer::createBuffer( boost::shared_ptr<Context>& context, Buffer::Flags props, size_t mem_size, void* host_ptr )
	{
		cl_int errorCode;

		Handle memHandle = clCreateBuffer(context->getHandle(), props, mem_size, host_ptr, &errorCode );

		if ( errorCode != CL_SUCCESS )
			throw Exception(errorCode);

		boost::shared_ptr<Buffer> buffer = boost::shared_ptr<Buffer>( new Buffer(memHandle) );
		buffer->_context = context;
		buffer->_memType = BUFFER;
		buffer->_flags = props;

		//Host::add(memHandle,buffer);

		return buffer;
	}
}
