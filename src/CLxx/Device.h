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

#ifndef DEVICE_H
#define DEVICE_H

#include <string>
#include "fwd.h"

namespace CLxx
{
   /// A device is a collection of compute units. A command-queue is used to queue
   /// commands to a device. Examples of commands include executing kernels, or reading and writing
   /// memory objects. OpenCL devices typically correspond to a GPU, a multi-core CPU, and other
   /// processors such as DSPs and the Cell/B.E. processor.

   class Device
   {
      friend class Platform;
	  friend class Context;

      public:

	      typedef cl_device_id Handle;

		  const Handle& getHandle() const { return _handle; }

	      enum DeviceTypes
		  {
		     /// An OpenCL device that is the host processor. The host
             /// processor runs the OpenCL implementations and is a single or multi-core CPU
		     CPU = CL_DEVICE_TYPE_CPU,

			 /// An OpenCL device that is a GPU. By this we mean that
             /// the device can also be used to accelerate a 3D API such as OpenGL or DirectX.
			 GPU = CL_DEVICE_TYPE_GPU,

			 /// Dedicated OpenCL accelerators (for example the IBM
             /// CELL Blade). These devices communicate with the host
             /// processor using a peripheral interconnect such as PCIe.
			 ACCELERATOR = CL_DEVICE_TYPE_ACCELERATOR,

			 /// The default OpenCL device in the system
			 DEFAULT = CL_DEVICE_TYPE_DEFAULT
		  };

		  typedef int DeviceType;

		  /// The OpenCL device type
		  DeviceType getDeviceType() const;

		  /// Device name 
		  std::string getName() const;

		  /// Vendor name
		  std::string getVendor() const;

		  /// A unique device vendor identifier. 
          /// An example of a unique device identifier could be the PCIe ID.
		  cl_uint getVendorId() const;

		  /// OpenCL version string. Returns the OpenCL version supported by the device. 
		  /// This version string has the following format:
          /// OpenCL<space><major_version.minor_version><space><vendor-specificinformation>
		  std::string getVersion() const;

		  /// OpenCL profile string. Returns the profile name supported by the device.
          /// The profile name returned can be one of the following strings
		  ///
		  /// FULL_PROFILE – if the device supports the OpenCL specification
          /// (functionality defined as part of the core specification and 
          /// does not require any extensions to be supported).
		  ///
          /// EMBEDDED_PROFILE - if the device
          /// supports the OpenCL embedded profile
		  std::string getProfile() const;

		  /// OpenCL software driver version string
          /// in the form major_number.minor_number
		  std::string getDriverVersion() const;

		  /// Returns a space separated list of extension names 
          /// (the extension names themselves do not contain any spaces).
		  std::string getExtensions() const;

		  // The number of parallel compute cores on the OpenCL device. The minimum value is 1.
		  cl_uint getMaxComputeUnits() const;

		  /// Maximum configured clock frequency of the device in MHz.
		  cl_uint getMaxClockFreq() const;

		  /// The default compute device address space size specified as an unsigned
          /// integer value in bits. Currently supported values are 32 or 64 bits
		  cl_uint getAddressBits() const;

		  /// Max size of memory object allocation in bytes. The minimum value is max
          /// (1/4th of GLOBAL_MEM_SIZE , 128*1024*1024)
		  cl_ulong getMaxMemAllocSize() const;

		  /// Is TRUE if images are supported
          /// by the OpenCL device and FALSE otherwise.
		  cl_bool isImageSupported() const;

		  /// Max number of simultaneous image objects that can be read by a kernel.
		  /// The minimum value is 128 if isImageSupported() is TRUE.
		  cl_uint getMaxReadImageArgs() const;

		  /// Max number of simultaneous image objects that can be written to by a
		  /// kernel. The minimum value is 8 if isImageSupported() is TRUE.
		  cl_uint getMaxWriteImageArgs() const;

		  /// Max width of 2D image in pixels. The minimum value is 8192 if
		  /// isImageSupported() is TRUE.
		  size_t getImage2dMaxWidth() const;

		  /// Max height of 2D image in pixels. The minimum value is 8192 if
		  /// isImageSupported() is TRUE.
		  size_t getImage2dMaxHeight() const;

		  /// Max width of 3D image in pixels. The minimum value is 2048 
		  /// if isImageSupported() is TRUE.
		  size_t getImage3dMaxWidth() const;

		  /// Max height of 3D image in pixels. The minimum value is 2048 
		  /// if isImageSupported() is TRUE.
		  size_t getImage3dMaxHeight() const;

		  /// Max depth of 3D image in pixels. The minimum value is 2048 
		  /// if isImageSupported() is TRUE.
		  size_t getImage3dMaxDepth() const;

		  /// Maximum number of samplers that can be used in a kernel. 
		  /// The minimum value is 16 if isImageSupported() is TRUE.
		  size_t getMaxSamplers() const;

		  /// Max size in bytes of the arguments that can be passed 
          /// to a kernel. The minimum value is 256.
		  size_t getMaxParamSize() const;

	      enum DeviceFloatingPointCapabilityEnum
		  {
		     /// denorms are supported
		     DENORM = CL_FP_DENORM,

			 ///INF and quiet NaNs are supported
			 INF_NAN = CL_FP_INF_NAN,

			 /// round to nearest even rounding mode supported
			 ROUND_TO_NEAREST = CL_FP_ROUND_TO_NEAREST,
			 
			 /// round to zero rounding mode supported
			 ROUND_TO_ZERO = CL_FP_ROUND_TO_ZERO,
			 
			 /// round to +ve and –ve infinity rounding modes supported
			 ROUND_TO_INF = CL_FP_ROUND_TO_INF,

			 /// IEEE754-2008 fused multiply-add is supported
			 FMA = CL_FP_FMA
		  };

		  typedef int DeviceFloatingPointCapabilities;

		  /// Describes single precision floatingpoint
          /// capability of the device. This is
          /// a bit-field that describes one or more
          /// of the values of DeviceFloatingPointCapabilityEnum
		  DeviceFloatingPointCapabilities getDeviceSingleFloatingPointCapabilities() const;

		  //DeviceFloatingPointCapabilities getDeviceHalfFloatingPointCapabilities() const;

		  //DeviceFloatingPointCapabilities getDeviceDoubleFloatingPointCapabilities() const;

		 
	      enum GlobalMemoryCacheType
		  {
		     NONE = CL_NONE,
			 READ_ONLY_CACHE = CL_READ_ONLY_CACHE,
			 READ_WRITE_CACHE = CL_READ_WRITE_CACHE
		  };

		  /// Type of global memory cache supported
		  GlobalMemoryCacheType getGlobalMemoryCacheType() const;
		  
		  /// Describes the alignment in bits of the
          /// base address of any allocated memory object
		  cl_uint getMemBaseAddrAlign() const;

		  /// The smallest alignment in bytes which
          /// can be used for any data type
		  cl_uint getMinDataTypeAlignSize() const;

		  /// Size of global memory cache line in bytes
		  cl_uint getGlobalMemCachelineSize() const;

		  /// Size of global memory cache in bytes
		  cl_ulong getGlobalMemCacheSize() const;

		  /// Size of global device memory in bytes
		  cl_ulong getGlobalMemSize() const;

		  /// Max size in bytes of a constant buffer
          /// allocation. The minimum value is 64KB
		  cl_ulong getMaxConstBufferSize() const;

		  /// Max number of arguments declared
          /// with the __constant qualifier in a
          /// kernel. The minimum value is 8
		  cl_uint getMaxConstArgs() const;


	      enum LocalMemoryType
		  {
			 LOCAL = CL_LOCAL,
			 GLOBAL = CL_GLOBAL
		  };

		  /// Type of local memory supported.
		  /// This can be set to LOCAL implying
          /// dedicated local memory storage such
          /// as SRAM, or GLOBAL
		  LocalMemoryType getLocalMemoryType() const;

		  /// Size of local memory arena in bytes.
          /// The minimum value is 16 KB.
		  cl_ulong getLocalMemSize() const;

		  /// Is TRUE if the device implements
          /// error correction for the memories,
          /// caches, registers etc. in the device. 
          /// Is FALSE if the device does not
          /// implement error correction. This can
          /// be a requirement for certain clients of OpenCL.
		  bool isErrorCorrectionSupported() const;

		  /// Describes the resolution of device
          /// timer. This is measured in nanoseconds
		  size_t getProfilingTimerResolution() const;

		  /// Is TRUE if the OpenCL device is a
          /// little endian device and FALSE otherwise
		  bool isEndianLittle() const;

		  /// Is TRUE if the device is available
          /// and FALSE if the device is not available
		  bool isAvailable() const;

		  /// Is FALSE if the implementation
          /// does not have a compiler available to
          /// compile the program source.
          /// Is TRUE if the compiler is available.
          /// This can be FALSE for the
          /// embededed platform profile only.
		  bool isCompilerAvailable() const;


	      enum ExecutionCapabilitiesEnum
		  {
		     /// The OpenCL device can execute OpenCL kernels
		     EXEC_KERNEL = CL_EXEC_KERNEL,

			 /// The OpenCL device can execute native kernels
			 EXEC_NATIVE_KERNEL = CL_EXEC_NATIVE_KERNEL
		  };

		  typedef int ExecutionCapabilities;

		  /// Describes the execution capabilities of
          /// the device. This is a bit-field that
          /// describes one or more of the values of ExecutionCapabilitiesEnum
		  ExecutionCapabilities getExecutionCapabilities() const;

	      enum CommandQueuePropertiesEnum
		  {
		     QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE = CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE,
			 QUEUE_PROFILING_ENABLE = CL_QUEUE_PROFILING_ENABLE
		  };

          typedef int CommandQueueProperties;

		  /// Describes the command-queue
          /// properties supported by the device.
          /// This is a bit-field that describes one or
          /// more of the values of CommandQueueProperties
		  CommandQueueProperties getCommandQueueProperties() const;

		  /// Maximum dimensions that specify the
          /// global and local work-item IDs used
          /// by the data parallel execution model

		  size_t getMaxWorkItemDimensions() const;

		  /// Maximum number of work-items that can be specified in each dimension of
          /// the work-group to NDRangeCommand 
		  /// Returns n size_t entries, where n is the value returned by the query for
		  /// getMaxWorkItemDimensions(). 
		  /// The minimum value is (1, 1, 1).

		  void getMaxWorkItemSize(std::vector<size_t>& items) const;

		  /// Maximum number of work-items in a
          /// work-group executing a kernel using
          /// the data parallel execution model
          /// The minimum value is 1

		  size_t getMaxWorkGroupSize() const;

		  /// Preferred native vector width size for
          /// built-in scalar types that can be put
          /// into vectors. The vector width is
          /// defined as the number of scalar
          /// elements that can be stored in the vector.

		  cl_uint getPreferredVectorWidthChar() const;
		  cl_uint getPreferredVectorWidthShort() const;
		  cl_uint getPreferredVectorWidthInt() const;
		  cl_uint getPreferredVectorWidthLong() const;
		  cl_uint getPreferredVectorWidthFloat() const;
		  cl_uint getPreferredVectorWidthDouble() const;


		  /// The platform associated with this device
		  boost::shared_ptr<Platform>& getPlatform();

      protected:

		Device(Handle h);

	    boost::shared_ptr<Platform> _platform;

	    Handle _handle;

   };
};


#endif
