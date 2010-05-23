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

#include "boost/shared_ptr.hpp"

namespace CLxx
{
    class Context;
    class Program;
}

namespace math
{
    template <class T> class matrix;
}

class VoxelColorer
{
public:
    VoxelColorer();
    ~VoxelColorer();


public:
    void add_image(const char * image, int width, int height, const math::matrix<float> & image_calibration_matrix);
    void build_voxel_model();
    bool prepare();

    // setters
    void set_camera_calibration_matrix(const math::matrix<float> & _camera_calibration_matrix);
    void set_resulting_voxel_cube_dimensions(unsigned int _dimension_x, unsigned int _dimension_y, unsigned int _dimension_z);

    // getters
    boost::shared_ptr<CLxx::Context> get_context () const  {return ocl_context;}

private:
    bool build_program(const std::string & path_to_file_with_program);
    void calculate_bounding_box();
    bool prepare_opencl();
    void run_program();
    void set_parameters_to_program();


private:
    boost::shared_ptr<CLxx::Context> ocl_context;
    boost::shared_ptr<CLxx::Program> ocl_program;


    // dimensions of resulting voxel cube
    unsigned int dimension_x, dimension_y, dimension_z;

    math::matrix<float> camera_calibration_matrix;
};

#endif // VOXELCOLORER_H
