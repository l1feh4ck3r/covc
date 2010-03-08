
#include "Device.h"
#include "Exception.h"

namespace CLxx
{
	Device::Device(Handle h)
			:_handle(h) 
		{
		}

		  cl_uint Device::getAddressBits() const
		  {
			cl_uint value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_ADDRESS_BITS, sizeof(cl_uint), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  bool Device::isAvailable() const
		  {
			cl_bool value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_AVAILABLE, sizeof(cl_bool), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value == CL_TRUE;
		  }

		  bool Device::isCompilerAvailable() const
		  {
			cl_bool value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_COMPILER_AVAILABLE, sizeof(cl_bool), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value == CL_TRUE;
		  }

		  Device::DeviceFloatingPointCapabilities Device::getDeviceSingleFloatingPointCapabilities() const
		  {
			cl_device_fp_config value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_SINGLE_FP_CONFIG, sizeof(cl_device_fp_config), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return (Device::DeviceFloatingPointCapabilities) value;
		  }

		  /*
		  Device::DeviceFloatingPointCapabilities Device::getDeviceDoubleFloatingPointCapabilities() const
		  {
			cl_device_fp_config value;
			auto ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_DOUBLE_FP_CONFIG, sizeof(cl_device_fp_config), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return (Device::DeviceFloatingPointCapabilities) value;
		  }
		  

		  Device::DeviceFloatingPointCapabilities Device::getDeviceHalfFloatingPointCapabilities() const
		  {
			cl_device_fp_config value;
			auto ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_HALF_FP_CONFIG, sizeof(cl_device_fp_config), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return (Device::DeviceFloatingPointCapabilities) value;
		  }
		  */
		  bool Device::isEndianLittle() const
		  {
			cl_bool value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_ENDIAN_LITTLE, sizeof(cl_bool), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value == CL_TRUE;
		  }

		  bool Device::isErrorCorrectionSupported() const
		  {
			cl_bool value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_ERROR_CORRECTION_SUPPORT, sizeof(cl_bool), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value == CL_TRUE;
		  }

		  std::string Device::getExtensions() const
		  {
			char chBuffer[4000];
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_EXTENSIONS, sizeof(chBuffer), &chBuffer, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return chBuffer;
		  }

		  cl_ulong Device::getGlobalMemCacheSize() const
		  {
			cl_ulong value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(cl_ulong), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  Device::GlobalMemoryCacheType Device::getGlobalMemoryCacheType() const
		  {
			cl_device_mem_cache_type value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, sizeof(cl_device_mem_cache_type), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return (Device::GlobalMemoryCacheType) value;
		  }

		  cl_uint Device::getGlobalMemCachelineSize() const
		  {
			cl_uint value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, sizeof(cl_uint), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  Device::ExecutionCapabilities Device::getExecutionCapabilities() const
		  {
			cl_device_exec_capabilities value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_EXECUTION_CAPABILITIES, sizeof(cl_device_exec_capabilities), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return (Device::ExecutionCapabilities) value;
		  }


		  // The OpenCL device type
		  Device::DeviceType Device::getDeviceType() const
		  {
			cl_device_type clDevType;
			cl_int ciErrNum = clGetDeviceInfo(_handle, CL_DEVICE_TYPE, sizeof(clDevType), &clDevType, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return (Device::DeviceType) clDevType;
		  }

		  std::string Device::getName() const
		  {
			char chBuffer[256];
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_NAME, sizeof(chBuffer), &chBuffer, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return chBuffer;
		  }

		  std::string Device::getVendor() const
		  {
			char chBuffer[256];
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_VENDOR, sizeof(chBuffer), &chBuffer, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return chBuffer;
		  }

		  // A unique device vendor identifier
		  cl_uint Device::getVendorId() const
		  {
			cl_uint value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_VENDOR_ID, sizeof(cl_uint), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  std::string Device::getVersion() const
		  {
			char chBuffer[256];
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_VERSION, sizeof(chBuffer), &chBuffer, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return chBuffer;
		  }

		  std::string Device::getProfile() const
		  {
			char chBuffer[256];
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_PROFILE, sizeof(chBuffer), &chBuffer, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return chBuffer;
		  }

		  std::string Device::getDriverVersion() const
		  {
			char chBuffer[256];
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DRIVER_VERSION, sizeof(chBuffer), &chBuffer, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return chBuffer;
		  }


		  // The number of parallel compute cores on the OpenCL device. The minimum value is 1.
		  cl_uint Device::getMaxComputeUnits() const
		  {
			cl_uint value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_EXTENSIONS, sizeof(cl_uint), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  cl_uint Device::getMaxClockFreq() const
		  {
			cl_uint value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }


		  cl_ulong Device::getMaxMemAllocSize() const
		  {
			cl_ulong value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(cl_ulong), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  cl_bool Device::isImageSupported() const
		  {
			cl_bool value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  cl_uint Device::getMaxReadImageArgs() const
		  {
			cl_uint value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_MAX_READ_IMAGE_ARGS, sizeof(cl_uint), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  cl_uint Device::getMaxWriteImageArgs() const
		  {
			cl_uint value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_MAX_WRITE_IMAGE_ARGS, sizeof(cl_uint), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  size_t Device::getImage2dMaxWidth() const
		  {
			size_t value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(size_t), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  size_t Device::getImage2dMaxHeight() const
		  {
			size_t value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(size_t), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  size_t Device::getImage3dMaxWidth() const
		  {
			size_t value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_IMAGE3D_MAX_WIDTH, sizeof(size_t), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  size_t Device::getImage3dMaxHeight() const
		  {
			size_t value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_IMAGE3D_MAX_HEIGHT, sizeof(size_t), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  size_t Device::getImage3dMaxDepth() const
		  {
			size_t value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_IMAGE3D_MAX_DEPTH, sizeof(size_t), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  size_t Device::getMaxSamplers() const
		  {
			size_t value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_MAX_SAMPLERS, sizeof(size_t), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  size_t Device::getMaxParamSize() const
		  {
			size_t value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_MAX_PARAMETER_SIZE, sizeof(size_t), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }




		  cl_uint Device::getMemBaseAddrAlign() const
		  {
			cl_uint value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_MEM_BASE_ADDR_ALIGN, sizeof(cl_uint), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  cl_uint Device::getMinDataTypeAlignSize() const
		  {
			cl_uint value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, sizeof(cl_uint), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }



		  cl_ulong Device::getGlobalMemSize() const
		  {
			cl_ulong value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  cl_ulong Device::getMaxConstBufferSize() const
		  {
			cl_ulong value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(cl_ulong), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  cl_uint Device::getMaxConstArgs() const
		  {
			cl_uint value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_MAX_CONSTANT_ARGS, sizeof(cl_uint), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  Device::LocalMemoryType Device::getLocalMemoryType() const
		  {
			cl_device_local_mem_type value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_LOCAL_MEM_TYPE, sizeof(cl_device_local_mem_type), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return (Device::LocalMemoryType) value;
		  }

		  cl_ulong Device::getLocalMemSize() const
		  {
			cl_ulong value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }


		  size_t Device::getProfilingTimerResolution() const
		  {
			size_t value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_PROFILING_TIMER_RESOLUTION, sizeof(size_t), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }





		  Device::CommandQueueProperties Device::getCommandQueueProperties() const
		  {
			cl_command_queue_properties value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_QUEUE_PROPERTIES, sizeof(cl_command_queue_properties), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return (Device::CommandQueueProperties) value;
		  }

		  size_t Device::getMaxWorkItemDimensions() const
		  {
			size_t value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(size_t), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  void Device::getMaxWorkItemSize(std::vector<size_t>& items) const
		  {
			   cl_uint maxWorkItemDimensions = getMaxWorkItemDimensions();
			   items.resize(maxWorkItemDimensions);
  			   cl_int ciErrNum = clGetDeviceInfo(_handle, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * maxWorkItemDimensions, &(items[0]), NULL);

	   			if(ciErrNum != CL_SUCCESS)
					throw Exception(ciErrNum);
		  }

		  size_t Device::getMaxWorkGroupSize() const
		  {
			size_t value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  cl_uint Device::getPreferredVectorWidthChar() const
		  {
			cl_uint value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, sizeof(cl_uint), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  cl_uint Device::getPreferredVectorWidthShort() const
		  {
			cl_uint value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, sizeof(cl_uint), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  cl_uint Device::getPreferredVectorWidthInt() const
		  {
			cl_uint value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, sizeof(cl_uint), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  cl_uint Device::getPreferredVectorWidthLong() const
		  {
			cl_uint value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, sizeof(cl_uint), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  cl_uint Device::getPreferredVectorWidthFloat() const
		  {
			cl_uint value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, sizeof(cl_uint), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }

		  cl_uint Device::getPreferredVectorWidthDouble() const
		  {
			cl_uint value;
			cl_int ciErrNum =  clGetDeviceInfo(_handle, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, sizeof(cl_uint), &value, NULL);

			if(ciErrNum != CL_SUCCESS)
				throw Exception(ciErrNum);
			
			return value;
		  }



}

