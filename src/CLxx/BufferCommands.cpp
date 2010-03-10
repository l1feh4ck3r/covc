#include "BufferCommands.h"
#include "CommandQueue.h"
#include "Context.h"
#include "Buffer.h"
#include "Image.h"
#include "Exception.h"

namespace CLxx
{
    BufferCommand::BufferCommand(boost::shared_ptr<Buffer>& buffer, void* mem)
    {
        _buffer = buffer;
        _offset = 0;
        _hostMem = mem;

        if ( buffer.get() )
            _memSize = buffer->getMemSize();
    }

    ReadBufferCommand::ReadBufferCommand(boost::shared_ptr<Buffer>& buffer, void* mem , bool blocking)
        :BufferCommand(buffer,mem)
    {
        _isBlocking = blocking;
    }

    ReadBufferCommand::ReadBufferCommand(boost::shared_ptr<Buffer>& buffer, void* mem, size_t offset, size_t mem_size, bool blocking)
        :BufferCommand(buffer,mem)
    {
        _isBlocking = blocking;
        _offset = offset;
        _memSize = mem_size;
    }

    void ReadBufferCommand::executePimp()
    {
        cl_int ciErrNum;

        if ( _waitingListHandles.empty() )
            ciErrNum =  clEnqueueReadBuffer(_cmdQueue->getHandle(), _buffer->getHandle(), _isBlocking ? CL_TRUE : CL_FALSE,  _offset, _memSize, _hostMem, 0, NULL, &_handle );
        else
            ciErrNum =  clEnqueueReadBuffer(_cmdQueue->getHandle(), _buffer->getHandle(), _isBlocking ? CL_TRUE : CL_FALSE,  _offset, _memSize, _hostMem, _waitingListHandles.size(), &_waitingListHandles[0], &_handle );

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);

        _enquedSuccessfully = true;
    }






    WriteBufferCommand::WriteBufferCommand(boost::shared_ptr<Buffer>& buffer, void* mem , bool blocking)
        :BufferCommand(buffer,mem)
    {
        _isBlocking = blocking;
    }

    WriteBufferCommand::WriteBufferCommand(boost::shared_ptr<Buffer>& buffer, void* mem, size_t mem_size, size_t offset, bool blocking)
        :BufferCommand(buffer,mem)
    {
        _isBlocking = blocking;
        _offset = offset;
        _memSize = mem_size;
    }

    void WriteBufferCommand::executePimp()
    {
        cl_int ciErrNum;

        if ( _waitingListHandles.empty() )
            ciErrNum =  clEnqueueWriteBuffer(_cmdQueue->getHandle(), _buffer->getHandle(), _isBlocking ? CL_TRUE : CL_FALSE,  _offset, _memSize, _hostMem, 0, NULL, &_handle );
        else
            ciErrNum =  clEnqueueWriteBuffer(_cmdQueue->getHandle(), _buffer->getHandle(), _isBlocking ? CL_TRUE : CL_FALSE,  _offset, _memSize, _hostMem, _waitingListHandles.size(), &_waitingListHandles[0], &_handle );

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);

        _enquedSuccessfully = true;
    }




    CopyBufferCommand::CopyBufferCommand(boost::shared_ptr<Buffer>& src_buffer, boost::shared_ptr<Buffer>& dst_buffer)
        :BufferCommand(src_buffer,NULL)
    {
        _dstBuffer = dst_buffer;
        _dstOffset = _offset;
    }

    CopyBufferCommand::CopyBufferCommand(boost::shared_ptr<Buffer>& src_buffer, boost::shared_ptr<Buffer>& dst_buffer, size_t mem_size , size_t src_offset, size_t dst_offset)
        :BufferCommand(src_buffer,NULL)
    {
        _dstBuffer = dst_buffer;
        _offset = src_offset;
        _dstOffset = dst_offset;
        _memSize = mem_size;
    }

    void CopyBufferCommand::executePimp()
    {
        cl_int ciErrNum;

        if ( _waitingListHandles.empty() )
            ciErrNum =  clEnqueueCopyBuffer(_cmdQueue->getHandle(), _buffer->getHandle(), _dstBuffer->getHandle(),  _offset, _dstOffset, _memSize, 0, NULL, &_handle );
        else
            ciErrNum =  clEnqueueCopyBuffer(_cmdQueue->getHandle(), _buffer->getHandle(), _dstBuffer->getHandle(),  _offset, _dstOffset, _memSize, _waitingListHandles.size(), &_waitingListHandles[0], &_handle );

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);

        _enquedSuccessfully = true;
    }


    CopyBufferToImageCommand::CopyBufferToImageCommand(boost::shared_ptr<Buffer> buffer,
                                                       boost::shared_ptr<Image> image,
                                                       size_t buffer_offset,
                                                       boost::shared_ptr<ImageCommand::ImageArea> image_area)
                                                           :BufferCommand(buffer,NULL)
    {
        _offset = buffer_offset;
        _image = image;
        _imageArea = image_area;
    }

    void CopyBufferToImageCommand::executePimp()
    {
        cl_int ciErrNum;

        if ( _waitingListHandles.empty() )
            ciErrNum =  clEnqueueCopyBufferToImage(_cmdQueue->getHandle(),
                                                   _buffer->getHandle(),
                                                   _image->getHandle(),
                                                   _offset,
                                                   _imageArea->_origin,
                                                   _imageArea->_area,
                                                   0,
                                                   NULL,
                                                   &_handle );
        else
            ciErrNum =  clEnqueueCopyBufferToImage(_cmdQueue->getHandle(),
                                                   _buffer->getHandle(),
                                                   _image->getHandle(),
                                                   _offset,
                                                   _imageArea->_origin,
                                                   _imageArea->_area,
                                                   _waitingListHandles.size(),
                                                   &_waitingListHandles[0],
                                                   &_handle );

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);

        _enquedSuccessfully = true;
    }


    MapBufferCommand::MapBufferCommand(boost::shared_ptr<Buffer>& buffer, Flags flags, bool blocking)
        :BufferCommand(buffer,NULL)
    {
        _isBlocking = blocking;
        _flags = flags;
    }

    MapBufferCommand::MapBufferCommand(boost::shared_ptr<Buffer>& buffer, Flags flags, size_t mem_size, size_t offset, bool blocking)
        :BufferCommand(buffer,NULL)
    {
        _isBlocking = blocking;
        _flags = flags;
        _offset = offset;
        _memSize = mem_size;
    }

    void MapBufferCommand::executePimp()
    {
        cl_int ciErrNum;

        if ( _waitingListHandles.empty() )
            _hostMem =  clEnqueueMapBuffer(_cmdQueue->getHandle(),
                                           _buffer->getHandle(),
                                           _isBlocking ? CL_TRUE : CL_FALSE,
                                           _flags,
                                           _offset,
                                           _memSize,
                                           0,
                                           NULL,
                                           &_handle,
                                           &ciErrNum);
        else
            _hostMem =  clEnqueueMapBuffer(_cmdQueue->getHandle(),
                                           _buffer->getHandle(),
                                           _isBlocking ? CL_TRUE : CL_FALSE,
                                           _flags,
                                           _offset,
                                           _memSize,
                                           _waitingListHandles.size(),
                                           &_waitingListHandles[0],
                                           &_handle,
                                           &ciErrNum);

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);

        _enquedSuccessfully = true;
    }

    void MapBufferCommand::unmapBufferCommand()
    {
        cl_int ciErrNum;

        if ( _waitingListHandles.empty() )
            ciErrNum =  clEnqueueUnmapMemObject(_cmdQueue->getHandle(),
                                                _buffer->getHandle(),
                                                getHostMemoryPtr(),
                                                0,
                                                NULL,
                                                &_handle);
        else
            ciErrNum =  clEnqueueUnmapMemObject(_cmdQueue->getHandle(),
                                                _buffer->getHandle(),
                                                getHostMemoryPtr(),
                                                _waitingListHandles.size(),
                                                &_waitingListHandles[0],
                                                &_handle);

        if(ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);
    }

}
