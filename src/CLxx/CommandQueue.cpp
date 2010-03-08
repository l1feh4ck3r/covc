
#include "CommandQueue.h"
#include "Command.h"
#include "Host.h"
#include "Context.h"
#include "Device.h"
#include "Exception.h"

namespace CLxx
{
	CommandQueue::CommandQueue(Handle h)
			:_handle(h) 
		{
		}

	boost::shared_ptr<CommandQueue> CommandQueue::createCommandQueue( boost::shared_ptr<Context>& context, CommandQueue::Properties props, boost::shared_ptr<Device>& dev )
	{
		cl_int errorCode;
		boost::shared_ptr<Device> deviceUsed;
	
		if ( dev.get() )
			deviceUsed = dev;
		else
			deviceUsed = context->getDevices().front();
			
		cl_command_queue commandQueueHandle = clCreateCommandQueue(context->getHandle(), deviceUsed->getHandle(), props, &errorCode );

		if ( errorCode != CL_SUCCESS )
			throw Exception(errorCode);

		boost::shared_ptr<CommandQueue> queue = boost::shared_ptr<CommandQueue>( new CommandQueue(commandQueueHandle) );
		queue->_this = queue;
		queue->_context = context;
		queue->_device = deviceUsed;
		queue->_properties = props;

		context->addCommandQueue(queue);

		//Host::add(commandQueueHandle,queue);

		return queue;
	}

	cl_uint CommandQueue::getReferenceCount() const
	{
		cl_uint value;
		cl_int err = clGetCommandQueueInfo(_handle,CL_QUEUE_REFERENCE_COUNT,sizeof(cl_uint), &value, NULL); 

		if ( err != CL_SUCCESS )
			throw Exception(err);

		return value;
	}

	void CommandQueue::enableProperty( Properties prop, cl_bool val )
	{
		cl_int err = clSetCommandQueueProperty( _handle, (cl_command_queue_properties) prop , val, NULL);

		if ( err != CL_SUCCESS )
			throw Exception(err);

		if ( val == CL_TRUE )
			_properties |= prop;
		else
			_properties &= !prop;
	}

	void CommandQueue::enque(boost::shared_ptr<Command> cmd)
	{
		if ( cmd.get() )
		{
			cmd->setCommandQueue( _this.lock() );
			cmd->execute();
		}
	}

	void CommandQueue::flush()
	{
		cl_int err = clFlush( _handle);

		if ( err != CL_SUCCESS )
			throw Exception(err);
	}

	void CommandQueue::finish()
	{
		cl_int err = clFinish( _handle);

		if ( err != CL_SUCCESS )
			throw Exception(err);
	}

}
