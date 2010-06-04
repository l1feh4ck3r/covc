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

#include "voxelcolorer.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include <limits.h>

#include <math.h>

VoxelColorer::VoxelColorer()
    :width(0), height(0),
    number_of_images(0),
    number_of_last_added_image(0),
    threshold(0.1)

{
    memset(dimensions, 0, sizeof(dimensions));
    memset(camera_calibration_matrix, 0, sizeof(camera_calibration_matrix));
    memset(bounding_box, 0, sizeof(bounding_box));
}


VoxelColorer::~VoxelColorer()
{

}

void VoxelColorer::add_image(const unsigned char * image, size_t _width, size_t _height, const float * image_calibration_matrix)
{
    width = _width;
    height = _height;

    for (size_t h = 0; h < height; ++h)
    {
        for (size_t w = 0; w < width; ++w)
        {
            pixels.data()[number_of_last_added_image*width*height + h*width + w*4] = image[h*width + w*4];
            pixels.data()[number_of_last_added_image*width*height + h*width + w*4 + 1] = image[h*width + w*4 + 1];
            pixels.data()[number_of_last_added_image*width*height + h*width + w*4 + 2] = image[h*width + w*4 + 2];
            pixels.data()[number_of_last_added_image*width*height + h*width + w*4 + 3] = image[h*width + w*4 + 3];
        }
    }

    for (size_t i = 0; i < 16; ++i)
        image_calibration_matrices[number_of_last_added_image][i] = image_calibration_matrix[i];

    calculate_projection_matrix();

    number_of_last_added_image++;
}

///////////////////////////////////////////////////////////////////////////////
//! Build voxel model from seqence of images and matrices
///////////////////////////////////////////////////////////////////////////////
bool VoxelColorer::build_voxel_model()
{
    cl::Program ocl_program;
    cl::Kernel  ocl_kernel;

    unsigned int number_of_consistent_hypotheses;

    calculate_bounding_box();

    ///////////////////////////////////////////////////////////////////////////////
    //! Create buffers
    ///////////////////////////////////////////////////////////////////////////////

    // create opencl buffer for images
    cl::Image3D images_buffer(ocl_context,
                              CL_MEM_READ_ONLY,
                              cl::ImageFormat(CL_ARGB, CL_UNSIGNED_INT8),
                              width,
                              height,
                              number_of_images,
                              width*4*sizeof(unsigned char),
                              width*height*4*sizeof(unsigned char),
                              pixels.data());

    cl::Buffer dimensions_buffer (ocl_context,
                                  CL_MEM_READ_ONLY,
                                  3*sizeof(size_t));

    // create opencl buffer for projection matricies
    cl::Buffer projection_matrices_buffer(ocl_context,
                                          CL_MEM_READ_ONLY,
                                          number_of_images*16*sizeof(float));

    // create opencl buffer for hypotheses
    cl::Buffer hypotheses_buffer(ocl_context,
                                 CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                                 dimensions[0]*dimensions[1]*dimensions[2]*
                                 (2*sizeof(unsigned char)+number_of_images*3*sizeof(unsigned char)));

    cl::Buffer number_of_consistent_hypotheses_buffer(ocl_context,
                                                      CL_MEM_READ_WRITE,
                                                      sizeof(unsigned int));

    // create opencl buffer for bounding box
    cl::Buffer bounding_box_buffer (ocl_context, CL_MEM_READ_ONLY, sizeof(bounding_box));

    // create opencl buffer for resulting voxel model
    cl::Buffer voxel_model_buffer (ocl_context,
                                   CL_MEM_WRITE_ONLY,
                                   dimensions[0]*dimensions[1]*dimensions[2]*3*sizeof(unsigned char));
    ///////////////////////////////////////////////////////////////////////////////
    //! end of create buffers
    ///////////////////////////////////////////////////////////////////////////////

    std::vector<cl::Device> devices = ocl_context.getInfo<CL_CONTEXT_DEVICES>();
    cl::CommandQueue ocl_command_queue(ocl_context, devices[0]);

    ///////////////////////////////////////////////////////////////////////////////
    //! step 1: for each voxel build variety of hypotheses
    //! HYPOTHESIS EXTRACTION
    ///////////////////////////////////////////////////////////////////////////////
    if (!build_program(ocl_program, "ocl/step_1_build_variety_of_hypotheses.cl"))
        return false;

    cl::Kernel ocl_kernel_step_1 = cl::Kernel(ocl_program, "build_variety_of_hypotheses");
    ocl_kernel_step_1.setArg(0, bounding_box_buffer);
    ocl_kernel_step_1.setArg(1, images_buffer);
    ocl_kernel_step_1.setArg(2, projection_matrices_buffer);
    ocl_kernel_step_1.setArg(3, hypotheses_buffer);
    ocl_kernel_step_1.setArg(4, dimensions_buffer);
    ocl_kernel_step_1.setArg(5, number_of_images);

    ocl_command_queue.enqueueWriteBuffer(bounding_box_buffer,
                                         CL_TRUE,
                                         0,
                                         sizeof(bounding_box),
                                         bounding_box);

    ocl_command_queue.enqueueWriteBuffer(projection_matrices_buffer,
                                         CL_TRUE,
                                         0,
                                         number_of_images*16*sizeof(float),
                                         projection_matrices.data());

    ocl_command_queue.enqueueWriteBuffer(dimensions_buffer,
                                         CL_TRUE,
                                         0,
                                         3*sizeof(size_t),
                                         dimensions);

    cl::KernelFunctor func_step_1 = ocl_kernel_step_1.bind(ocl_command_queue,
                                                           cl::NDRange(dimensions[0], dimensions[1], dimensions[2]),
                                                           cl::NDRange(1, 1, 1));

    func_step_1().wait();

    ocl_command_queue.finish();

    ///////////////////////////////////////////////////////////////////////////////
    //! step 2: initial inconsistent voxels rejection
    //! CONSISTENCY CHECK AND HYPOTHESIS REMOVAL
    ///////////////////////////////////////////////////////////////////////////////
    if (!build_program(ocl_program, "ocl/step_2_initial_inconsistent_voxels_rejection_by_hypotheses.cl"))
        return false;

    cl::Kernel ocl_kernel_step_2 = cl::Kernel(ocl_program, "initial_inconsistent_voxels_rejection");

    for (size_t x = 0; x < dimensions[0]; ++x)
    {
        for (size_t y = 0; y < dimensions[1]; ++y)
        {
            for (size_t z = 0; z < dimensions[2]; ++z)
            {
                // offset to hypothesis for voxel with coordinates [x][y][z]
                ocl_kernel_step_2.setArg(0, hypotheses_buffer);
                ocl_kernel_step_2.setArg(1, x);
                ocl_kernel_step_2.setArg(2, y);
                ocl_kernel_step_2.setArg(3, z);
                ocl_kernel_step_2.setArg(4, dimensions_buffer);
                ocl_kernel_step_2.setArg(5, threshold);

                cl::KernelFunctor func_step_2 = ocl_kernel_step_2.bind(ocl_command_queue,
                                                                       cl::NDRange(number_of_images),
                                                                       cl::NDRange(1));

                func_step_2().wait();
            }
        }
    }

    ocl_command_queue.finish();

    // calculate number of consistent hypotheses for each voxel
    // if number of consistent hypotheses for one voxel is zero,
    // make this voxel transparent
    if (!build_program(ocl_program, "ocl/step_2_3_calculate_number_of_consistent_hypotheses_by_voxels.cl"))
        return false;
    cl::Kernel ocl_kernel_step_2_3_first = cl::Kernel(ocl_program, "calculate_number_of_consistent_hypotheses_by_voxels");
    ocl_kernel_step_2_3_first.setArg(0, hypotheses_buffer);
    ocl_kernel_step_2_3_first.setArg(1, dimensions_buffer);
    ocl_kernel_step_2_3_first.setArg(2, number_of_images);

    cl::KernelFunctor func_step_2_3_first = ocl_kernel_step_2_3_first.bind(ocl_command_queue,
                                                                           cl::NDRange(dimensions[0], dimensions[1], dimensions[2]),
                                                                           cl::NDRange(1, 1, 1));

    func_step_2_3_first().wait();
    ocl_command_queue.finish();

    if (!build_program(ocl_program, "ocl/step_2_3_calculate_number_of_consistent_hypotheses.cl"))
        return false;
    cl::Kernel ocl_kernel_step_2_3_second = cl::Kernel(ocl_program, "calculate_number_of_consistent_hypotheses");
    ocl_kernel_step_2_3_second.setArg(0, hypotheses_buffer);
    ocl_kernel_step_2_3_second.setArg(1, dimensions_buffer);
    ocl_kernel_step_2_3_second.setArg(2, number_of_images);
    ocl_kernel_step_2_3_second.setArg(3, number_of_consistent_hypotheses_buffer);

    cl::KernelFunctor func_step_2_3_second = ocl_kernel_step_2_3_second.bind(ocl_command_queue,
                                                                             cl::NDRange(1),
                                                                             cl::NDRange(1));
    func_step_2_3_second().wait();
    ocl_command_queue.finish();

    ocl_command_queue.enqueueReadBuffer(number_of_consistent_hypotheses_buffer,
                                        CL_TRUE,
                                        0,
                                        sizeof(unsigned int),
                                        &number_of_consistent_hypotheses);

    ocl_command_queue.finish();

    //threshold = const_cast<float>(number_of_consistent_hypotheses)/const_cast<float>(dimensions[0]*dimensions[1]*dimensions[2]*number_of_images);

    ///////////////////////////////////////////////////////////////////////////////
    //! step 3: inconsistent hypotheses rejection. visibility buffer in use
    ///////////////////////////////////////////////////////////////////////////////

    if (!build_program(ocl_program, "ocl/step_3_inconsistent_voxels_rejection.cl"))
        return false;

    cl::Kernel ocl_kernel_step_3 = cl::Kernel(ocl_program, "inconsistent_voxel_rejection");

    // create opencl buffer for z buffer
    // z buffer element contain only one value: free or occupied
    cl::Buffer * z_buffer = new cl::Buffer(ocl_context,
                                           CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                                           width*height*number_of_images*sizeof(unsigned char));

    unsigned int old_number_of_consistent_hypotheses = UINT_MAX;

    while (number_of_consistent_hypotheses != old_number_of_consistent_hypotheses)
    {
        old_number_of_consistent_hypotheses = number_of_consistent_hypotheses;

        for (size_t x = 0; x < dimensions[0]; ++x)
        {
            for (size_t y = 0; y < dimensions[1]; ++y)
            {
                for (size_t z = 0; z < dimensions[2]; ++z)
                {
                    ocl_kernel_step_3.setArg(0, hypotheses_buffer);
                    ocl_kernel_step_3.setArg(1, x);
                    ocl_kernel_step_3.setArg(2, y);
                    ocl_kernel_step_3.setArg(3, z);
                    ocl_kernel_step_3.setArg(4, bounding_box_buffer);
                    ocl_kernel_step_3.setArg(5, dimensions_buffer);
                    ocl_kernel_step_3.setArg(6, *z_buffer);
                    ocl_kernel_step_3.setArg(7, projection_matrices_buffer);
                    ocl_kernel_step_3.setArg(8, threshold);

                    cl::KernelFunctor func_step_3 = ocl_kernel_step_3.bind(ocl_command_queue,
                                                                           cl::NDRange(number_of_images),
                                                                           cl::NDRange(1));

                    func_step_3().wait();
                }
            }
        }

        ocl_command_queue.finish();

        func_step_2_3_first().wait();
        ocl_command_queue.finish();

        func_step_2_3_second().wait();
        ocl_command_queue.finish();

        ocl_command_queue.enqueueReadBuffer(number_of_consistent_hypotheses_buffer,
                                            CL_TRUE,
                                            0,
                                            sizeof(unsigned int),
                                            &number_of_consistent_hypotheses);
        ocl_command_queue.finish();

        // remove and create new z buffer
        delete z_buffer;
        z_buffer = new cl::Buffer(ocl_context,
                                  CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                                  width*height*number_of_images*sizeof(unsigned char));
    }

    delete z_buffer;
    z_buffer = NULL;

    ///////////////////////////////////////////////////////////////////////////////
    //! step 4: build voxel model from variety of hypotheses
    ///////////////////////////////////////////////////////////////////////////////

    if (!build_program(ocl_program, "ocl/step_4_build_voxel_model_from_variety_of_hypotheses.cl"))
        return false;

    cl::Kernel ocl_kernel_step_4 = cl::Kernel(ocl_program, "build_voxel_model");
    ocl_kernel_step_4.setArg(0, hypotheses_buffer);
    ocl_kernel_step_4.setArg(1, voxel_model_buffer);
    ocl_kernel_step_4.setArg(2, dimensions_buffer);
    ocl_kernel_step_4.setArg(3, number_of_images);

    cl::KernelFunctor func_step_4 = ocl_kernel_step_4.bind(ocl_command_queue,
                                                           cl::NDRange(dimensions[0], dimensions[1], dimensions[2]),
                                                           cl::NDRange(1, 1, 1));

    func_step_4().wait();
    ocl_command_queue.finish();

    ocl_command_queue.enqueueReadBuffer(voxel_model_buffer,
                                        CL_TRUE,
                                        0,
                                        dimensions[0]*dimensions[1]*dimensions[2]*3*sizeof(unsigned char),
                                        voxel_model.data());

    return true;
}

///////////////////////////////////////////////////////////////////////////////
//! Build the opencl program.
//!
//! @param path to file with opencl program
///////////////////////////////////////////////////////////////////////////////
bool VoxelColorer::build_program(cl::Program & program, const std::string & path_to_file_with_program)
{
    std::ifstream file(path_to_file_with_program.data());

    if ( !file )
    {
        std::cerr << "COVC: Error while opening " << path_to_file_with_program << std::endl;
        return false;
    }

    std::stringstream ss;
    ss << file.rdbuf();
    file.close();

    bool result = false;

    try
    {
        std::vector<cl::Device> devices = ocl_context.getInfo<CL_CONTEXT_DEVICES>();

        std::string src(ss.str());

        cl::Program::Sources source(1, std::make_pair(src.c_str(), src.length()));

        program = cl::Program(ocl_context, source);
        program.build(devices, "-cl-mad-enable");

        result = true;
    }
    catch(cl::Error ex)
    {
        std::cerr << "COVC: " << ex.what() << "(" << ex.error_code() << ": " << ex.error() << "):" << std::endl;
        result = false;
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
//! Calculate bounding box
///////////////////////////////////////////////////////////////////////////////
void VoxelColorer::calculate_bounding_box()
{
    float center3d[4];
    float image_center[4] = {255.0f, 255.0f, 1.0f, 1.0f};

    float inverted_camera_calibration_matrix[16];
    inverse(camera_calibration_matrix, inverted_camera_calibration_matrix);

    for (size_t i = 0; i < number_of_images; ++i)
    {
        float temp_vector[4];
        multiply_matrix_vector(inverted_camera_calibration_matrix, image_center, temp_vector);

        float inverted_image_calibration_matrix[16];
        inverse(image_calibration_matrices[i].data(), inverted_image_calibration_matrix);

        float result_pos[4];
        multiply_matrix_vector(inverted_image_calibration_matrix, temp_vector, result_pos);

        center3d[0] += result_pos[0] / static_cast<float>(number_of_images);
        center3d[1] += result_pos[1] / static_cast<float>(number_of_images);
        center3d[2] += result_pos[2] / static_cast<float>(number_of_images);
        center3d[3] = 1.0f;
    }

    float max_x = -10000.0f;
    float max_y = -10000.0f;
    float max_z = -10000.0f;

    for (size_t i = 0; i < number_of_images; ++i)
    {
        float inverted_image_calibration_matrix[16];
        inverse(image_calibration_matrices[i].data(), inverted_image_calibration_matrix);

        float camera_pos[4] = {0.0f, 0.0f, 0.0f, 1.0f};
        multiply_matrix_vector(inverted_image_calibration_matrix, camera_pos, camera_pos);

        //left top
        float left_top_pos[4] = {0.0f, 0.0f, 1.0f, 1.0f};

        multiply_matrix_vector(inverted_camera_calibration_matrix, left_top_pos, left_top_pos);
        left_top_pos[3] = 1.0f;
        multiply_matrix_vector(inverted_image_calibration_matrix, left_top_pos, left_top_pos);

        //right top
        float right_top_pos[4] = {511.0f, 0.0f, 1.0f, 1.0f};

        multiply_matrix_vector(inverted_camera_calibration_matrix, right_top_pos, right_top_pos);
        right_top_pos[3] = 1.0f;
        multiply_matrix_vector(inverted_image_calibration_matrix, right_top_pos, right_top_pos);

        // left bottom
        float left_bottom_pos[4] = {0.0f, 511.0f, 1.0f, 1.0f};

        multiply_matrix_vector(inverted_camera_calibration_matrix, left_bottom_pos, left_bottom_pos);
        left_bottom_pos[3] = 1.0f;
        multiply_matrix_vector(inverted_image_calibration_matrix, left_bottom_pos, left_bottom_pos);

        // right bottom
        float right_bottom_pos[4] = {511.0f, 511.0f, 1.0f, 1.0f};

        multiply_matrix_vector(inverted_camera_calibration_matrix, right_bottom_pos, right_bottom_pos);
        right_bottom_pos[3] = 1.0f;
        multiply_matrix_vector(inverted_image_calibration_matrix, right_bottom_pos, right_bottom_pos);

        float distance[4];
        vector_minus_vector(camera_pos, center3d, distance);

        float koef[4];
        normalize_vector(distance, koef);

        float projlt[4];
        vector_minus_vector(left_top_pos, camera_pos, projlt);
        multiply_vector_vector(projlt, koef, projlt);
        vector_plus_vector(projlt, camera_pos, projlt);

        float projlb[4];
        vector_minus_vector(left_bottom_pos, camera_pos, projlb);
        multiply_vector_vector(projlb, koef, projlb);
        vector_plus_vector(projlb, camera_pos, projlb);

        float projrt[4];
        vector_minus_vector(right_top_pos, camera_pos, projrt);
        multiply_vector_vector(projrt, koef, projrt);
        vector_plus_vector(projrt, camera_pos, projrt);

        float projrb[4];
        vector_minus_vector(right_bottom_pos, camera_pos, projrb);
        multiply_vector_vector(projrb, koef, projrb);
        vector_plus_vector(projrb, camera_pos, projrb);

        if (projlt[0] > max_x)
            max_x = projlt[0];
        if (projlt[1] > max_y)
            max_y = projlt[1];
        if (projlt[2] > max_z)
            max_z = projlt[2];

        if (projlb[0] > max_x)
            max_x = projlb[0];
        if (projlb[1] > max_y)
            max_y = projlb[1];
        if (projlb[2] > max_z)
            max_z = projlb[2];

        if (projrt[0] > max_x)
            max_x = projrt[0];
        if (projrt[1] > max_y)
            max_y = projrt[1];
        if (projrt[2] > max_z)
            max_z = projrt[2];

        if (projrb[0] > max_x)
            max_x = projrb[0];
        if (projrb[1] > max_y)
            max_y = projrb[1];
        if (projrb[2] > max_z)
            max_z = projrb[2];
    }

    float half_width = max_x - center3d[0];
    float half_height = max_y - center3d[1];
    float half_depth = max_z - center3d[2];

    bounding_box[0] = center3d[0] - half_width;
    bounding_box[1] = center3d[1] - half_height;
    bounding_box[2] = center3d[2] - half_depth;
    bounding_box[3] = half_width*2.0f;
    bounding_box[4] = half_height*2.0f;
    bounding_box[5] = half_depth*2.0f;
}


void VoxelColorer::calculate_projection_matrix()
{
    float * image_calibration_matrix = image_calibration_matrices[number_of_last_added_image].data();
    float unit_matrix[16] = {1.0f, 0.0f, 0.0f, 0.0f,
                             0.0f, 1.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 1.0f, 0.0f,
                             0.0f, 0.0f, 0.0f, 1.0f };

    // multiply camera calibration matrix to unit matrix
    float temp_matrix[16];
    for (size_t i=0; i < 4; i++)
        for (size_t j=0; j < 4; j++)
        {
        temp_matrix[i*4 + j] = 0.0f;
        for (size_t k=0; k < 4; ++k)
            temp_matrix[i*4 + j] += camera_calibration_matrix[i*4 + k] * unit_matrix[k*4 + j];
    }

    // multiply temp matrix to image calibration matrix
    for (size_t i=0; i < 4; i++)
        for (size_t j=0; j < 4; j++)
        {
        projection_matrices.at(number_of_last_added_image)[i*4 + j] = 0.0f;
        for (size_t k=0; k < 4; ++k)
            projection_matrices.at(number_of_last_added_image)[i*4 + j] += temp_matrix[i*4 + k] * image_calibration_matrix[k*4 + j];
    }
}

void VoxelColorer::inverse(const float *matrix, float *inverted_matrix)
{
    float a0 = matrix[ 0]*matrix[ 5] - matrix[ 1]*matrix[ 4];
    float a1 = matrix[ 0]*matrix[ 6] - matrix[ 2]*matrix[ 4];
    float a2 = matrix[ 0]*matrix[ 7] - matrix[ 3]*matrix[ 4];
    float a3 = matrix[ 1]*matrix[ 6] - matrix[ 2]*matrix[ 5];
    float a4 = matrix[ 1]*matrix[ 7] - matrix[ 3]*matrix[ 5];
    float a5 = matrix[ 2]*matrix[ 7] - matrix[ 3]*matrix[ 6];
    float b0 = matrix[ 8]*matrix[13] - matrix[ 9]*matrix[12];
    float b1 = matrix[ 8]*matrix[14] - matrix[10]*matrix[12];
    float b2 = matrix[ 8]*matrix[15] - matrix[11]*matrix[12];
    float b3 = matrix[ 9]*matrix[14] - matrix[10]*matrix[13];
    float b4 = matrix[ 9]*matrix[15] - matrix[11]*matrix[13];
    float b5 = matrix[10]*matrix[15] - matrix[11]*matrix[14];

    float det = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
    float epsilon = 0.1;
    if (fabs(det) > epsilon)
    {
        float inverse[16];

        inverse[ 0] = + matrix[ 5]*b5 - matrix[ 6]*b4 + matrix[ 7]*b3;
        inverse[ 4] = - matrix[ 4]*b5 + matrix[ 6]*b2 - matrix[ 7]*b1;
        inverse[ 8] = + matrix[ 4]*b4 - matrix[ 5]*b2 + matrix[ 7]*b0;
        inverse[12] = - matrix[ 4]*b3 + matrix[ 5]*b1 - matrix[ 6]*b0;
        inverse[ 1] = - matrix[ 1]*b5 + matrix[ 2]*b4 - matrix[ 3]*b3;
        inverse[ 5] = + matrix[ 0]*b5 - matrix[ 2]*b2 + matrix[ 3]*b1;
        inverse[ 9] = - matrix[ 0]*b4 + matrix[ 1]*b2 - matrix[ 3]*b0;
        inverse[13] = + matrix[ 0]*b3 - matrix[ 1]*b1 + matrix[ 2]*b0;
        inverse[ 2] = + matrix[13]*a5 - matrix[14]*a4 + matrix[15]*a3;
        inverse[ 6] = - matrix[12]*a5 + matrix[14]*a2 - matrix[15]*a1;
        inverse[10] = + matrix[12]*a4 - matrix[13]*a2 + matrix[15]*a0;
        inverse[14] = - matrix[12]*a3 + matrix[13]*a1 - matrix[14]*a0;
        inverse[ 3] = - matrix[ 9]*a5 + matrix[10]*a4 - matrix[11]*a3;
        inverse[ 7] = + matrix[ 8]*a5 - matrix[10]*a2 + matrix[11]*a1;
        inverse[11] = - matrix[ 8]*a4 + matrix[ 9]*a2 - matrix[11]*a0;
        inverse[15] = + matrix[ 8]*a3 - matrix[ 9]*a1 + matrix[10]*a0;

        float invDet = ((float)1)/det;
        inverse[ 0] *= invDet;
        inverse[ 1] *= invDet;
        inverse[ 2] *= invDet;
        inverse[ 3] *= invDet;
        inverse[ 4] *= invDet;
        inverse[ 5] *= invDet;
        inverse[ 6] *= invDet;
        inverse[ 7] *= invDet;
        inverse[ 8] *= invDet;
        inverse[ 9] *= invDet;
        inverse[10] *= invDet;
        inverse[11] *= invDet;
        inverse[12] *= invDet;
        inverse[13] *= invDet;
        inverse[14] *= invDet;
        inverse[15] *= invDet;

        for (size_t i = 0; i < 16; ++i)
            inverted_matrix[i] = inverse[i];
    }
}

void VoxelColorer::multiply_matrix_vector(const float *matrix, const float *vector, float *result)
{
    float temp[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    for (size_t i = 0; i < 4; ++i)
        for (size_t j = 0; j < 4; ++j)
            temp[i] += matrix[i*4 + j]*vector[j];

    for (size_t i = 0; i < 4; ++i)
        result[i] = temp[i];
}

void VoxelColorer::multiply_vector_vector(const float *vec1, const float *vec2, float *result)
{
    float temp[4];
    temp[0] = vec1[1]*vec2[2] - vec1[2]*vec2[1];
    temp[0] = vec1[2]*vec2[0] - vec1[0]*vec2[2];
    temp[0] = vec1[0]*vec2[1] - vec1[1]*vec2[0];

    for (size_t i = 0; i < 4; ++i)
        result[i] = temp[i];
}

void VoxelColorer::normalize_vector(const float *vector, float *result)
{
    float sum = 0.0f;
    for (size_t i = 0; i < 4; ++i)
        sum += vector[i]*vector[i];

    float radix = sqrt(sum);
    for (size_t i = 0; i < 4; ++i)
        result[i] = vector[i] / radix;
}

///////////////////////////////////////////////////////////////////////////////
//! Prepare voxel colorer
///////////////////////////////////////////////////////////////////////////////
bool VoxelColorer::prepare()
{
    if (!prepare_opencl())
        return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////
//! Prepare opencl
//!
//! @return true if opencl available and was prepared, false instread
///////////////////////////////////////////////////////////////////////////////
bool VoxelColorer::prepare_opencl()
{
    bool result = false;

    try
    {
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        cl_context_properties context_properties[] = {
            CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platforms[0]()),
            0
        };

        ocl_context = cl::Context(CL_DEVICE_TYPE_GPU, &context_properties[0]);

        result = true;
    }
    catch(cl::Error ex)
    {
        std::cerr << "COVC: " << ex.what() << "(" << ex.error_code() << ": " << ex.error() << ")" << std::endl;
        result = false;
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
//! Set camera calibration matrix
//!
//! @param _camera_calibration_matrix Pointer to camera calibration matrix
///////////////////////////////////////////////////////////////////////////////
void VoxelColorer::set_camera_calibration_matrix(const float *_camera_calibration_matrix)
{
    for (size_t i = 0; i < 16; ++i)
        camera_calibration_matrix[i] = _camera_calibration_matrix[i];
}

void VoxelColorer::set_number_of_images(size_t _number_of_images)
{
    number_of_images = _number_of_images;

    // resize buffers
    pixels.resize(number_of_images*512*512*4);
    image_calibration_matrices.resize(number_of_images, std::vector<float>(16));
    projection_matrices.resize(number_of_images, std::vector<float>(16));

    number_of_last_added_image = 0;
}

void VoxelColorer::set_resulting_voxel_cube_dimensions (size_t dimension_x, size_t dimension_y, size_t dimension_z)
{
    dimensions[0] = dimension_x;
    dimensions[1] = dimension_y;
    dimensions[2] = dimension_z;

    voxel_model.resize(dimensions[0]*dimensions[1]*dimensions[2]*3*sizeof(unsigned char));
}

void VoxelColorer::vector_minus_vector(const float *vec1, const float *vec2, float *result)
{
    float temp[4];
    for (size_t i = 0; i < 4; ++i)
        temp[i] = vec1[i] - vec2[i];

    for (size_t i = 0; i < 4; ++i)
        result[i] = temp[i];
}

void VoxelColorer::vector_plus_vector(const float *vec1, const float *vec2, float *result)
{
    float temp[4];
    for (size_t i = 0; i < 4; ++i)
        temp[i] = vec1[i] + vec2[i];

    for (size_t i = 0; i < 4; ++i)
        result[i] = temp[i];
}
