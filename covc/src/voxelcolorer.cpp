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

#include "CLxxIncludes.h"

using namespace CLxx;

#include <iostream>

VoxelColorer::VoxelColorer()
    :dimension_x(0),
    dimension_y(0),
    dimension_z(0)
{
}


VoxelColorer::~VoxelColorer()
{

}

///////////////////////////////////////////////////////////////////////////////
//! Build voxel model from seqence of images and matrices
///////////////////////////////////////////////////////////////////////////////
void VoxelColorer::build_voxel_model()
{
    build_program("ocl/voxelcolorer.cl");
    calculate_bounding_box();
    set_parameters_to_program();
    run_program();
}

///////////////////////////////////////////////////////////////////////////////
//! Build the opencl program.
//!
//! @param path to file with opencl program
///////////////////////////////////////////////////////////////////////////////
bool VoxelColorer::build_program(const std::string & path_to_file_with_program)
{
    bool result = false;

    try
    {
        ocl_program = ocl_context->createProgramFromFile(path_to_file_with_program);

        result = ocl_program->buildProgram("-cl-mad-enable");

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


void VoxelColorer::run_program()
{
    set_parameters_to_program();

    boost::shared_ptr<Kernel> kernel = prog->createKernel("VectorAdd");
    kernel->setArguments(vecBufferA,vecBufferB,verBufferR,numElements);

}

void VoxelColorer::set_camera_calibration_matrix(const float *_camera_calibration_matrix)
{
    for (size_t i = 0; i < 16; ++i)
        camera_calibration_matrix[i] = _camera_calibration_matrix[i];
}

void VoxelColorer::set_parameters_to_program()
{

}

void VoxelColorer::set_resulting_voxel_cube_dimensions
        (unsigned int _dimension_x,
         unsigned int _dimension_y,
         unsigned int _dimension_z)
{
    dimension_x = _dimension_x;
    dimension_y = _dimension_y;
    dimension_z = _dimension_z;
}
