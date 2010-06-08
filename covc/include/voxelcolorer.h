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

#ifndef VOXELCOLORER_H
#define VOXELCOLORER_H

#define __CL_ENABLE_EXCEPTIONS

#include "cl.hpp"

class VoxelColorer
{
public:
    VoxelColorer();
    ~VoxelColorer();


public:
    void add_image(const unsigned char * image, size_t width, size_t height, const float * image_calibration_matrix);
    bool build_voxel_model();
    std::vector<unsigned char> & get_voxel_model() {return voxel_model;}
    bool prepare();

    // setters
    void set_camera_calibration_matrix(const float * _camera_calibration_matrix);
    void set_number_of_images(const size_t _number_of_images);
    void set_resulting_voxel_cube_dimensions(size_t dimension_x, size_t dimension_y, size_t dimension_z);

    // getters
    const cl::Context get_context () const  {return ocl_context;}

private:
    void build_program(cl::Program & program, const std::string & path_to_file_with_program);
    void calculate_bounding_box();
    void calculate_projection_matrix();
    void calculate_unprojection_matrices();
    bool prepare_opencl();

    void run_step_1(cl::Buffer & bounding_box_buffer,
                    cl::Buffer & projection_matrices_buffer,
                    cl::Buffer & hypotheses_buffer,
                    cl::Buffer & dimensions_buffer);

    void run_step_2(cl::Buffer & hypotheses_buffer,
                    cl::Buffer & dimensions_buffer,
                    cl::KernelFunctor & func_step_2_3_first,
                    cl::KernelFunctor & func_step_2_3_second,
                    cl::Buffer & number_of_consistent_hypotheses_buffer,
                    unsigned int * number_of_consistent_hypotheses);


    void build_step_2_3(cl::Buffer & hypotheses_buffer,
                        cl::Buffer & dimensions_buffer,
                        cl::Buffer & number_of_consistent_hypotheses_buffer,
                        cl::KernelFunctor & func_step_2_3_first,
                        cl::KernelFunctor & func_step_2_3_second);

    void build_clear_z_buffer(cl::Kernel & kernel);
    void clear_z_buffer(cl::Kernel & kernel, cl::Buffer & z_buffer);

    void run_step_3(cl::Buffer & hypotheses_buffer,
                    cl::Buffer & bounding_box_buffer,
                    cl::Buffer & dimensions_buffer,
                    cl::Buffer & projection_matrices_buffer,
                    cl::KernelFunctor & func_step_2_3_first,
                    cl::KernelFunctor & func_step_2_3_second,
                    cl::Buffer & number_of_consistent_hypotheses_buffer,
                    unsigned int * number_of_consistent_hypotheses);

    void run_step_4(cl::Buffer & hypotheses_buffer, cl::Buffer & dimensions_buffer);


private:
    cl::Context ocl_context;
    cl::CommandQueue ocl_command_queue;

    //! dimensions of resulting voxel cube by x, y, z
    size_t dimensions[3];

    //! rusulting voxel model. size = dimension[0]*dimension[1]*dimension[2]*4*size_of(color)
    std::vector<unsigned char> voxel_model;

    ///////////////////////////////////////////////////////////////////////////
    //! Info about images
    ///////////////////////////////////////////////////////////////////////////
    //! images. number of pixels = width*height*number_of_images*3*size_of(color)
    std::vector<unsigned char> pixels;

    //! image dimensions
    size_t width, height;

    //! number of images
    size_t number_of_images;

    // number of last added image
    size_t number_of_last_added_image;

    //! projection matrices for images
    //! size = number_of_images*16*size_of(float)
    std::vector<std::vector<float> > projection_matrices;
    std::vector<std::vector<float> > unprojection_matrices;

    std::vector<std::vector<float> > image_calibration_matrices;
    ///////////////////////////////////////////////////////////////////////////
    //! End of info about images
    ///////////////////////////////////////////////////////////////////////////

    float camera_calibration_matrix[16];

    //! bounding box. elements: pos_x, pos_y, pos_z, size_x, size_y, size_z;
    float bounding_box[6];

    //! hypotheses. size = dimension[0]*dimension[1]*dimension[2]*(4*sizeof(char) + number_of_images*4*size_of(color))
    // hypotheses:
    //  _
    // |_| - voxel visibility
    // |_| - number of consistent hypoteses on previous iteration
    // |_| - empty
    // |_| - empty
    // |_|-
    // |_| \
    // ...  - hypotheses
    // |_| /
    // |_|-
    //
    //std::vector<unsigned char> hypotheses;

    //! threshold.
    float threshold;
    float step_size;
    float precision;
};

#endif // VOXELCOLORER_H
