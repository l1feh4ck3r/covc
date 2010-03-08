
#include "Sampler.h"
#include "Exception.h"
#include "Context.h"
#include "Host.h"

namespace CLxx
{
	Sampler::Sampler(Handle h)
		:_handle(h) 
	{
	}


	boost::shared_ptr<Sampler> Sampler::createSampler(boost::shared_ptr<Context>& context, bool normalized_coords, AddressingMode addr, FilteringMode filter )
	{
		cl_int errorCode;			

		cl_sampler handle = clCreateSampler(context->getHandle(), (normalized_coords ? CL_TRUE : CL_FALSE), addr, filter, &errorCode );

		if ( errorCode != CL_SUCCESS )
			throw Exception(errorCode);

		boost::shared_ptr<Sampler> queue = boost::shared_ptr<Sampler>( new Sampler(handle) );
		queue->_context = context;

		//Host::add(handle,queue);

		return queue;
	}

	cl_uint Sampler::getReferenceCount() const
	{
		cl_uint value;
		cl_int ciErrNum =  clGetSamplerInfo(_handle, CL_MEM_REFERENCE_COUNT, sizeof(cl_uint), &value, NULL);

		if(ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);

		return value;
	}

	Sampler::AddressingMode Sampler::getAddressingMode() const
	{
		AddressingMode value;
		cl_int ciErrNum =  clGetSamplerInfo(_handle, CL_SAMPLER_ADDRESSING_MODE, sizeof(cl_addressing_mode), &value, NULL);

		if(ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);

		return value;
	}

	Sampler::FilteringMode Sampler::getFilteringMode() const
	{
		FilteringMode value;
		cl_int ciErrNum =  clGetSamplerInfo(_handle, CL_SAMPLER_FILTER_MODE, sizeof(cl_filter_mode), &value, NULL);

		if(ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);

		return value;
	}

	bool Sampler::isNormilizedCoords() const
	{
		cl_bool value;
		cl_int ciErrNum =  clGetSamplerInfo(_handle, CL_SAMPLER_FILTER_MODE, sizeof(cl_bool), &value, NULL);

		if(ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);

		return value == CL_TRUE;
	}

}
