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

#ifndef BUFFERCOMMANDS_H
#define BUFFERCOMMANDS_H

#include "Command.h"
#include "ImageCommands.h"

namespace CLxx
{
	/// Base class for buffer commands 
	/// BufferCommand is abstract  
	class BufferCommand : public Command
	{

	public:

		BufferCommand(boost::shared_ptr<Buffer>& buffer, void* mem);

		/// get the buffer object 
        boost::shared_ptr<Buffer>& getBuffer() { return _buffer; }

		/// get the buffer object 
        const boost::shared_ptr<Buffer>& getBuffer() const { return _buffer; }

		/// Is the offset in bytes in the buffer object to read from or write to. 
		/// default value is 0
        size_t getOffset() const { return _offset; }

		/// Is the offset in bytes in the buffer object to read from or write to. 
		/// default value is 0
        size_t getMemSize() const { return _memSize; }
  
		/// get a pointer to buffer in host memory where data is to be read into or to be written from
        void* getHostMemoryPtr() const { return _hostMem; }

	protected:

        boost::shared_ptr<Buffer> _buffer;
        size_t _offset;
		size_t _memSize;
        void* _hostMem;

	};


	/// Read buffer operation.  
	///   
	/// Note : Enqueing ReadBufferCommand to read a region of the buffer object with the host_ptr argument value
    ///        set to host_ptr + offset, where host_ptr is a pointer to the memory region specified when the
    ///        buffer object being read is created with USE_HOST_PTR, must meet the following requirements 
	///        in order to avoid undefined behavior:
    ///        • All commands that use this buffer object have finished execution before the read command begins execution.
    ///        • The buffer object is not mapped.
    ///        • The buffer object is not used by any command-queue until the read command has finished execution.

	class ReadBufferCommand : public BufferCommand
	{

	public:
 
		/// buffer   : refers to a valid buffer object.
		/// host_ptr : is the pointer to buffer in host memory where data is to be read into
		/// blocking : if the read command is blocking, when enqued ReadBufferCommand does
        ///            not return until the buffer data has been read and copied into memory pointed to by host_ptr

		ReadBufferCommand(boost::shared_ptr<Buffer>& buffer, void* host_ptr, bool blocking = true);

		/// buffer   : refers to a valid buffer object.
		/// host_ptr : is the pointer to buffer in host memory where data is to be read from
		/// mem_size : is the size in bytes of data being read
		/// offset   : is the offset in bytes in the buffer object to read from
		/// blocking : if TRUE the read command is blocking, when enqued ReadBufferCommand does
        ///            not return until the buffer data has been read and copied into memory pointed to by host_ptr
		///          : if FALSE the read command is non-blocking, the command queue queues a non-blocking read command and returns.
        ///            The contents of the buffer that ptr points to cannot be used until the read command has completed. 
		///            getStatus() can be used to query the execution status of the read command.
        ///            When the read command has completed, the contents of the buffer that ptr points to can be used by the
        ///            application.
	
		ReadBufferCommand(boost::shared_ptr<Buffer>& buffer, void* host_ptr, size_t mem_size, size_t offset = 0 , bool blocking = true);
       
        virtual CommandType getCommandType() const { return READ_BUFFER; }
     
    protected:

         virtual void executePimp();       

	};


	/// Write buffer operation.  
	///   
	/// Note : Enqueing ReadBufferCommand to update the latest bits in a region of the buffer object with the
    ///        host_ptr argument value set to host_ptr + offset, where host_ptr is a pointer to the memory region
    ///        specified when the buffer object being written is created with USE_HOST_PTR, must meet the following 
	///        requirements in order to avoid undefined behavior:
    ///        • The host memory region given by (host_ptr + offset, mem_size) contains the latest bits when the enqueued write command begins execution.
    ///        • The buffer object is not mapped.
    ///        • The buffer object is not used by any command-queue until the write command has finished execution.

	class WriteBufferCommand : public BufferCommand
	{

	public:

		/// buffer   : refers to a valid buffer object.
		/// host_ptr : is the pointer to buffer in host memory where data is to be written into
		/// blocking : if the read command is blocking, when enqued ReadBufferCommand does
        ///            not return until the buffer data has been writen and copied into memory pointed to by host_ptr

		WriteBufferCommand(boost::shared_ptr<Buffer>& buffer, void* host_ptr, bool blocking = true);

		/// buffer   : refers to a valid buffer object.
		/// host_ptr : is the pointer to buffer in host memory where data is to be written into
		/// mem_size : is the size in bytes of data being written
		/// offset   : is the offset in bytes in the buffer object to write to
		/// blocking : if TRUE the write command is blocking, when enqued WriteBufferCommand does
        ///            not return until the buffer data has been written and copied from memory pointed to by host_ptr
		///            The memory pointed to by host_ptr can be reused by the application after the enque() returns
        ///            if FALSE, the OpenCL implementation will use host_ptr to perform a nonblocking write. 
		///            As the write is non-blocking the implementation can return immediately. 
		///            The memory pointed to by ptr cannot be reused by the application after the call returns. The event
		///            getStatus() can be used to query the execution status of the read command.
        ///            When the read command has completed, the contents of the buffer that ptr points to can be used by the
        ///            application.

		WriteBufferCommand(boost::shared_ptr<Buffer>& buffer, void* host_ptr, size_t mem_size, size_t offset = 0, bool blocking = true);
       
        virtual CommandType getCommandType() const { return WRITE_BUFFER; }
     
    protected:

         virtual void executePimp();       

	};

	/////
	/// CopyBufferCommand copies a buffer object identified by src_buffer to another buffer object identified by dst_buffer
	/////

	class CopyBufferCommand : public BufferCommand
	{

	public:

		/// src_buffer : refers to a valid buffer object to read from
		/// dst_buffer : refers to a valid buffer object to write to
		/// 
		/// Note : src_buffer is copied entirely. dst_buffer size has to be >= of src_buffer size. 
		CopyBufferCommand(boost::shared_ptr<Buffer>& src_buffer, boost::shared_ptr<Buffer>& dst_buffer);

		/// src_buffer : refers to a valid buffer object to read from
		/// dst_buffer : refers to a valid buffer object to write to
		/// mem_size   : refers to the size in bytes to copy
		/// src_offset : refers to the offset where to begin copying data from src_buffer.
		/// dst_offset : refers to the offset where to begin copying data into dst_buffer
		CopyBufferCommand(boost::shared_ptr<Buffer>& src_buffer, boost::shared_ptr<Buffer>& dst_buffer, size_t mem_size, size_t src_offset = 0, size_t dst_offset = 0 );
       
        virtual CommandType getCommandType() const { return COPY_BUFFER; }
     
        const boost::shared_ptr<Buffer>& getSourceBuffer() const { return getBuffer(); }
 	    
		const boost::shared_ptr<Buffer>& getDestinationBuffer() const { return _dstBuffer; }

		size_t getSourceOffset() const { return getOffset(); }

        size_t getDestinationOffset() const { return _dstOffset; }

    protected:

         virtual void executePimp();       

		 boost::shared_ptr<Buffer> _dstBuffer;
		 size_t _dstOffset;
	};

	/////
	/// Copy a buffer object to an image object
	/////
	class CopyBufferToImageCommand : public BufferCommand
	{

	public:

		/// buffer        : refers to a valid buffer object to read from
		/// image         : refers to a valid image object to write to
		/// buffer_offset : refers to the offset where to begin copying data from buffer
		/// image_area    : refers to ImageCommand::ImageArea for details
        ///
		/// The size in bytes of the ImageArea to be copied from buffer referred to as src_size is computed as
        /// width * height * depth * bytes/image element if image is a 3D image object and is computed
        /// as width * height * bytes/image element if image is a 2D image object.

        CopyBufferToImageCommand(boost::shared_ptr<Buffer> buffer,
                                 boost::shared_ptr<Image> image,
								 size_t buffer_offset = 0,
                                 boost::shared_ptr<ImageCommand::ImageArea> image_area = boost::shared_ptr<ImageCommand::ImageArea>());
       
        virtual CommandType getCommandType() const { return COPY_BUFFER_TO_IMAGE; }
     
        boost::shared_ptr<Image>& getImage() { return _image; }

		boost::shared_ptr<ImageCommand::ImageArea>& getImageArea() { return _imageArea; }

    protected:

         virtual void executePimp();       

		 boost::shared_ptr<Image> _image;
		 boost::shared_ptr<ImageCommand::ImageArea> _imageArea;
	};

	/////
	/// Map a region of the buffer object into the host address
	/// MapBufferCommand will hold a pointer to this mapped region... getHostMemoryPtr()
    /////
	class MapBufferCommand : public BufferCommand
	{

	public:

		enum Flag 
		{
			READ = CL_MAP_READ,  // region in the buffer object is being mapped for reading
			WRITE = CL_MAP_WRITE // region in the buffer object is being mapped for writing
		};
	
		typedef int Flags;

		/// buffer   : is a valid buffer object. The OpenCL context associated with CommandQueue and buffer must be the same.
		/// flags    : is a bit-field and can be set to READ to indicate that the region specified by
        ///            (offset, cb) in the buffer object is being mapped for reading, and/or WRITE to indicate
        ///            that the region specified by (offset, cb) in the buffer object is being mapped for writing.
        /// blocking : if TRUE, MapBufferCommand does not return until the specified region
        ///            in buffer can be mapped.
        ///            if FALSE i.e. map operation is non-blocking, the pointer to the mapped
        ///            region cannot be used until the map command has completed. 
		///            getStatus() can be used to query the execution status of the map command.
        ///            When the read command has completed, the contents of the buffer that ptr points to can be used by the
        ///            application.
        ///            When the map command is completed, the application can access
        ///            the contents of the mapped region using the pointer returned by getHostMemoryPtr().

		MapBufferCommand(boost::shared_ptr<Buffer>& buffer, Flags flags = READ | WRITE, bool blocking = true );

		/// buffer   : is a valid buffer object. The OpenCL context associated with CommandQueue and buffer must be the same.
		/// flags    : is a bit-field and can be set to READ to indicate that the region specified by
        ///            (offset, cb) in the buffer object is being mapped for reading, and/or WRITE to indicate
        ///            that the region specified by (offset, cb) in the buffer object is being mapped for writing.
		/// mem_size : size in bytes of the region in the buffer object that is being mapped.
		/// offset   : offset in bytes of the region in the buffer object that is being mapped.
		/// blocking : if TRUE, MapBufferCommand does not return until the specified region
        ///            in buffer can be mapped.
        ///            if FALSE i.e. map operation is non-blocking, the pointer to the mapped
        ///            region cannot be used until the map command has completed. 
		///            getStatus() can be used to query the execution status of the map command.
        ///            When the read command has completed, the contents of the buffer that ptr points to can be used by the
        ///            application.
        ///            When the map command is completed, the application can access
        ///            the contents of the mapped region using the pointer returned by getHostMemoryPtr().

		MapBufferCommand(boost::shared_ptr<Buffer>& buffer, Flags flags, size_t mem_size, size_t offset = 0, bool blocking = true);
    
		// if destroyed the host memory will be unmapped
		~MapBufferCommand()
		{
			unmapBufferCommand();
		}

        virtual CommandType getCommandType() const { return MAP_BUFFER; }

		Flags getFlags() const { return _flags; }
     
    protected:

         virtual void executePimp();    

		 void unmapBufferCommand();

		 Flags _flags;

	};
}

#endif
