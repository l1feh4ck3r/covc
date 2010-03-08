
#include "ImageCommands.h"
#include "CommandQueue.h"
#include "Context.h"
#include "Image.h"
#include "Buffer.h"
#include "Exception.h"

namespace CLxx
{
	ImageCommand::ImageCommand(boost::shared_ptr<Image> image, void* mem, bool blocking, boost::shared_ptr<ImageArea> area)
		:Command(blocking)
	{
		_image = image;
		_hostMem = mem;
		_imageArea = area;

		if ( !_imageArea.get() )
		{
			// TO DO
			_imageArea = boost::shared_ptr<ImageArea>(new ImageArea());
			_imageArea->_area[0] = image->getWidth() != 0 ? image->getMemSize() / image->getWidth() : 0;
			_imageArea->_area[1] = image->getHeight() != 0 ? image->getMemSize() / image->getHeight() : 0;
		    //_imageArea->_area[2] = image->getDe() != 0 ? image->getMemSize() / image->getHeight() : 0;
		}
	}

	Image2DCommand::Image2DCommand(boost::shared_ptr<Image2D>& image, void* mem, bool blocking, boost::shared_ptr<Image2DArea>& area)
		:ImageCommand(image,mem,blocking,area)
	{
	}

	Image3DCommand::Image3DCommand(boost::shared_ptr<Image3D>& image, void* mem, bool blocking, boost::shared_ptr<Image3DArea>& area)
		:ImageCommand(image,mem,blocking,area)
	{
	}

	ReadImage2DCommand::ReadImage2DCommand(boost::shared_ptr<Image2D>& image, void* mem, bool blocking, boost::shared_ptr<Image2DArea>& area)
		:Image2DCommand(image,mem,blocking,area)
	{
	}

	void ReadImage2DCommand::executePimp()
	{
		cl_int ciErrNum;

		if ( _waitingListHandles.empty() )
			ciErrNum =  clEnqueueReadImage(_cmdQueue->getHandle(), 
			                               _image->getHandle(), 
										   _isBlocking ? CL_TRUE : CL_FALSE,  
										   _imageArea->_origin,
										   _imageArea->_area,
										   _imageArea->_rowPitch,
										   _imageArea->_slicePitch,
										   _hostMem, 
										   0, 
										   NULL, 
										   &_handle );
		else
			ciErrNum =  clEnqueueReadImage(_cmdQueue->getHandle(), 
			                               _image->getHandle(), 
										   _isBlocking ? CL_TRUE : CL_FALSE,  
										   _imageArea->_origin,
										   _imageArea->_area,
										   _imageArea->_rowPitch,
										   _imageArea->_slicePitch,
										   _hostMem, 
										   _waitingListHandles.size(), 
										   &_waitingListHandles[0], 
										   &_handle );

		if(ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);

		_enquedSuccessfully = true;
	}

	ReadImage3DCommand::ReadImage3DCommand(boost::shared_ptr<Image3D>& image, void* mem, bool blocking, boost::shared_ptr<Image3DArea>& area)
		:Image3DCommand(image,mem,blocking,area)
	{
	}

	void ReadImage3DCommand::executePimp()
	{
		cl_int ciErrNum;

		if ( _waitingListHandles.empty() )
			ciErrNum =  clEnqueueReadImage(_cmdQueue->getHandle(), 
			                               _image->getHandle(), 
										   _isBlocking ? CL_TRUE : CL_FALSE,  
										   _imageArea->_origin,
										   _imageArea->_area,
										   _imageArea->_rowPitch,
										   _imageArea->_slicePitch,
										   _hostMem, 
										   0, 
										   NULL, 
										   &_handle );
		else
			ciErrNum =  clEnqueueReadImage(_cmdQueue->getHandle(), 
			                               _image->getHandle(), 
										   _isBlocking ? CL_TRUE : CL_FALSE,  
										   _imageArea->_origin,
										   _imageArea->_area,
										   _imageArea->_rowPitch,
										   _imageArea->_slicePitch,
										   _hostMem, 
										   _waitingListHandles.size(), 
										   &_waitingListHandles[0], 
										   &_handle );

		if(ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);

		_enquedSuccessfully = true;
	}

	WriteImage2DCommand::WriteImage2DCommand(boost::shared_ptr<Image2D>& image, void* mem, bool blocking, boost::shared_ptr<Image2DArea>& area)
		:Image2DCommand(image,mem,blocking,area)
	{
	}

	void WriteImage2DCommand::executePimp()
	{
		cl_int ciErrNum;

		if ( _waitingListHandles.empty() )
			ciErrNum =  clEnqueueWriteImage(_cmdQueue->getHandle(), 
			                               _image->getHandle(), 
										   _isBlocking ? CL_TRUE : CL_FALSE,  
										   _imageArea->_origin,
										   _imageArea->_area,
										   _imageArea->_rowPitch,
										   _imageArea->_slicePitch,
										   _hostMem, 
										   0, 
										   NULL, 
										   &_handle );
		else
			ciErrNum =  clEnqueueWriteImage(_cmdQueue->getHandle(), 
			                               _image->getHandle(), 
										   _isBlocking ? CL_TRUE : CL_FALSE,  
										   _imageArea->_origin,
										   _imageArea->_area,
										   _imageArea->_rowPitch,
										   _imageArea->_slicePitch,
										   _hostMem, 
										   _waitingListHandles.size(), 
										   &_waitingListHandles[0], 
										   &_handle );

		if(ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);

		_enquedSuccessfully = true;
	}


	WriteImage3DCommand::WriteImage3DCommand(boost::shared_ptr<Image3D>& image, void* mem, bool blocking, boost::shared_ptr<Image3DArea>& area)
		:Image3DCommand(image,mem,blocking,area)
	{
	}

	void WriteImage3DCommand::executePimp()
	{
		cl_int ciErrNum;

		if ( _waitingListHandles.empty() )
			ciErrNum =  clEnqueueWriteImage(_cmdQueue->getHandle(), 
			                               _image->getHandle(), 
										   _isBlocking ? CL_TRUE : CL_FALSE,  
										   _imageArea->_origin,
										   _imageArea->_area,
										   _imageArea->_rowPitch,
										   _imageArea->_slicePitch,
										   _hostMem, 
										   0, 
										   NULL, 
										   &_handle );
		else
			ciErrNum =  clEnqueueWriteImage(_cmdQueue->getHandle(), 
			                               _image->getHandle(), 
										   _isBlocking ? CL_TRUE : CL_FALSE,  
										   _imageArea->_origin,
										   _imageArea->_area,
										   _imageArea->_rowPitch,
										   _imageArea->_slicePitch,
										   _hostMem, 
										   _waitingListHandles.size(), 
										   &_waitingListHandles[0], 
										   &_handle );

		if(ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);

		_enquedSuccessfully = true;
	}


	MapImage2DCommand::MapImage2DCommand(boost::shared_ptr<Image2D>& image, Flags flags, bool blocking, boost::shared_ptr<Image2DArea>& area)
		:Image2DCommand(image,NULL,blocking,area)
	{
		_flags = flags;
	}

	void MapImage2DCommand::executePimp()
	{
		cl_int ciErrNum;

		if ( _waitingListHandles.empty() )
			_hostMem =  clEnqueueMapImage(_cmdQueue->getHandle(), 
			                               _image->getHandle(), 
										   _isBlocking ? CL_TRUE : CL_FALSE,  
										   _flags,
										   _imageArea->_origin,
										   _imageArea->_area,
										   &_imageArea->_rowPitch,
										   &_imageArea->_slicePitch,
										   0, 
										   NULL, 
										   &_handle,
										   &ciErrNum);
		else
			_hostMem =  clEnqueueMapImage(_cmdQueue->getHandle(), 
			                               _image->getHandle(), 
										   _isBlocking ? CL_TRUE : CL_FALSE,  
										   _flags,
										   _imageArea->_origin,
										   _imageArea->_area,
										   &_imageArea->_rowPitch,
										   &_imageArea->_slicePitch,
										   _waitingListHandles.size(), 
										   &_waitingListHandles[0], 
										   &_handle,
										   &ciErrNum);

		if(ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);

		_enquedSuccessfully = true;
	}

	void MapImage2DCommand::unmapImageCommand()
	{
		cl_int ciErrNum;

		if ( _waitingListHandles.empty() )
			ciErrNum =  clEnqueueUnmapMemObject(getCommandQueue()->getHandle(), 
			                                    getImage()->getHandle(), 
												getHostMemoryPtr(),  
											   0, 
											   NULL, 
											   &_handle);
		else
			ciErrNum =  clEnqueueUnmapMemObject(getCommandQueue()->getHandle(), 
			                                    getImage()->getHandle(), 
												getHostMemoryPtr(),  
										   _waitingListHandles.size(), 
   										   &_waitingListHandles[0], 
										   &_handle);

		if(ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);
	}

	MapImage3DCommand::MapImage3DCommand(boost::shared_ptr<Image3D>& image, Flags flags, bool blocking, boost::shared_ptr<Image3DArea>& area)
		:Image3DCommand(image,NULL,blocking,area)
	{
		_flags = flags;
	}

	void MapImage3DCommand::executePimp()
	{
		cl_int ciErrNum;

		if ( _waitingListHandles.empty() )
			_hostMem =  clEnqueueMapImage(_cmdQueue->getHandle(), 
			                               _image->getHandle(), 
										   _isBlocking ? CL_TRUE : CL_FALSE,  
										   _flags,
										   _imageArea->_origin,
										   _imageArea->_area,
										   &_imageArea->_rowPitch,
										   &_imageArea->_slicePitch,
										   0, 
										   NULL, 
										   &_handle,
										   &ciErrNum);
		else
			_hostMem =  clEnqueueMapImage(_cmdQueue->getHandle(), 
			                               _image->getHandle(), 
										   _isBlocking ? CL_TRUE : CL_FALSE,  
										   _flags,
										   _imageArea->_origin,
										   _imageArea->_area,
										   &_imageArea->_rowPitch,
										   &_imageArea->_slicePitch,
										   _waitingListHandles.size(), 
										   &_waitingListHandles[0], 
										   &_handle,
										   &ciErrNum);

		if(ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);

		_enquedSuccessfully = true;
	}

	void MapImage3DCommand::unmapImageCommand()
	{
		cl_int ciErrNum;

		if ( _waitingListHandles.empty() )
			ciErrNum =  clEnqueueUnmapMemObject(getCommandQueue()->getHandle(), 
			                                    getImage()->getHandle(), 
												getHostMemoryPtr(),  
											   0, 
											   NULL, 
											   &_handle);
		else
			ciErrNum =  clEnqueueUnmapMemObject(getCommandQueue()->getHandle(), 
			                                    getImage()->getHandle(), 
												getHostMemoryPtr(),  
										   _waitingListHandles.size(), 
   										   &_waitingListHandles[0], 
										   &_handle);

		if(ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);
	}


	CopyImageCommand::CopyImageCommand(boost::shared_ptr<Image>& src_image, boost::shared_ptr<Image>& dst_image, boost::shared_ptr<ImageArea>& src_area, boost::shared_ptr<ImageArea>& dst_area)
		:ImageCommand(src_image,NULL,true,src_area)
	{
		_dstImage = dst_image;
		_dstImageArea = dst_area;
	}

	void CopyImageCommand::executePimp()
	{
		cl_int ciErrNum;

		if ( _waitingListHandles.empty() )
			ciErrNum =  clEnqueueCopyImage(_cmdQueue->getHandle(), 
			                               _image->getHandle(), 
										   _dstImage->getHandle(),  
										   _imageArea->_origin,
										   _dstImageArea->_origin,
										   _imageArea->_area,
										   0, 
										   NULL, 
										   &_handle );
		else
			ciErrNum =  clEnqueueCopyImage(_cmdQueue->getHandle(), 
			                               _image->getHandle(), 
										   _dstImage->getHandle(),  
										   _imageArea->_origin,
										   _dstImageArea->_origin,
										   _imageArea->_area,
										   _waitingListHandles.size(), 
										   &_waitingListHandles[0], 
										   &_handle );

		if(ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);

		_enquedSuccessfully = true;
	}

	CopyImageToBufferCommand::CopyImageToBufferCommand(boost::shared_ptr<Image>& image, boost::shared_ptr<Buffer>& buffer, boost::shared_ptr<ImageCommand::ImageArea>& image_area, size_t buffer_offset)
		:ImageCommand(image,NULL,true,image_area)
	{
		_buffer = buffer;
		_bufferOffset = buffer_offset;
	}

	void CopyImageToBufferCommand::executePimp()
	{
		cl_int ciErrNum;

		if ( _waitingListHandles.empty() )
			ciErrNum =  clEnqueueCopyImageToBuffer(_cmdQueue->getHandle(), 
			                               _image->getHandle(), 
										   _buffer->getHandle(),  
										   _imageArea->_origin,
										   _imageArea->_area,
										   _bufferOffset,
										   0, 
										   NULL, 
										   &_handle );
		else
			ciErrNum =  clEnqueueCopyImageToBuffer(_cmdQueue->getHandle(), 
			                               _image->getHandle(), 
										   _buffer->getHandle(),  
										   _imageArea->_origin,
										   _imageArea->_area,
										   _bufferOffset,
										   _waitingListHandles.size(), 
										   &_waitingListHandles[0], 
										   &_handle );

		if(ciErrNum != CL_SUCCESS)
			throw Exception(ciErrNum);

		_enquedSuccessfully = true;
	}

}
