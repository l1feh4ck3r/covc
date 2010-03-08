
#include "Image.h"
#include "Context.h"
#include "Device.h"
#include "Host.h"
#include "Exception.h"

namespace CLxx
{
	boost::shared_ptr<Image2D> Image2D::createImage2D( boost::shared_ptr<Context>& context, Image::Flags flags, size_t image_width, size_t image_height, const Image::ImageFormat& image_format, size_t image_row_pitch, void* host_ptr)
	{
		cl_int errorCode;

		cl_mem memHandle = clCreateImage2D(context->getHandle(), flags, &image_format, image_width, image_height, image_row_pitch, host_ptr, &errorCode );

		if ( errorCode != CL_SUCCESS )
			throw Exception(errorCode);

		boost::shared_ptr<Image2D> image = boost::shared_ptr<Image2D>( new Image2D(memHandle) );
		image->_context = context;
		image->_memType = IMAGE2D;
		image->_flags = flags;

		//Host::add(memHandle,image);

		return image;
	}

	boost::shared_ptr<Image3D> Image3D::createImage3D( boost::shared_ptr<Context>& context, Flags flags, size_t image_width, size_t image_height, size_t image_depth, const Image::ImageFormat& image_format, size_t image_row_pitch,size_t image_slice_pitch,  void* host_ptr)
	{
		cl_int errorCode;

		cl_mem memHandle = clCreateImage3D(context->getHandle(), flags, &image_format, image_width, image_height, image_depth,image_row_pitch, image_slice_pitch, host_ptr, &errorCode );

		if ( errorCode != CL_SUCCESS )
			throw Exception(errorCode);

		boost::shared_ptr<Image3D> image = boost::shared_ptr<Image3D>( new Image3D(memHandle) );
		image->_context = context;
		image->_memType = IMAGE3D;
		image->_flags = flags;

		//Host::add(memHandle,image);

		return image;
	}

	void Image2D::getSupportedImage2DFormats( boost::shared_ptr<Context>& context, Flags flags, std::vector<Image::ImageFormat>& formats )
	{
		const unsigned max_formats = 256;
		cl_uint num_image_formats;
		ImageFormat forms[max_formats];

		cl_int errorCode = clGetSupportedImageFormats(context->getHandle(), flags, CL_MEM_OBJECT_IMAGE2D, max_formats, forms, &num_image_formats );

		if ( errorCode != CL_SUCCESS )
			throw Exception(errorCode);

		for(size_t i = 0; i < num_image_formats; i++)
			formats.push_back(forms[i]);
	}

	Image::ImageFormat Image::getImageFormat() const
	{
		ImageFormat value;
		cl_int ciErrNum =  clGetImageInfo(_handle, CL_IMAGE_FORMAT, sizeof(ImageFormat), &value, NULL);

		if(ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);

		return value;
	}


	size_t Image::getElementSize() const
	{
		size_t value;
		cl_int ciErrNum =  clGetMemObjectInfo(_handle, CL_IMAGE_ELEMENT_SIZE, sizeof(size_t), &value, NULL);

		if(ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);

		return value;
	}

	size_t Image::getRowPitch() const
	{
		size_t value;
		cl_int ciErrNum =  clGetMemObjectInfo(_handle, CL_IMAGE_ROW_PITCH, sizeof(size_t), &value, NULL);

		if(ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);

		return value;
	}

	size_t Image3D::getSlicePitch() const
	{
		size_t value;
		cl_int ciErrNum =  clGetMemObjectInfo(_handle, CL_IMAGE_SLICE_PITCH, sizeof(size_t), &value, NULL);

		if(ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);

		return value;
	}

	size_t Image::getWidth() const
	{
		size_t value;
		cl_int ciErrNum =  clGetMemObjectInfo(_handle, CL_IMAGE_WIDTH, sizeof(size_t), &value, NULL);

		if(ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);

		return value;
	}

	size_t Image::getHeight() const
	{
		size_t value;
		cl_int ciErrNum =  clGetMemObjectInfo(_handle, CL_IMAGE_HEIGHT, sizeof(size_t), &value, NULL);

		if(ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);

		return value;
	}

	size_t Image3D::getDepth() const
	{
		size_t value;
		cl_int ciErrNum =  clGetMemObjectInfo(_handle, CL_IMAGE_DEPTH, sizeof(size_t), &value, NULL);

		if(ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);

		return value;
	}

}
