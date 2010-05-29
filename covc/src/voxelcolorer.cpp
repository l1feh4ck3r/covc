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

#include "cl.hpp"

#include <iostream>

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

    boost::shared_ptr<Program> ocl_program;
    boost::shared_ptr<Kernel>  ocl_kernel;

    unsigned int number_of_consistent_hypotheses;

    boost::shared_ptr<CommandQueue> ocl_command_queue = ocl_context->createCommandQueue();

    calculate_bounding_box();

///////////////////////////////////////////////////////////////////////////////
//! Create buffers
///////////////////////////////////////////////////////////////////////////////
    // create opencl buffer for images
    boost::shared_ptr<Image3D> images_buffer = Image3D::createImage3D(ocl_context,
                                                                      Memory::READ_ONLY,
                                                                      width,
                                                                      height,
                                                                      number_of_images,
                                                                      Image::ImageFormat(Image::RGB, Image::UNSIGNED_INT8));

    // create opencl buffer for projection matricies
    boost::shared_ptr<Buffer> projection_matrices_buffer = ocl_context->createBuffer(Memory::READ_ONLY, number_of_images*16*sizeof(float));

    // create opencl buffer for hypotheses

    boost::shared_ptr<Buffer> hypotheses_buffer = ocl_context->createBuffer(Memory::READ_WRITE | Memory::ALLOC_HOST_PTR,
                                                                            dimensions[0]*dimensions[1]*dimensions[2]*
                                                                            (2*sizeof(char)+number_of_images*3*sizeof(char)));

    // create opencl buffer for number of consistent hypotheses
    boost::shared_ptr<Buffer> consistent_hypotheses_buffer = ocl_context->createBuffer(Memory::READ_WRITE | Memory::ALLOC_HOST_PTR,
                                                                                       dimensions[0]*dimensions[1]*dimensions[2]*number_of_images*3*sizeof(unsigned int));

    boost::shared_ptr<Buffer> number_of_consistent_hypotheses_buffer = ocl_context->createBuffer(Memory::READ_WRITE,
                                                                                                 sizeof(unsigned int));

    // create opencl buffer for bounding box
    boost::shared_ptr<Buffer> bounding_box_buffer = ocl_context->createBuffer(Memory::READ_ONLY, sizeof(bounding_box));

    // create opencl buffer for z buffer
    boost::shared_ptr<Buffer> z_buffer = ocl_context->createBuffer(Memory::READ_WRITE | Memory::ALLOC_HOST_PTR,
                                                                   width*height*number_of_images*sizeof(char));

    // create opencl buffer for resulting voxel model
    boost::shared_ptr<Image3D> voxel_model_buffer = Image3D::createImage3D(ocl_context,
                                                                           Memory::WRITE_ONLY,
                                                                           dimensions[0],
                                                                           dimensions[1],
                                                                           dimensions[2],
                                                                           Image::ImageFormat(Image::RGB, Image::UNSIGNED_INT8));

    boost::shared_ptr<WriteImage3DCommand> write_images_buffer_command (new WriteImage3DCommand(images_buffer, pixels.get()));
    boost::shared_ptr<WriteBufferCommand> write_projection_matrices_buffer_command (new WriteBufferCommand(projection_matrices_buffer, projection_matrices.data()));
    boost::shared_ptr<WriteBufferCommand> write_hypotheses_buffer_command (new WriteBufferCommand(hypotheses_buffer,  hypotheses.get()));
    boost::shared_ptr<WriteBufferCommand> write_bounding_box_buffer_command (new WriteBufferCommand(bounding_box_buffer, bounding_box));
    boost::shared_ptr<ReadBufferCommand> read_voxel_model_buffer_command (new ReadBufferCommand(voxel_model_buffer, voxel_model.get()));
    boost::shared_ptr<ReadBufferCommand> read_number_of_consistent_hypotheses_command (new ReadBufferCommand(number_of_consistent_hypotheses_buffer, &number_of_consistent_hypotheses));
///////////////////////////////////////////////////////////////////////////////
//! end of create buffers
///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    //! step 1: for each voxel build variety of hypotheses
    //! HYPOTHESIS EXTRACTION
    ///////////////////////////////////////////////////////////////////////////////
    build_program(ocl_program, "ocl/step_1_build_variety_of_hypotheses.cl");

    ocl_kernel = ocl_program->createKernel("build_variety_of_hypotheses");
    ocl_kernel->setArguments(bounding_box_buffer, images_buffer, projection_matrices_buffer, hypotheses_buffer);

    boost::shared_ptr<Range3DCommand> calculate_by_voxels (new Range3DCommand(ocl_kernel, dimensions[0], dimensions[1], dimensions[2]));

    ocl_command_queue->enque(write_bounding_box_buffer_command);
    ocl_command_queue->enque(write_images_buffer_command);
    ocl_command_queue->enque(write_projection_matrices_buffer_command);
    ocl_command_queue->enque(write_hypotheses_buffer_command);

    ocl_command_queue->enque(calculate_by_voxels);
    ocl_command_queue->finish();


    ///////////////////////////////////////////////////////////////////////////////
    //! step 2: initial inconsistent voxels rejection
    //! CONSISTENCY CHECK AND HYPOTHESIS REMOVAL
    ///////////////////////////////////////////////////////////////////////////////
    build_program(ocl_program, "ocl/step_2_initial_inconsistent_voxels_rejection_by_hypotheses.cl");

    ocl_kernel = ocl_program->createKernel("initial_inconsistent_voxels_rejection");

    // size of data for (voxel visibility + number_of_consistent_hypotheses + hypotheses)
    size_t hypotheses_size = 2*sizeof(char)+number_of_images*3*sizeof(char);

    for (size_t x = 0; x < dimensions[0]; ++x)
        for (size_t y = 0; y < dimensions[1]; ++y)
            for (size_t z = 0; z < dimensions[2]; ++z)
            {
                // offset to hypothesis for voxel with coordinates [x][y][z]
                ocl_kernel->setArguments(hypotheses_buffer,
                                         z*hypotheses_size +
                                         y*dimensions[2]*hypotheses_size +
                                         x*dimensions[1]*dimensions[0]*hypotheses_size,
                                         threshold);

                boost::shared_ptr<Range1DCommand> calculate_by_hypotheses (new Range1DCommand(ocl_kernel, number_of_images));

                ocl_command_queue->enque(calculate_by_voxels);
            }

    ocl_command_queue->finish();

    // calculate number of consistent hypotheses for each voxel
    // if number of consistent hypotheses for one voxel is zero,
    // make this voxel transparent
    build_program(ocl_program, "ocl/step_2_3_calculate_number_of_consistent_hypotheses_by_voxels.cl");
    boost::shared_ptr<Kernel> number_of_consistent_hypotheses_by_voxel_calculator_kernel = ocl_program->createKernel("calculate_number_of_consistent_hypotheses_by_voxels");
    number_of_consistent_hypotheses_by_voxel_calculator_kernel->setArguments(hypotheses_buffer, consistent_hypotheses_buffer, number_of_images);
    boost::shared_ptr<Range3DCommand> number_of_consistent_hypotheses_by_voxel_calculator (new Range3DCommand(number_of_consistent_hypotheses_by_voxel_calculator_kernel,
                                                                                                              dimensions[0],
                                                                                                              dimensions[1],
                                                                                                              dimensions[2]));

    build_program(ocl_program, "ocl/step_2_3_calculate_number_of_consistent_hypotheses.cl");
    boost::shared_ptr<Kernel> number_of_consistent_hypotheses_calculator_kernel = ocl_program->createKernel("calculate_number_of_consistent_hypotheses");
    number_of_consistent_hypotheses_calculator_kernel->setArguments(consistent_hypotheses_buffer, number_of_images, number_of_consistent_hypotheses);
    boost::shared_ptr<Range1DCommand> number_of_consistent_hypotheses_calculator (new Range1DCommand(number_of_consistent_hypotheses_by_voxel_calculator_kernel, 1));

    ocl_command_queue->enque(number_of_consistent_hypotheses_by_voxel_calculator);
    ocl_command_queue->finish();
    ocl_command_queue->enque(number_of_consistent_hypotheses_calculator);
    ocl_command_queue->finish();
    ocl_command_queue->enque(read_number_of_consistent_hypotheses_command);
    ocl_command_queue->finish();

    threshold = const_cast<float>(number_of_consistent_hypotheses)/const_cast<float>(dimensions[0]*dimensions[1]*dimensions[2]*number_of_images);

    ///////////////////////////////////////////////////////////////////////////////
    //! step 3: inconsistent hypotheses rejection. visibility buffer in use
    ///////////////////////////////////////////////////////////////////////////////

    build_program(ocl_program, "ocl/step_3_inconsistent_voxels_rejection.cl");

    ocl_kernel = ocl_program->createKernel("inconsistent_voxel_rejection");

    // size of data for (voxel visibility + number_of_consistent_hypotheses + hypotheses)
    size_t hypotheses_size = 2*sizeof(char)+number_of_images*3*sizeof(char);

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

                    ocl_kernel->setArguments(hypotheses_buffer,
                                             // offset to hypothesis for voxel with coordinates [x][y][z]
                                             z*hypotheses_size +
                                             y*dimensions[2]*hypotheses_size +
                                             x*dimensions[1]*dimensions[0]*hypotheses_size,
                                             z_buffer,
                                             threshold);

                    boost::shared_ptr<Range1DCommand> calculate_by_hypotheses (new Range1DCommand(ocl_kernel, number_of_images));

                    ocl_command_queue->enque(calculate_by_voxels);
                }
            }
        }

        ocl_command_queue->finish();

        // calculate number of consistent hypotheses for each voxel
        // if number of consistent hypotheses for one voxel is zero,
        // make this voxel transparent
        build_program(ocl_program, "ocl/step_2_3_calculate_number_of_consistent_hypotheses_by_voxels.cl");
        boost::shared_ptr<Kernel> number_of_consistent_hypotheses_by_voxel_calculator_kernel = ocl_program->createKernel("calculate_number_of_consistent_hypotheses_by_voxels");
        number_of_consistent_hypotheses_by_voxel_calculator_kernel->setArguments(hypotheses_buffer, consistent_hypotheses_buffer, number_of_images);
        boost::shared_ptr<Range3DCommand> number_of_consistent_hypotheses_by_voxel_calculator (new Range3DCommand(number_of_consistent_hypotheses_by_voxel_calculator_kernel,
                                                                                                                  dimensions[0],
                                                                                                                  dimensions[1],
                                                                                                                  dimensions[2]));

        build_program(ocl_program, "ocl/step_2_3_calculate_number_of_consistent_hypotheses.cl");
        boost::shared_ptr<Kernel> number_of_consistent_hypotheses_calculator_kernel = ocl_program->createKernel("calculate_number_of_consistent_hypotheses");
        number_of_consistent_hypotheses_calculator_kernel->setArguments(consistent_hypotheses_buffer, number_of_images, number_of_consistent_hypotheses);
        boost::shared_ptr<Range1DCommand> number_of_consistent_hypotheses_calculator (new Range1DCommand(number_of_consistent_hypotheses_by_voxel_calculator_kernel, 1));

        ocl_command_queue->enque(number_of_consistent_hypotheses_by_voxel_calculator);
        ocl_command_queue->finish();
        ocl_command_queue->enque(number_of_consistent_hypotheses_calculator);
        ocl_command_queue->finish();
        ocl_command_queue->enque(read_number_of_consistent_hypotheses_command);
        ocl_command_queue->finish();

    }

    ///////////////////////////////////////////////////////////////////////////////
    //! step 4: build voxel model from variety of hypotheses
    ///////////////////////////////////////////////////////////////////////////////

    build_program(ocl_program, "step_4_build_voxel_model_from_variety_of_hypotheses.cl");

    ocl_kernel = ocl_program->createKernel("build_voxel_model");
    ocl_kernel->setArguments(hypotheses_buffer, voxel_model_buffer, number_of_images);

    boost::shared_ptr<Range3DCommand> build_voxel_model_command (new Range3DCommand(ocl_kernel, dimensions[0], dimensions[1], dimensions[2]));

    ocl_command_queue->enque(build_voxel_model_command);
    ocl_command_queue->finish();
    ocl_command_queue->enque(read_voxel_model_buffer_command);
}

///////////////////////////////////////////////////////////////////////////////
//! Build the opencl program.
//!
//! @param path to file with opencl program
///////////////////////////////////////////////////////////////////////////////
bool VoxelColorer::build_program(boost::shared_ptr<Program> & program, const std::string & path_to_file_with_program)
{
    bool result = false;

    try
    {
        program = ocl_context->createProgramFromFile(path_to_file_with_program);

        result = program->buildProgram("-cl-mad-enable");

        if (!result)
            std::cerr << ocl_program->getBuildLog() << std::endl;
    }
    catch(Exception ex)
    {
        std::cerr << ex.what() << std::endl;
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
        if (!ocl_context.use_count())
        {
            PlatformList platformList(Host::getPlatformList());

            // BUG: fucking woodoo magic
            std::vector<Context::Property> properties;
            properties.push_back(Context::Property(Context::CONTEXT_PLATFORM,
                                                   // using of reinterpret cast in this case is abnormal !!!
                                                   reinterpret_cast<Context::PropertyValue>(platformList[0]->getHandle())));
            ocl_context = Context::createContext(Device::GPU, properties);
        }
        result = true;
    }
    catch(Exception ex)
    {
        std::cerr << ex.what() << std::endl;
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
