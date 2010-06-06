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
#include "matrices_and_vectors.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include <limits.h>

#include <math.h>


VoxelColorer::VoxelColorer()
    :width(0), height(0),
    number_of_images(0),
    number_of_last_added_image(0),
    threshold(1.0f)
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

    for (size_t i = 0; i < width*height*4; ++i)
        pixels.data()[number_of_last_added_image*width*height + i] = image[i];

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
    unsigned int number_of_consistent_hypotheses = 0;

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
                                 (4*sizeof(unsigned char)+number_of_images*4*sizeof(unsigned char)));

    cl::Buffer number_of_consistent_hypotheses_buffer(ocl_context,
                                                      CL_MEM_READ_WRITE,
                                                      sizeof(unsigned int));

    // create opencl buffer for bounding box
    cl::Buffer bounding_box_buffer (ocl_context, CL_MEM_READ_ONLY, sizeof(bounding_box));

    ///////////////////////////////////////////////////////////////////////////////
    //! end of create buffers
    ///////////////////////////////////////////////////////////////////////////////

    std::vector<cl::Device> devices = ocl_context.getInfo<CL_CONTEXT_DEVICES>();
    ocl_command_queue = cl::CommandQueue(ocl_context, devices[0]);


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

    std::cout << "Total number of hypotheses = " << dimensions[0]*dimensions[1]*dimensions[2]*number_of_images << std::endl;

    run_step_1(bounding_box_buffer,
               images_buffer,
               projection_matrices_buffer,
               hypotheses_buffer,
               dimensions_buffer);

    cl::KernelFunctor step_2_3_1;
    cl::KernelFunctor step_2_3_2;
    build_step_2_3(hypotheses_buffer,
                   dimensions_buffer,
                   number_of_consistent_hypotheses_buffer,
                   step_2_3_1,
                   step_2_3_2);

    run_step_2(hypotheses_buffer,
               dimensions_buffer,
               step_2_3_1,
               step_2_3_2,
               number_of_consistent_hypotheses_buffer,
               &number_of_consistent_hypotheses);

    run_step_3(hypotheses_buffer,
               bounding_box_buffer,
               dimensions_buffer,
               projection_matrices_buffer,
               step_2_3_1,
               step_2_3_2,
               number_of_consistent_hypotheses_buffer,
               &number_of_consistent_hypotheses);

    run_step_4(hypotheses_buffer,
               dimensions_buffer);

    return true;
}

///////////////////////////////////////////////////////////////////////////////
//! Build the opencl program.
//!
//! @param path to file with opencl program
///////////////////////////////////////////////////////////////////////////////
void VoxelColorer::build_program(cl::Program & program, const std::string & path_to_file_with_program)
{
    std::ifstream file(path_to_file_with_program.data());

    if ( !file )
    {
        std::cerr << "COVC: Error while opening " << path_to_file_with_program << std::endl;
        throw std::exception();
    }

    std::stringstream ss;
    ss << file.rdbuf();
    file.close();

    bool result = false;

    std::vector<cl::Device> devices = ocl_context.getInfo<CL_CONTEXT_DEVICES>();

    std::string src(ss.str());

    cl::Program::Sources source(1, std::make_pair(src.c_str(), src.length()));

    program = cl::Program(ocl_context, source);
    program.build(devices, "-cl-mad-enable");

    return;
}

///////////////////////////////////////////////////////////////////////////////
//! Calculate bounding box
///////////////////////////////////////////////////////////////////////////////
void VoxelColorer::calculate_bounding_box()
{
    float center3d[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float image_center[4] = {255.0f, 255.0f, 1.0f, 1.0f};

    float inverted_camera_calibration_matrix[16] = {0.0f, 0.0f, 0.0f, 0.0f,
                                                    0.0f, 0.0f, 0.0f, 0.0f,
                                                    0.0f, 0.0f, 0.0f, 0.0f,
                                                    0.0f, 0.0f, 0.0f, 0.0f};
    inverse(camera_calibration_matrix, inverted_camera_calibration_matrix);

    for (size_t i = 0; i < number_of_images; ++i)
    {
        float inverted_image_calibration_matrix[16] = {0.0f, 0.0f, 0.0f, 0.0f,
                                                       0.0f, 0.0f, 0.0f, 0.0f,
                                                       0.0f, 0.0f, 0.0f, 0.0f,
                                                       0.0f, 0.0f, 0.0f, 0.0f};
        inverse(image_calibration_matrices[i].data(), inverted_image_calibration_matrix);

        float image_center_in_global_space[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        multiply_matrix_vector(inverted_camera_calibration_matrix, image_center, image_center_in_global_space);
        multiply_matrix_vector(inverted_image_calibration_matrix, image_center_in_global_space, image_center_in_global_space);

        center3d[0] += image_center_in_global_space[0] / static_cast<float>(number_of_images);
        center3d[1] += image_center_in_global_space[1] / static_cast<float>(number_of_images);
        center3d[2] += image_center_in_global_space[2] / static_cast<float>(number_of_images);
        center3d[3] += image_center_in_global_space[2] / static_cast<float>(number_of_images);
    }

    float max_x = -10000.0f;
    float max_y = -10000.0f;
    float max_z = -10000.0f;

    for (size_t i = 0; i < number_of_images; ++i)
    {
        float inverted_image_calibration_matrix[16] = {0.0f, 0.0f, 0.0f, 0.0f,
                                                       0.0f, 0.0f, 0.0f, 0.0f,
                                                       0.0f, 0.0f, 0.0f, 0.0f,
                                                       0.0f, 0.0f, 0.0f, 0.0f};

        inverse(image_calibration_matrices[i].data(), inverted_image_calibration_matrix);

        float camera_pos[4] = {0.0f, 0.0f, 0.0f, 1.0f};
        multiply_matrix_vector(inverted_image_calibration_matrix, camera_pos, camera_pos);

        //left top
        float left_top_pos[4] = {0.0f, 0.0f, 1.0f, 1.0f};
        multiply_matrix_vector(inverted_camera_calibration_matrix, left_top_pos, left_top_pos);
        multiply_matrix_vector(inverted_image_calibration_matrix, left_top_pos, left_top_pos);

        //right top
        float right_top_pos[4] = {511.0f, 0.0f, 1.0f, 1.0f};
        multiply_matrix_vector(inverted_camera_calibration_matrix, right_top_pos, right_top_pos);
        multiply_matrix_vector(inverted_image_calibration_matrix, right_top_pos, right_top_pos);

        // left bottom
        float left_bottom_pos[4] = {0.0f, 511.0f, 1.0f, 1.0f};
        multiply_matrix_vector(inverted_camera_calibration_matrix, left_bottom_pos, left_bottom_pos);
        multiply_matrix_vector(inverted_image_calibration_matrix, left_bottom_pos, left_bottom_pos);

        // right bottom
        float right_bottom_pos[4] = {511.0f, 511.0f, 1.0f, 1.0f};
        multiply_matrix_vector(inverted_camera_calibration_matrix, right_bottom_pos, right_bottom_pos);
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
    float temp_matrix[16] = {0.0f, 0.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 0.0f, 0.0f };

    for (size_t i=0; i < 4; i++)
        for (size_t j=0; j < 4; j++)
        {
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
//! step 1: for each voxel build variety of hypotheses
//! HYPOTHESIS EXTRACTION
///////////////////////////////////////////////////////////////////////////////
void VoxelColorer::run_step_1(cl::Buffer & bounding_box_buffer,
                              cl::Image3D & images_buffer,
                              cl::Buffer & projection_matrices_buffer,
                              cl::Buffer & hypotheses_buffer,
                              cl::Buffer & dimensions_buffer)
{
    cl::Program ocl_program;

    build_program(ocl_program, "ocl/step_1_build_variety_of_hypotheses.cl");

    cl::Kernel ocl_kernel_step_1 = cl::Kernel(ocl_program, "build_variety_of_hypotheses");
    ocl_kernel_step_1.setArg(0, bounding_box_buffer);
    ocl_kernel_step_1.setArg(1, images_buffer);
    ocl_kernel_step_1.setArg(2, projection_matrices_buffer);
    ocl_kernel_step_1.setArg(3, hypotheses_buffer);
    ocl_kernel_step_1.setArg(4, dimensions_buffer);
    ocl_kernel_step_1.setArg(5, number_of_images);

    cl::KernelFunctor func_step_1 = ocl_kernel_step_1.bind(ocl_command_queue,
                                                           cl::NDRange(dimensions[0], dimensions[1], dimensions[2]),
                                                           cl::NDRange(1, 1, 1));

    func_step_1().wait();

    ocl_command_queue.finish();

}

///////////////////////////////////////////////////////////////////////////////
//! step 2: initial inconsistent voxels rejection
//! CONSISTENCY CHECK AND HYPOTHESIS REMOVAL
///////////////////////////////////////////////////////////////////////////////
void VoxelColorer::run_step_2(cl::Buffer & hypotheses_buffer,
                              cl::Buffer & dimensions_buffer,
                              cl::KernelFunctor & func_step_2_3_first,
                              cl::KernelFunctor & func_step_2_3_second,
                              cl::Buffer & number_of_consistent_hypotheses_buffer,
                              unsigned int * number_of_consistent_hypotheses)
{
    cl::Program ocl_program;

    build_program(ocl_program, "ocl/step_2_initial_inconsistent_voxels_rejection_by_hypotheses.cl");

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

    func_step_2_3_first().wait();
    ocl_command_queue.finish();
    func_step_2_3_second().wait();
    ocl_command_queue.finish();

    ocl_command_queue.enqueueReadBuffer(number_of_consistent_hypotheses_buffer,
                                        CL_TRUE,
                                        0,
                                        sizeof(unsigned int),
                                        number_of_consistent_hypotheses);

    std::cout << "Number of consistent hypotheses = " << *number_of_consistent_hypotheses << std::endl;
    float float_number_of_consistent_hypotheses = *number_of_consistent_hypotheses;
    threshold = float_number_of_consistent_hypotheses/(float)(dimensions[0]*dimensions[1]*dimensions[2]*number_of_images*10000);
    std::cout << "threshold = " << threshold << std::endl;

}

void VoxelColorer::build_step_2_3(cl::Buffer & hypotheses_buffer,
                                  cl::Buffer & dimensions_buffer,
                                  cl::Buffer & number_of_consistent_hypotheses_buffer,
                                  cl::KernelFunctor & func_step_2_3_first,
                                  cl::KernelFunctor & func_step_2_3_second)
{
    cl::Program ocl_program;

    build_program(ocl_program, "ocl/step_2_3_calculate_number_of_consistent_hypotheses_by_voxels.cl");

    cl::Kernel ocl_kernel_step_2_3_first = cl::Kernel(ocl_program, "calculate_number_of_consistent_hypotheses_by_voxels");
    ocl_kernel_step_2_3_first.setArg(0, hypotheses_buffer);
    ocl_kernel_step_2_3_first.setArg(1, dimensions_buffer);
    ocl_kernel_step_2_3_first.setArg(2, number_of_images);

    func_step_2_3_first = ocl_kernel_step_2_3_first.bind(ocl_command_queue,
                                                         cl::NDRange(dimensions[0], dimensions[1], dimensions[2]),
                                                         cl::NDRange(1, 1, 1));

    build_program(ocl_program, "ocl/step_2_3_calculate_number_of_consistent_hypotheses.cl");

    cl::Kernel ocl_kernel_step_2_3_second = cl::Kernel(ocl_program, "calculate_number_of_consistent_hypotheses");
    ocl_kernel_step_2_3_second.setArg(0, hypotheses_buffer);
    ocl_kernel_step_2_3_second.setArg(1, dimensions_buffer);
    ocl_kernel_step_2_3_second.setArg(2, number_of_images);
    ocl_kernel_step_2_3_second.setArg(3, number_of_consistent_hypotheses_buffer);

    func_step_2_3_second = ocl_kernel_step_2_3_second.bind(ocl_command_queue,
                                                           cl::NDRange(1),
                                                           cl::NDRange(1));
}

void VoxelColorer::build_clear_z_buffer(cl::Kernel & kernel)
{
    cl::Program ocl_program;

    build_program(ocl_program, "ocl/step_3_clear_z_buffer.cl");

    kernel = cl::Kernel(ocl_program, "clear_z_buffer");
}

void VoxelColorer::clear_z_buffer(cl::Kernel &kernel, cl::Buffer *z_buffer)
{
    kernel.setArg(0, *z_buffer);
    cl::KernelFunctor func = kernel.bind(ocl_command_queue,
                                         cl::NDRange(number_of_images),
                                         cl::NDRange(1));
    func().wait();

    ocl_command_queue.finish();
}

///////////////////////////////////////////////////////////////////////////////
//! step 3: inconsistent hypotheses rejection. visibility buffer in use
///////////////////////////////////////////////////////////////////////////////
void VoxelColorer::run_step_3(cl::Buffer & hypotheses_buffer,
                              cl::Buffer & bounding_box_buffer,
                              cl::Buffer & dimensions_buffer,
                              cl::Buffer & projection_matrices_buffer,
                              cl::KernelFunctor & func_step_2_3_first,
                              cl::KernelFunctor & func_step_2_3_second,
                              cl::Buffer & number_of_consistent_hypotheses_buffer,
                              unsigned int * number_of_consistent_hypotheses)
{
    cl::Program ocl_program;

    build_program(ocl_program, "ocl/step_3_inconsistent_voxels_rejection.cl");

    cl::Kernel ocl_kernel_step_3 = cl::Kernel(ocl_program, "inconsistent_voxel_rejection");

    cl::Kernel clear_z_buffer_kernel;
    build_clear_z_buffer(clear_z_buffer_kernel);

    unsigned int old_number_of_consistent_hypotheses = UINT_MAX;

    while (*number_of_consistent_hypotheses != old_number_of_consistent_hypotheses)
    {
        old_number_of_consistent_hypotheses = *number_of_consistent_hypotheses;

        // create opencl buffer for z buffer
        // z buffer element contain only one value: free or occupied
        cl::Buffer * z_buffer = new cl::Buffer(ocl_context,
                                               CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                                               width*height*number_of_images*4*sizeof(unsigned char));

        // fill z buffer with non occupied values
        clear_z_buffer(clear_z_buffer_kernel, z_buffer);

        for (size_t current_image = 0; current_image < number_of_images; ++current_image)
        {
            float corners[8][4] = {{bounding_box[0],                   bounding_box[1],                   bounding_box[2],                   1.0f},
                                   {bounding_box[0] + bounding_box[3], bounding_box[1],                   bounding_box[2],                   1.0f},
                                   {bounding_box[0],                   bounding_box[1] + bounding_box[4], bounding_box[2],                   1.0f},
                                   {bounding_box[0] + bounding_box[3], bounding_box[1] + bounding_box[4], bounding_box[2],                   1.0f},
                                   {bounding_box[0],                   bounding_box[1],                   bounding_box[2] + bounding_box[5], 1.0f},
                                   {bounding_box[0] + bounding_box[3], bounding_box[1],                   bounding_box[2] + bounding_box[5], 1.0f},
                                   {bounding_box[0],                   bounding_box[1] + bounding_box[4], bounding_box[2] + bounding_box[5], 1.0f},
                                   {bounding_box[0] + bounding_box[3], bounding_box[1] + bounding_box[4], bounding_box[2] + bounding_box[5], 1.0f}};

            float camera[4] = {0.0f, 0.0f, 0.0f, 1.0f};
            multiply_matrix_vector(image_calibration_matrices.at(current_image).data(), camera, camera);

            for (size_t i = 0; i < 8; ++i)
                multiply_matrix_vector(image_calibration_matrices.at(current_image).data(), corners[i], corners[i]);

            float corners_to_camera[8][4];
            for (size_t i = 0; i < 8; ++i)
                vector_minus_vector(corners[i], camera, corners_to_camera[i]);

            float distance[8];
            for (size_t i = 0; i < 8; ++i)
                distance[i] = vector_length(corners_to_camera[i]);

            struct {int index; float length;} nearest[3];
            for (size_t j = 0; j < 3; j++)
            {
                nearest[j].index = 0;
                float length = distance[0];
            }

            for (size_t i = 0; i < 3; ++i)
                for (size_t j = 0; j < 8; ++j)
                {
                if (i == 0)
                {
                    if (distance[j] < nearest[0].length)
                    {
                        nearest[0].index = j;
                        nearest[0].length = distance[j];
                    }
                }
                else if (i == 1)
                {
                    if (nearest[0].length < distance[j] && distance[j] < nearest[1].length)
                    {
                        nearest[1].index = j;
                        nearest[1].length = distance[j];
                    }
                }
                else if (i == 2)
                {
                    if (nearest[1].length < distance[j] && distance[j] < nearest[2].length)
                    {
                        nearest[2].index = j;
                        nearest[2].length = distance[j];
                    }
                }
            }

            size_t start[3];
            size_t stop[3];
            int direction[3];
            switch(nearest[0].index)
            {
            case 0:
                start[0] = 0;   stop[0] = dimensions[0];    direction[0] = 1;
                start[1] = 0;   stop[1] = dimensions[1];    direction[1] = 1;
                start[2] = 0;   stop[2] = dimensions[2];    direction[2] = 1;
                break;
            case 1:
                start[0] = dimensions[0];   stop[0] = 0;    direction[0] = -1;
                start[1] = 0;   stop[1] = dimensions[1];    direction[1] = 1;
                start[2] = 0;   stop[2] = dimensions[2];    direction[2] = 1;
                break;
            case 2:
                start[0] = 0;   stop[0] = dimensions[0];    direction[0] = 1;
                start[1] = dimensions[1];   stop[1] = 0;    direction[1] = -1;
                start[2] = 0;   stop[2] = dimensions[2];    direction[2] = 1;
                break;
            case 3:
                start[0] = dimensions[0];   stop[0] = 0;    direction[0] = -1;
                start[1] = dimensions[1];   stop[1] = 0;    direction[1] = -1;
                start[2] = 0;   stop[2] = dimensions[2];    direction[2] = 1;
                break;
            case 4:
                start[0] = 0;   stop[0] = dimensions[0];    direction[0] = 1;
                start[1] = 0;   stop[1] = dimensions[1];    direction[1] = 1;
                start[2] = dimensions[2];   stop[2] = dimensions[2];    direction[2] = -1;
                break;
            case 5:
                start[0] = dimensions[0];   stop[0] = dimensions[0];    direction[0] = -1;
                start[1] = 0;   stop[1] = dimensions[1];    direction[1] = 1;
                start[2] = dimensions[2];   stop[2] = dimensions[2];    direction[2] = -1;
                break;
            case 6:
                start[0] = 0;   stop[0] = dimensions[0];    direction[0] = 1;
                start[1] = dimensions[1];   stop[1] = 0;    direction[1] = -1;
                start[2] = dimensions[2];   stop[2] = 0;    direction[2] = -1;
                break;
            case 7:
                start[0] = dimensions[0];   stop[0] = 0;    direction[0] = -1;
                start[1] = dimensions[1];   stop[1] = 0;    direction[1] = -1;
                start[2] = dimensions[2];   stop[2] = 0;    direction[2] = -1;
                break;
            }

            for (size_t x = start[0]; x < stop[0]; x+=direction[0])
            {
                for (size_t y = start[1]; y < stop[1]; y+=direction[1])
                {
                    for (size_t z = start[2]; z < stop[2]; z+=direction[2])
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
                        ocl_kernel_step_3.setArg(9, current_image);
                        ocl_kernel_step_3.setArg(10, number_of_images);

                        cl::KernelFunctor func_step_3 = ocl_kernel_step_3.bind(ocl_command_queue,
                                                                               cl::NDRange(1),
                                                                               cl::NDRange(1));

                        func_step_3().wait();
                    }
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
                                            number_of_consistent_hypotheses);

        std::cout << "Number of consistent hypotheses = " << *number_of_consistent_hypotheses << std::endl;
        float float_number_of_consistent_hypotheses = *number_of_consistent_hypotheses;
        threshold = float_number_of_consistent_hypotheses/(float)(dimensions[0]*dimensions[1]*dimensions[2]*number_of_images*10000);
        std::cout << "threshold = " << threshold << std::endl;

        // remove sz buffer
        delete z_buffer;
    }
}



///////////////////////////////////////////////////////////////////////////////
//! step 4: build voxel model from variety of hypotheses
///////////////////////////////////////////////////////////////////////////////
void VoxelColorer::run_step_4(cl::Buffer & hypotheses_buffer,
                              cl::Buffer & dimensions_buffer)
{
    cl::Program ocl_program;

    // create opencl buffer for resulting voxel model
    cl::Buffer voxel_model_buffer (ocl_context,
                                   CL_MEM_WRITE_ONLY,
                                   dimensions[0]*dimensions[1]*dimensions[2]*4*sizeof(unsigned char));

    build_program(ocl_program, "ocl/step_4_build_voxel_model_from_variety_of_hypotheses.cl");

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
                                        dimensions[0]*dimensions[1]*dimensions[2]*4*sizeof(unsigned char),
                                        voxel_model.data());
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

    voxel_model.resize(dimensions[0]*dimensions[1]*dimensions[2]*4*sizeof(unsigned char), 0);
}
