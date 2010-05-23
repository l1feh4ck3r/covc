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

#ifndef IMAGE_H
#define IMAGE_H

#include "Memory.h"

namespace CLxx
{
    /// An image object is used to represent a buffer that can be used as a texture
    /// or a frame-buffer. The elements of an image object are selected from a list of predefined image
    /// formats. The minimum number of elements in a memory object is one
    /// Elements of an image are stored in a format that is opaque to the user and cannot
    /// be directly accessed using a pointer. Built-in functions are
    /// provided by the OpenCL C programming language to allow a kernel to read from or write to an image
    class Image : public Memory
    {

    public:

        /// Specifies the number of channels and the channel layout i.e. the
        /// memory layout in which channels are stored in the image. 

        enum ChannelOrder
        {
            R = CL_R,
            A = CL_A,
            INTENSITY = CL_INTENSITY,
            LUMINANCE = CL_LUMINANCE,
            RG = CL_RG,
            RA = CL_RA,
            RGB = CL_RGB,
            RGBA = CL_RGBA,
            ARGB = CL_ARGB,
            BGRA = CL_BGRA
        };

        /// Describes the size of the channel data type

        enum ChannelDataType
        {

            SNORM_INT8 = CL_SNORM_INT8,               /// Each channel component is a normalized signed 8-bit integer value
            SNORM_INT16 = CL_SNORM_INT16,             /// Each channel component is a normalized signed 16-bit integer value
            UNORM_INT8 = CL_UNORM_INT8,               /// Each channel component is a normalized unsigned 8-bit integer value
            UNORM_INT16 = CL_UNORM_INT16,             /// Each channel component is a normalized unsigned 16-bit integer value
            UNORM_SHORT_565 = CL_UNORM_SHORT_565,     /// Represents a normalized 5-6-5 3-channel RGB image. The channel order must be RGB.
            UNORM_SHORT_555 = CL_UNORM_SHORT_555,     /// Represents a normalized x-5-5-5 4-channel xRGB image. The channel order must be RGB
            UNORM_INT_101010 = CL_UNORM_INT_101010,   /// Represents a normalized x-10-10-10 4-channel xRGB image. The channel order must be RGB
            SIGNED_INT8 = CL_SIGNED_INT8,             /// Each channel component is an unnormalized signed 8-bit integer value
            SIGNED_INT16 = CL_SIGNED_INT16,           /// Each channel component is an unnormalized signed 16-bit integer value
            SIGNED_INT32 = CL_SIGNED_INT32,           /// Each channel component is an unnormalized signed 32-bit integer value
            UNSIGNED_INT8 = CL_UNSIGNED_INT8,         /// Each channel component is an unnormalized unsigned 8-bit integer value
            UNSIGNED_INT16 = CL_UNSIGNED_INT16,       /// Each channel component is an unnormalized unsigned 16-bit integer value
            UNSIGNED_INT32 = CL_UNSIGNED_INT32,       /// Each channel component is an unnormalized unsigned 32-bit integer value
            HALF_FLOAT = CL_HALF_FLOAT,               /// Each channel component is a 16-bit half-float value
            FLOAT = CL_FLOAT                          /// Each channel component is a single precision floatingpoint value
		};


        struct ImageFormat : public cl_image_format
        {
            ImageFormat()
            {
                image_channel_order = RGBA;
                image_channel_data_type = FLOAT;
            }

            ImageFormat(ChannelOrder channelOrder, ChannelDataType channelDataType )
            {
                image_channel_order = channelOrder;
                image_channel_data_type = channelDataType;
            }

            void setChannelOrder( ChannelOrder channelOrder ) { image_channel_order = channelOrder; }
            ChannelOrder getChannelOrder() const { return (ChannelOrder) image_channel_order; }

            void setChannelDataType( ChannelDataType ch ) { image_channel_data_type = ch; }
            ChannelDataType getChannelDataType() const { return (ChannelDataType) image_channel_data_type; }
        };


    public:

        ImageFormat getImageFormat() const;
        size_t getElementSize() const;
        size_t getRowPitch() const;
        size_t getWidth() const;
        size_t getHeight() const;


    protected:


        Image(Handle h)
            : Memory(h){}

    };

    class Image2D : public Image
    {
    public:

        /// context         : is a valid OpenCL context on which the image object is to be created
        /// flags           : is a bit-field that is used to specify allocation and usage information about the image
        ///                   memory object being created and is described in Memory::FlagEnum
        /// image_format    : point to a structure that describes format properties of the image to be
        ///                   allocated. Refer to ImageFormat for a detailed description of the image format descriptor
        /// image_width,
        /// image_height    : are the width and height of the image in pixels. These must be
        ///                   values greater than or equal to 1
        /// image_row_pitch : is the scan-line pitch in bytes. This must be 0 if host_ptr is NULL and can be
        ///                   either 0 or >= image_width * size of element in bytes if host_ptr is not NULL. 
        ///                   If host_ptr is not NULL and image_row_pitch = 0, image_row_pitch is calculated
        ///                   as image_width * size of element in bytes. If image_row_pitch is not 0,
        ///                   it must be a multiple of the image element size in bytes
        /// host_ptr        : is a pointer to the image data that may already be allocated by the application.
        ///                   The size of the buffer that host_ptr points to must be >= image_row_pitch * image_height.
        ///                   The size of each element in bytes must be a power of 2. The image data specified by host_ptr
        ///                   is stored as a linear sequence of adjacent scanlines.
        ///                   Each scanline is stored as a linear sequence of image elements

        static boost::shared_ptr<Image2D> createImage2D( boost::shared_ptr<Context>& context,
                                                         Flags flags,
                                                         size_t image_width,
                                                         size_t image_height,
                                                         const ImageFormat& image_format = ImageFormat(),
                                                         size_t image_row_pitch = 0,
                                                         void* host_ptr = NULL);

        /// get the list of image formats supported by an OpenCL implementation
        /// context  : is a valid OpenCL context on which the image object is to be created
        /// flags    : is a bit-field that is used to specify allocation and usage information about the image
        ///                   memory object being created and is described in Memory::FlagEnum
        /// formats  : list of supported image formats retunrd
        static void getSupportedImage2DFormats( boost::shared_ptr<Context>& context, Flags flags, std::vector<ImageFormat>& formats);

    protected:

        Image2D(Handle h): Image(h){}
    };

    class Image3D : public Image
    {

    public:

        /// context           : is a valid OpenCL context on which the image object is to be created
        /// flags             : is a bit-field that is used to specify allocation and usage information about the image
        ///                     memory object being created and is described in Memory::FlagEnum
        /// image_format      : point to a structure that describes format properties of the image to be
        ///                     allocated. Refer to ImageFormat for a detailed description of the image format descriptor
        /// image_width,
        /// image_height      : are the width and height of the image in pixels. These must be
        ///                     values greater than or equal to 1
        /// image_depth       : is the depth of the image in pixels. This must be a value > 1
        /// image_row_pitch   : is the scan-line pitch in bytes. This must be 0 if host_ptr is NULL and can be
        ///                     either 0 or >= image_width * size of element in bytes if host_ptr is not NULL. 
        ///                     If host_ptr is not NULL and image_row_pitch = 0, image_row_pitch is calculated
        ///                     as image_width * size of element in bytes. If image_row_pitch is not 0,
        ///                     it must be a multiple of the image element size in bytes
        /// image_slice_pitch : is the size in bytes of each 2D slice in the 3D image. This must be 0 if host_ptr is NULL
        ///                     and can be either 0 or >= image_row_pitch * image_height if host_ptr is not NULL.
        ///                     If host_ptr is not NULL and image_slice_pitch = 0, image_slice_pitch is calculated as
        ///                     image_row_pitch * image_height. If image_slice_pitch is not 0, it must be a multiple of the
        ///                     image_row_pitch
        /// host_ptr          : is a pointer to the image data that may already be allocated by the application. The size
        ///                     of the buffer that host_ptr points to must be >= image_slice_pitch * image_depth. The size of
        ///                     each element in bytes must be a power of 2. The image data specified by host_ptr is stored as a
        ///                     linear sequence of adjacent 2D slices. Each 2D slice is a linear sequence of adjacent scanlines.
        ///                     Each scanline is a linear sequence of image elements.

        static boost::shared_ptr<Image3D> createImage3D( boost::shared_ptr<Context>& context,
                                                         Flags flags,
                                                         size_t image_width,
                                                         size_t image_height,
                                                         size_t image_depth,
                                                         const ImageFormat& image_format = ImageFormat(),
                                                         size_t image_row_pitch = 0,
                                                         size_t image_slice_pitch = 0,
                                                         void* host_ptr = NULL);

        /// get the list of image formats supported by an OpenCL implementation
        /// context  : is a valid OpenCL context on which the image object is to be created
        /// flags    : is a bit-field that is used to specify allocation and usage information about the image
        ///                   memory object being created and is described in Memory::FlagEnum
        /// formats  : list of supported image formats retunrd
        static void getSupportedImage3DFormats( boost::shared_ptr<Context>& context, Flags flags, std::vector<ImageFormat>& formats );
	
    public:
        size_t getDepth() const;
        size_t getSlicePitch() const;

    protected:

        Image3D(Handle h): Image(h){}
    };

}

#endif
