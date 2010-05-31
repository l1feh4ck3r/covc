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

VoxelColorer::VoxelColorer()
    :width(0), height(0),
    number_of_images(0),
    threshold(0.0)

{
    memset(dimensions, 0, sizeof(dimensions));
    memset(camera_calibration_matrix, 0, sizeof(camera_calibration_matrix));
    memset(bounding_box, 0, sizeof(bounding_box));
}


VoxelColorer::~VoxelColorer()
{

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
                              cl::ImageFormat(CL_RGB, CL_UNSIGNED_INT8),
                              width,
                              height,
                              number_of_images,
                              width*3*sizeof(char),
                              width*height*3*sizeof(char));

    // create opencl buffer for projection matricies
    cl::Buffer projection_matrices_buffer(ocl_context,
                                          CL_MEM_READ_ONLY,
                                          number_of_images*16*sizeof(float));

    // create opencl buffer for hypotheses
    cl::Buffer hypotheses_buffer(ocl_context,
                                 CL_MEM_READ_WRITE,
                                 dimensions[0]*dimensions[1]*dimensions[2]*
                                 (2*sizeof(char)+number_of_images*3*sizeof(char)));

    // create opencl buffer for number of consistent hypotheses
    cl::Buffer consistent_hypotheses_buffer(ocl_context,
                                            CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                                            dimensions[0]*dimensions[1]*dimensions[2]*sizeof(char));

    cl::Buffer number_of_consistent_hypotheses_buffer (ocl_context,
                                                      CL_MEM_READ_WRITE,
                                                      sizeof(unsigned int));

    // create opencl buffer for bounding box
    cl::Buffer bounding_box_buffer (ocl_context, CL_MEM_READ_ONLY, sizeof(bounding_box));

    // create opencl buffer for z buffer
    cl::Buffer z_buffer ( ocl_context,
                          CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                          width*height*number_of_images*sizeof(char));

    // create opencl buffer for resulting voxel model
    cl::Image3D voxel_model_buffer ( ocl_context,
                                     CL_MEM_WRITE_ONLY,
                                     cl::ImageFormat(CL_RGB, CL_UNSIGNED_INT8),
                                     dimensions[0],
                                     dimensions[1],
                                     dimensions[2],
                                     dimensions[0]*3*sizeof(char),
                                     dimensions[0]*dimensions[1]*3*sizeof(char));

//    cl::WriteImage3DCommand> write_images_buffer_command (new WriteImage3DCommand(images_buffer, pixels.get()));
//    cl::WriteBufferCommand> write_projection_matrices_buffer_command (new WriteBufferCommand(projection_matrices_buffer, projection_matrices.data()));
//    cl::WriteBufferCommand> write_hypotheses_buffer_command (new WriteBufferCommand(hypotheses_buffer,  hypotheses.get()));
//    cl::WriteBufferCommand> write_bounding_box_buffer_command (new WriteBufferCommand(bounding_box_buffer, bounding_box));
//    cl::ReadBufferCommand> read_voxel_model_buffer_command (new ReadBufferCommand(voxel_model_buffer, voxel_model.get()));
//    cl::ReadBufferCommand> read_number_of_consistent_hypotheses_command (new ReadBufferCommand(number_of_consistent_hypotheses_buffer, &number_of_consistent_hypotheses));
///////////////////////////////////////////////////////////////////////////////
//! end of create buffers
///////////////////////////////////////////////////////////////////////////////

    std::vector<cl::Device> devices = ocl_context.getInfo<CL_CONTEXT_DEVICES>();
    cl::CommandQueue ocl_command_queue(ocl_context, devices[0]);

    ///////////////////////////////////////////////////////////////////////////////
    //! step 1: for each voxel build variety of hypotheses
    //! HYPOTHESIS EXTRACTION
    ///////////////////////////////////////////////////////////////////////////////
    build_program(ocl_program, "ocl/step_1_build_variety_of_hypotheses.cl");

    cl::Kernel ocl_kernel_step_1 = cl::Kernel(ocl_program, "build_variety_of_hypotheses");
    ocl_kernel_step_1.setArg(0, bounding_box_buffer);
    ocl_kernel_step_1.setArg(1, images_buffer);
    ocl_kernel_step_1.setArg(2, projection_matrices_buffer);
    ocl_kernel_step_1.setArg(3, hypotheses_buffer);

    ocl_command_queue.enqueueWriteBuffer(bounding_box_buffer,
                                         CL_TRUE,
                                         0,
                                         sizeof(bounding_box),
                                         bounding_box);

    cl::size_t<3> offsets; //size_t offsets[3] = {0, 0, 0};
    cl::size_t<3> sizes;
    sizes[0] = width; sizes[1] = height; sizes[2] = number_of_images;

    ocl_command_queue.enqueueWriteImage(images_buffer,
                                        CL_TRUE,
                                        offsets,
                                        sizes,
                                        width*3*sizeof(char),
                                        width*height*3*sizeof(char),
                                        pixels.data());

    ocl_command_queue.enqueueWriteBuffer(projection_matrices_buffer,
                                         CL_TRUE,
                                         0,
                                         number_of_images*16*sizeof(float),
                                         projection_matrices.data());

    ocl_command_queue.enqueueWriteBuffer(hypotheses_buffer,
                                         CL_TRUE,
                                         0,
                                         dimensions[0]*dimensions[1]*dimensions[2]*
                                         (2*sizeof(char)+number_of_images*3*sizeof(char)),
                                         hypotheses.data());

    cl::KernelFunctor func_step_1 = ocl_kernel_step_1.bind(ocl_command_queue,
                                                    cl::NDRange(dimensions[0], dimensions[1], dimensions[2]),
                                                    cl::NDRange(0, 0, 0));

    func_step_1().wait();

    ocl_command_queue.finish();

    ///////////////////////////////////////////////////////////////////////////////
    //! step 2: initial inconsistent voxels rejection
    //! CONSISTENCY CHECK AND HYPOTHESIS REMOVAL
    ///////////////////////////////////////////////////////////////////////////////
    build_program(ocl_program, "ocl/step_2_initial_inconsistent_voxels_rejection_by_hypotheses.cl");

    cl::Kernel ocl_kernel_step_2 = cl::Kernel(ocl_program, "initial_inconsistent_voxels_rejection");

    // size of data for (voxel visibility + number_of_consistent_hypotheses + hypotheses)
    size_t hypotheses_size = 2*sizeof(char)+number_of_images*3*sizeof(char);

    for (size_t x = 0; x < dimensions[0]; ++x)
    {
        for (size_t y = 0; y < dimensions[1]; ++y)
        {
            for (size_t z = 0; z < dimensions[2]; ++z)
            {
                // offset to hypothesis for voxel with coordinates [x][y][z]
                ocl_kernel_step_2.setArg(0, hypotheses);
                ocl_kernel_step_2.setArg(1,
                                  z*hypotheses_size +
                                  y*dimensions[2]*hypotheses_size +
                                  x*dimensions[1]*dimensions[0]*hypotheses_size);
                ocl_kernel_step_2.setArg(2, threshold);

                cl::KernelFunctor func_step_2 = ocl_kernel_step_2.bind(ocl_command_queue,
                                       cl::NDRange(number_of_images),
                                       cl::NDRange(0));

                func_step_2().wait();
            }
        }
    }

    ocl_command_queue.finish();

    // calculate number of consistent hypotheses for each voxel
    // if number of consistent hypotheses for one voxel is zero,
    // make this voxel transparent
    build_program(ocl_program, "ocl/step_2_3_calculate_number_of_consistent_hypotheses_by_voxels.cl");
    cl::Kernel ocl_kernel_step_2_3_first = cl::Kernel(ocl_program, "calculate_number_of_consistent_hypotheses_by_voxels");
    ocl_kernel_step_2_3_first.setArg(0, hypotheses_buffer);
    ocl_kernel_step_2_3_first.setArg(1, consistent_hypotheses_buffer);
    ocl_kernel_step_2_3_first.setArg(2, number_of_images);

    cl::KernelFunctor func_step_2_3_first = ocl_kernel_step_2_3_first.bind(ocl_command_queue,
                                                               cl::NDRange(dimensions[0], dimensions[1], dimensions[2]),
                                                               cl::NDRange(0, 0, 0));

    func_step_2_3_first().wait();
    ocl_command_queue.finish();

    build_program(ocl_program, "ocl/step_2_3_calculate_number_of_consistent_hypotheses.cl");
    cl::Kernel ocl_kernel_step_2_3_second = cl::Kernel(ocl_program, "calculate_number_of_consistent_hypotheses");
    ocl_kernel_step_2_3_second.setArg(0, consistent_hypotheses_buffer);
    ocl_kernel_step_2_3_second.setArg(1, number_of_images);
    ocl_kernel_step_2_3_second.setArg(2, number_of_consistent_hypotheses);

    cl::KernelFunctor func_step_2_3_second = ocl_kernel_step_2_3_second.bind(ocl_command_queue,
                                                                             cl::NDRange(1),
                                                                             cl::NDRange(0));
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

    build_program(ocl_program, "ocl/step_3_inconsistent_voxels_rejection.cl");

    cl::Kernel ocl_kernel_step_3 = cl::Kernel(ocl_program, "inconsistent_voxel_rejection");

    // size of data for (voxel visibility + number_of_consistent_hypotheses + hypotheses)
    //size_t hypotheses_size = 2*sizeof(char)+number_of_images*3*sizeof(char);

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
                    ocl_kernel_step_3.setArg(0, hypotheses);
                    ocl_kernel_step_3.setArg(1,
                                      z*hypotheses_size +
                                      y*dimensions[2]*hypotheses_size +
                                      x*dimensions[1]*dimensions[0]*hypotheses_size);
                    ocl_kernel_step_3.setArg(2, threshold);

                    cl::KernelFunctor func_step_3 = ocl_kernel_step_3.bind(ocl_command_queue,
                                           cl::NDRange(number_of_images),
                                           cl::NDRange(0));

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

    }

    ///////////////////////////////////////////////////////////////////////////////
    //! step 4: build voxel model from variety of hypotheses
    ///////////////////////////////////////////////////////////////////////////////

    build_program(ocl_program, "step_4_build_voxel_model_from_variety_of_hypotheses.cl");

    cl::Kernel ocl_kernel_step_4 = cl::Kernel(ocl_program, "build_voxel_model");
    ocl_kernel_step_4.setArg(0,hypotheses_buffer);
    ocl_kernel_step_4.setArg(1, voxel_model_buffer);
    ocl_kernel_step_4.setArg(2, number_of_images);

    cl::KernelFunctor func_step_4 = ocl_kernel_step_4.bind(ocl_command_queue,
                                                           cl::NDRange(dimensions[0], dimensions[1], dimensions[2]),
                                                           cl::NDRange(0, 0, 0));

    func_step_4().wait();
    ocl_command_queue.finish();

    cl::size_t<3> voxel_model_sizes;
    voxel_model_sizes[0] = dimensions[0];
    voxel_model_sizes[1] = dimensions[1];
    voxel_model_sizes[2] = dimensions[2];

    ocl_command_queue.enqueueReadImage(voxel_model_buffer,
                                       CL_TRUE,
                                       offsets,
                                       voxel_model_sizes,
                                       dimensions[0]*3*sizeof(char),
                                       dimensions[0]*dimensions[1]*3*sizeof(char),
                                       voxel_model.data());
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

        cl::Program::Sources source(1, std::make_pair(ss.str().c_str(), ss.str().length()));

        program = cl::Program(ocl_context, source);

        result = true;
    }
    catch(cl::Error ex)
    {
        std::cerr << "COVC: " << ex.what() << "(" << ex.err() << ": " << ex.error() << "):" << std::endl;
        result = false;
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
//! Calculate bounding box
///////////////////////////////////////////////////////////////////////////////
void VoxelColorer::calculate_bounding_box()
{
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

        cl::Context context (CL_DEVICE_TYPE_GPU, &context_properties[0]);
        ocl_context = context;

        result = true;
    }
    catch(cl::Error ex)
    {
        std::cerr << "COVC: " << ex.what() << "(" << ex.err() << ": " << ex.error() << ")" << std::endl;
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

void VoxelColorer::set_resulting_voxel_cube_dimensions
        (unsigned int dimension_x,
         unsigned int dimension_y,
         unsigned int dimension_z)
{
    dimensions[0] = dimension_x;
    dimensions[1] = dimension_y;
    dimensions[2] = dimension_z;
}
