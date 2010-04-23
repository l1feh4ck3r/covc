#include <iostream>
#include <jpeglib.h>

///////////////////////////////////////////////////////////////////////////////
//! Read jpeg file.
//! Based on code from
//! http://stackoverflow.com/questions/694080/how-do-i-read-jpeg-and-png-pixels-in-c-on-linux
//!
//! @param file_name Name of the jpeg image file
///////////////////////////////////////////////////////////////////////////////
int load_jpg(const std::string & file_name)
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
    unsigned char * test_data=raw_data;
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

    BMap = static_cast<int*>(test_data);
    Height = height;
    Width = width;
    Depht = 32;
}


int main(int argc, char * argv[])
{
    return 0;
}
