/*
 * Copyright (c) 2010 Alexey 'l1feh4ck3r' Antonov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "pictureinfo.h"

#include <jpeglib.h>

PictureInfo::PictureInfo()
{

}

///////////////////////////////////////////////////////////////////////////////
//! Read jpeg file.
//! Based on code from
//! http://stackoverflow.com/questions/694080/how-do-i-read-jpeg-and-png-pixels-in-c-on-linux
//! which base on source code from jpeglib usage example
//!
//! @param file_name Name of the jpeg image file
///////////////////////////////////////////////////////////////////////////////
int PictureInfo::load_jpg(const std::string & file_name)
{
    unsigned char a,r,g,b;
    int width, height;
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    FILE * infile;        /* source file */
    JSAMPARRAY jpeg_buffer;       /* Output row buffer */
    int row_stride;       /* physical row width in output buffer */

    if ((infile = fopen(file_name.data(), "rb")) == NULL)
    {
        std::cerr << "Can't open " << file_name << std::endl;
        return 0;
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    (void) jpeg_read_header(&cinfo, true);
    (void) jpeg_start_decompress(&cinfo);
    width = cinfo.output_width;
    height = cinfo.output_height;

    unsigned char * raw_data = new unsigned char [width*height*4];
    unsigned char * test_data = raw_data;
    if (!raw_data)
    {
        std::cerr << "NO MEM FOR JPEG CONVERT!" << std::endl;
        return 0;
    }
    row_stride = width * cinfo.output_components ;
    jpeg_buffer = (*cinfo.mem->alloc_sarray)
                  ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

    while (cinfo.output_scanline < cinfo.output_height)
    {
        (void) jpeg_read_scanlines(&cinfo, jpeg_buffer, 1);
        for (int x=0; x < width; x++)
        {
            a = 0; // alpha value is not supported on jpg
            r = jpeg_buffer[0][cinfo.output_components*x];
            if (cinfo.output_components > 2)
            {
                g = jpeg_buffer[0][cinfo.output_components*x+1];
                b = jpeg_buffer[0][cinfo.output_components*x+2];
            }
            else
            {
                g = r;
                b = r;
            }
            *(raw_data++) = b;
            *(raw_data++) = g;
            *(raw_data++) = r;
            *(raw_data++) = a;
        }
    }

    fclose(infile);
    (void) jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

//    BMap = static_cast<int*>(test_data);
//    Height = height;
//    Width = width;
//    Depht = 32;
}
