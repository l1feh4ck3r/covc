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

#ifndef IMAGECOMMANDS_H
#define IMAGECOMMANDS_H

#include "Command.h"

namespace CLxx
{
	// Base class for Image commands
	// ImageCommand is abstract
	class ImageCommand : public Command
	{

	public:

	    struct ImageArea
		{
		    size_t _origin[3];
			size_t _area[3];
			size_t _rowPitch;
			size_t _slicePitch;

			ImageArea()
			{
				_origin[0] = _origin[1] = _origin[2] = 0;
				_area[0] = _area[1] = _area[2] = 0;
				_rowPitch = _slicePitch = 0;
			}
		};

        const boost::shared_ptr<Image>& getImage() const { return _image; }

        const boost::shared_ptr<ImageArea>& getImageArea() const { return _imageArea; }

		void  setHostMemoryPtr(void* m) { _hostMem = m; }
  
        void* getHostMemoryPtr() const { return _hostMem; }

	protected:

		ImageCommand(boost::shared_ptr<Image> image, void* mem, bool blocking, boost::shared_ptr<ImageArea> area);

        boost::shared_ptr<Image> _image;
		boost::shared_ptr<ImageArea> _imageArea;
        void* _hostMem;

	};

	
	// Base class for 2D Image commands
	// Image2DCommand is abstract
	class Image2DCommand : public ImageCommand
	{
	public:

		struct Image2DArea : public ImageArea
		{
			/// set the (x, y) offset in pixels in the image from where to read or write.
			void setOrigin(size_t x, size_t y) { _origin[0] = x; _origin[1] = y; _origin[2] = 0;}

			/// get the (x, y) offset in pixels in the image from where to read or write.
			void getOrigin(size_t& x, size_t& y) const { x = _origin[0]; y = _origin[1]; }

			/// set the (width, height) in pixels of the 2D rectangle being read or written
			void setArea(size_t width, size_t height) { _area[0] = width; _area[1] = height; _area[2] = 0;}

			/// get the (width, height) in pixels of the 2D rectangle being read or written
			void getArea(size_t& width, size_t& height) const { width = _area[0]; height = _area[1]; }

			/// set the length of each row in bytes. This value must be greater than or equal 
            /// to the element size in bytes * width. If row_pitch is set to 0, the appropriate 
            /// row pitch is calculated based on the size of each element in bytes multiplied by width
			void setRowPitch( size_t row ) { _rowPitch = row; }

			/// get the length of each row in bytes
			size_t getRowPitch() const { return _rowPitch; }
		};

        //boost::shared_ptr<Image2D> getImage2D() const { return boost::static_pointer_cast<Image2D>(_image); }

        boost::shared_ptr<Image2DArea> getImage2DArea() const { return boost::static_pointer_cast<Image2DArea>(_imageArea); }

    protected:

		Image2DCommand(boost::shared_ptr<Image2D>& image, void* mem, bool blocking, boost::shared_ptr<Image2DArea>& area);

	};

	// Base class for 3D Image commands
	// Image3DCommand is abstract
	class Image3DCommand : public ImageCommand
	{
	public:

		struct Image3DArea : public ImageArea
		{
			/// set the (x, y, z) offset in pixels in the image from where to read or write.
			void setOrigin(size_t x, size_t y, size_t z) { _origin[0] = x; _origin[1] = y; _origin[2] = z;}

			/// get the (x, y, z) offset in pixels in the image from where to read or write.
			void getOrigin(size_t& x, size_t& y, size_t& z) const { x = _origin[0]; y = _origin[1]; z = _origin[2];}

			/// set the (width, height, depth) in pixels of the 3D rectangle being read or written
			void setArea(size_t width, size_t height, size_t depth) { _area[0] = width; _area[1] = height; _area[2] = depth;}

			/// get the (width, height, depth) in pixels of the 3D rectangle being read or written
			void getArea(size_t& width, size_t& height, size_t& depth) const { width = _area[0]; height = _area[1]; depth = _area[2];}

			/// set the length of each row in bytes. This value must be greater than or equal 
            /// to the element size in bytes * width. If row_pitch is set to 0, the appropriate 
            /// row pitch is calculated based on the size of each element in bytes multiplied by width
			void setRowPitch( size_t row ) { _rowPitch = row; }

			/// get the length of each row in bytes
			size_t getRowPitch() const { return _rowPitch; }

			/// set the size in bytes of the 2D slice of the 3D region of a 3D image 
			/// being read or written respectively. This must be 0 if image is a 2D image. 
			/// This value must be greater than or equal to row_pitch * height. 
			/// If slice_pitch is set to 0, the appropriate slice pitch is calculated based on the row_pitch * height
			void setSlicePitch( size_t row ) { _slicePitch = row; }

			/// get the size in bytes of the 2D slice of the 3D region of a 3D image 
			size_t getSlicePitch() const { return _slicePitch; }
		};

        //boost::shared_ptr<Image3D> getImage3D() const { return boost::static_pointer_cast<Image3D>(_image); }

        boost::shared_ptr<Image3DArea> getImage3DArea() const { return boost::static_pointer_cast<Image3DArea>(_imageArea); }

	protected:

		Image3DCommand(boost::shared_ptr<Image3D>& image, void* mem, bool blocking, boost::shared_ptr<Image3DArea>& area);

	};


	/// Commands to read from a 2D image object to host memory
	///
	/// Note : Enqueuing ReadImage2DCommand to read a region of the image object with the ptr argument value
    ///        set to host_ptr + (y * image row pitch + x * bytes per pixel), 
	///        where host_ptr is a pointer to the memory region specified when the image
    ///        object being read is created with USE_HOST_PTR, must meet the following
    ///        requirements in order to avoid undefined behavior:
    ///        • All commands that use this image object have finished execution before the read command begins execution.
    ///        • The row_pitch argument values in Image2DArea must be set to the image row pitch.
    ///        • The image object is not mapped.
	///        • The image object is not used by any command-queue until the read command has finished execution.
	class ReadImage2DCommand : public Image2DCommand
	{

	public:

		/// image    : refers to a valid 2D image object
		/// ptr      : is the pointer to a buffer in host memory where image data is to be read from
		/// blocking : If TRUE the command when enqued does not 
		///            return until the buffer data has been read and copied into memory pointed to by ptr
		///            If FALSE the command is non-blocking, the enque() function returns. 
		///            The contents of the buffer that ptr points to cannot be used until 
		///            the read command has completed. getStatus() can be used to query the execution status of the command
		/// area     : refere to Image2DArea for details
        ReadImage2DCommand(boost::shared_ptr<Image2D>& image, void* ptr, bool blocking = true, boost::shared_ptr<Image2DArea> area = boost::shared_ptr<Image2DArea>() );
       
        virtual CommandType getCommandType() const { return READ_IMAGE; }
     
    protected:

         virtual void executePimp();       

	};

	/// Commands to read from a 3D image object to host memory
	///
	/// Note : Enqueuing ReadImage3DCommand to read a region of the image object with the ptr argument value
    ///        set to host_ptr + (z * image slice pitch + y * image row pitch + x * bytes per pixel), 
	///        where host_ptr is a pointer to the memory region specified when the image
    ///        object being read is created with USE_HOST_PTR, must meet the following
    ///        requirements in order to avoid undefined behavior:
    ///        • All commands that use this image object have finished execution before the read command begins execution.
    ///        • The row_pitch and slice_pitch argument values in Image3DArea must be set to the image row pitch and slice pitch.
    ///        • The image object is not mapped.
	///        • The image object is not used by any command-queue until the read command has finished execution.
	class ReadImage3DCommand : public Image3DCommand
	{

	public:

		/// image    : refers to a valid 2D image object
		/// ptr      : is the pointer to a buffer in host memory where image data is to be read from
		/// blocking : If TRUE the command when enqued does not 
		///            return until the buffer data has been read and copied into memory pointed to by ptr
		///            If FALSE the command is non-blocking, the enque() function returns. 
		///            The contents of the buffer that ptr points to cannot be used until 
		///            the read command has completed. getStatus() can be used to query the execution status of the command
		/// area     : refere to Image3DArea for details
        ReadImage3DCommand(boost::shared_ptr<Image3D>& image, void* ptr, bool blocking = true, boost::shared_ptr<Image3DArea> area = boost::shared_ptr<Image3DArea>() );
       
        virtual CommandType getCommandType() const { return READ_IMAGE; }
     
    protected:

         virtual void executePimp();       
	};


	/// Commands to write to a 2D image object from host memory
	/// 
	/// Note : to update the latest bits in a region of the image object with the
    ///        ptr argument value set to host_ptr + (y * image row pitch + x * bytes per pixel),
    ///        where host_ptr is a pointer to the memory region specified when
    ///        the image object being written is created with USE_HOST_PTR, must meet the
    ///        following requirements in order to avoid undefined behavior:
    ///        • The host memory region being written contains the latest bits when the enqueued write
    ///          command begins execution.
    ///        • The input_row_pitch argument values in Image2DArea
    ///          must be set to the image row pitch and slice pitch.
    ///        • The image object is not mapped.
    ///        • The image object is not used by any command-queue until the write command has
    ///          finished execution.
	class WriteImage2DCommand : public Image2DCommand
	{

	public:

		/// image    : refers to a valid 2D image object
		/// ptr      : is the pointer to a buffer in host memory where image data is to be read from
		/// blocking : If TRUE the OpenCL implementation copies the data referred to by ptr
        ///            and enqueues the write command in the command-queue. The memory pointed to by ptr can be
		///            reused by the application after the enqueue() call returns
		///            If FALSE, the OpenCL implementation will use ptr to perform a nonblocking
        ///            write. As the write is non-blocking the implementation can return immediately. The
        ///            memory pointed to by ptr cannot be reused by the application after the call returns. 
		///            getStatus() can be used to query the execution status of the command. 
		///            When the write command has completed, the memory pointed to by ptr can then be reused by the application
		/// area     : refere to Image2DArea for details
        WriteImage2DCommand(boost::shared_ptr<Image2D>& image, void* ptr, bool blocking = true, boost::shared_ptr<Image2DArea> area = boost::shared_ptr<Image2DArea>() );
       
        virtual CommandType getCommandType() const { return WRITE_IMAGE; }
     
    protected:

         virtual void executePimp();       

	};

	/// Commands to write to a 3D image object from host memory
	///
	/// Note : to update the latest bits in a region of the image object with the
    ///        ptr argument value set to host_ptr + (z * image slice pitch + y * image row pitch + x * bytes per pixel),
    ///        where host_ptr is a pointer to the memory region specified when
    ///        the image object being written is created with USE_HOST_PTR, must meet the
    ///        following requirements in order to avoid undefined behavior:
    ///        • The host memory region being written contains the latest bits when the enqueued write
    ///          command begins execution.
    ///        • The input_row_pitch and input_slice_pitch argument values in Image3DArea
    ///          must be set to the image row pitch and slice pitch.
    ///        • The image object is not mapped.
    ///        • The image object is not used by any command-queue until the write command has
    ///          finished execution.
	class WriteImage3DCommand : public Image3DCommand
	{

	public:

		/// image    : refers to a valid 3D image object
		/// ptr      : is the pointer to a buffer in host memory where image data is to be read from
		/// blocking : If TRUE the OpenCL implementation copies the data referred to by ptr
        ///            and enqueues the write command in the command-queue. The memory pointed to by ptr can be
		///            reused by the application after the enqueue() call returns
		///            If FALSE, the OpenCL implementation will use ptr to perform a nonblocking
        ///            write. As the write is non-blocking the implementation can return immediately. The
        ///            memory pointed to by ptr cannot be reused by the application after the call returns. 
		///            getStatus() can be used to query the execution status of the command. 
		///            When the write command has completed, the memory pointed to by ptr can then be reused by the application
		/// area     : refere to Image3DArea for details
        WriteImage3DCommand(boost::shared_ptr<Image3D>& image, void* mem, bool blocking = true, boost::shared_ptr<Image3DArea> area = boost::shared_ptr<Image3DArea>() );
       
        virtual CommandType getCommandType() const { return WRITE_IMAGE; }
     
    protected:

         virtual void executePimp();       
	};


	/// command to map a region in the 3D image object given by image into the host address
    /// space and returns a pointer to this mapped region
	/// MapImage2DCommand will hold the pointer to this mapped region... getHostMemoryPtr()
	class MapImage2DCommand : public Image2DCommand
	{

	public:

		typedef int Flags;

		enum Flag {

			READ = CL_MAP_READ,
			WRITE = CL_MAP_WRITE
		};
	

		/// image    : is a valid image object. The OpenCL context associated with command_queue and image must be the same.
		/// flags    : is a bit-field and can be set to READ to indicate that the region specified by
        ///            (origin, region) in the image object is being mapped for reading, and/or WRITE to indicate 
        ///            that the region specified by (origin, region) in the image object is being mapped for writing
		/// blocking : If TRUE, when enqued MapImage3DCommand does not return until the specified region in image is mapped.
		///            If FALSE map operation is non-blocking, the pointer to the mapped region returned 
		///            cannot be used until the map command has completed.
		///            getStatus() can be used to query the execution status of the command.
		///            When the map command is completed, the application can access the
        ///            contents of the mapped region using the pointer returned by getHostMemoryPtr
		/// area     : refere to Image2DArea for details
        MapImage2DCommand(boost::shared_ptr<Image2D>& image, Flags flags = READ | WRITE, bool blocking = true, boost::shared_ptr<Image2DArea> area = boost::shared_ptr<Image2DArea>() );

		// if destroyed the host memory will be unmapped
		~MapImage2DCommand()
		{
			unmapImageCommand();
		}

        virtual CommandType getCommandType() const { return MAP_BUFFER; }

		Flags getFlags() const { return _flags; }
     
    protected:

		void unmapImageCommand();

         virtual void executePimp();    

		 Flags _flags;

	};


	/// command to map a region in the 3D image object given by image into the host address
    /// space and returns a pointer to this mapped region
	/// MapImage3DCommand will hold the pointer to this mapped region... getHostMemoryPtr()
	class MapImage3DCommand : public Image3DCommand
	{
	public:

		typedef int Flags;

		enum Flag {

			READ = CL_MAP_READ,
			WRITE = CL_MAP_WRITE
		};
	

		/// image    : is a valid image object. The OpenCL context associated with command_queue and image must be the same.
		/// flags    : is a bit-field and can be set to READ to indicate that the region specified by
        ///            (origin, region) in the image object is being mapped for reading, and/or WRITE to indicate 
        ///            that the region specified by (origin, region) in the image object is being mapped for writing
		/// blocking : If TRUE, when enqued MapImage3DCommand does not return until the specified region in image is mapped.
		///            If FALSE map operation is non-blocking, the pointer to the mapped region returned 
		///            cannot be used until the map command has completed.
		///            getStatus() can be used to query the execution status of the command.
		///            When the map command is completed, the application can access the
        ///            contents of the mapped region using the pointer returned by getHostMemoryPtr
		/// area     : refere to Image3DArea for details
        MapImage3DCommand(boost::shared_ptr<Image3D>& image, Flags flags = READ | WRITE, bool blocking = true, boost::shared_ptr<Image3DArea> area = boost::shared_ptr<Image3DArea>() );
       
		// if destroyed the host memory will be unmapped
		~MapImage3DCommand()
		{
			unmapImageCommand();
		}

        virtual CommandType getCommandType() const { return MAP_BUFFER; }

		Flags getFlags() const { return _flags; }
     
    protected:

         virtual void executePimp();    

		 void unmapImageCommand();

		 Flags _flags;
	};


	/// Command to copy image objects. src_image and dst_image can be 2D or 3D image
    /// objects allowing to perform the following actions:
	///
    ///   • Copy a 2D image object to a 2D image object.
    ///   • Copy a 2D image object to a 2D slice of a 3D image object.
    ///   • Copy a 2D slice of a 3D image object to a 2D image object.
    ///   • Copy a 3D image object to a 3D image object.
	///
	/// The OpenCL context associated with command_queue, src_image and dst_image must be the same
	class CopyImageCommand : public ImageCommand
	{

	public:

		CopyImageCommand(boost::shared_ptr<Image>& src_image, 
			             boost::shared_ptr<Image>& dst_image, 
                         boost::shared_ptr<ImageArea> src_area = boost::shared_ptr<ImageArea>(),
                         boost::shared_ptr<ImageArea> dst_area = boost::shared_ptr<ImageArea>());
       
        virtual CommandType getCommandType() const { return COPY_IMAGE; }
     
        const boost::shared_ptr<Image>& getSourceImage() const { return getImage(); }
 	    const boost::shared_ptr<Image>& getDestinationImage() const { return _dstImage; }

		const boost::shared_ptr<ImageArea>& getSourceImageArea() const { return getImageArea(); }
		const boost::shared_ptr<ImageArea>& getDestinationImageArea() const { return _dstImageArea; }

    protected:

         virtual void executePimp();       

		 boost::shared_ptr<Image> _dstImage;
		 boost::shared_ptr<ImageArea> _dstImageArea;
	};


	/// Command to copy an image object to a buffer object
	/// The OpenCL context associated with command_queue, src_image and dst_buffer must be the same
	class CopyImageToBufferCommand : public ImageCommand
	{

	public:

		CopyImageToBufferCommand(boost::shared_ptr<Image>& image, 
			                     boost::shared_ptr<Buffer>& buffer, 
                                 boost::shared_ptr<ImageArea> image_area = boost::shared_ptr<ImageArea>(),
						         size_t buffer_offset = 0);
       
        virtual CommandType getCommandType() const { return COPY_IMAGE_TO_BUFFER; }
     
        boost::shared_ptr<Buffer> getBuffer() const { return _buffer; }
        size_t getOffset() const { return _bufferOffset; }

    protected:

         virtual void executePimp();       

		 boost::shared_ptr<Buffer> _buffer;
		 size_t _bufferOffset;
	};


}

#endif
