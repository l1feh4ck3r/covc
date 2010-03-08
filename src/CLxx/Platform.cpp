
#include "Platform.h"
#include "Device.h"
#include "Exception.h"
#include "Host.h"
#include <sstream>

namespace CLxx
{
	namespace
	{
		bool platformQueried = false;
		unsigned int numPlatforms = 0;
	}


	Platform::Platform(Handle h)
		:_handle(h) 
	{
	}

	std::string Platform::getName() const
	{
	      char chBuffer[256];
		  cl_int ciErrNum = clGetPlatformInfo (_handle, CL_PLATFORM_PROFILE, sizeof(chBuffer), &chBuffer, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
          return chBuffer;
	}

	std::string Platform::getProfile() const
	{
	      char chBuffer[256];
		  cl_int ciErrNum = clGetPlatformInfo (_handle, CL_PLATFORM_VERSION, sizeof(chBuffer), &chBuffer, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
          return chBuffer;
	}

	std::string Platform::getVersion() const
	{
	      char chBuffer[256];
		  cl_int ciErrNum = clGetPlatformInfo (_handle, CL_PLATFORM_NAME, sizeof(chBuffer), &chBuffer, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
          return chBuffer;
	}

	std::string Platform::getVendor() const
	{
	      char chBuffer[256];
		  cl_int ciErrNum = clGetPlatformInfo (_handle, CL_PLATFORM_VENDOR, sizeof(chBuffer), &chBuffer, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
          return chBuffer;
	}

	std::string Platform::getExtensions() const
	{
	      char chBuffer[4000];
		  cl_int ciErrNum = clGetPlatformInfo (_handle, CL_PLATFORM_EXTENSIONS, sizeof(chBuffer), &chBuffer, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
          return chBuffer;
	}


	void Platform::queryDevices()
	{
		cl_uint numDevices;
		cl_int ciErrNum = clGetDeviceIDs (_handle, CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);

		if (ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);

		cl_uint ciDeviceCount;
		std::vector<cl_device_id> clDeviceIDs(numDevices);
		ciErrNum = clGetDeviceIDs (_handle, CL_DEVICE_TYPE_ALL, numDevices, &clDeviceIDs[0], &ciDeviceCount);

		if (ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);

		for(unsigned int i = 0; i < ciDeviceCount; ++i ) 
		{ 
			cl_device_id deviceId = clDeviceIDs[i];

			boost::shared_ptr<Device> device = boost::shared_ptr<Device>(new Device(deviceId));

			//Host::add(deviceId,device);

			device->_platform = boost::shared_ptr<Platform>(this);

			_deviceList.push_back(device);
		}
	}

	const DeviceList& Platform::getDeviceList()
	{
		if ( _deviceList.empty() )
			queryDevices();

		return _deviceList;
	}
}
