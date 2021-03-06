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

#include <fstream>
#include <iostream>
using namespace std;

#include <CL/cl.h>

///////////////////////////////////////////////////////////////////////////////
//! Global variables
//
cl_context          ocl_context;
cl_kernel           ocl_kernel;
cl_uint             ocl_device_count;
cl_device_id        ocl_device_id;
cl_command_queue    ocl_command_queue;
cl_program          ocl_program;
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//! Functions definitions
//
int     build_source(const char *source, size_t source_length);
char   *load_source(const char *filename, size_t &file_size);
int     prepare_opencl();
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//! Build opencl source code and print to stdout errors.
//!
//! @return 0 if succeeded, OpenCL error number otherwise
//! @param source OpenCL source code
//! @param source_length Source code length
///////////////////////////////////////////////////////////////////////////////
int build_source(const char *source, size_t source_length)
{
    cl_int ocl_error_number = CL_SUCCESS;
    ocl_program = clCreateProgramWithSource(ocl_context, 1, &source, &source_length, &ocl_error_number);
    if (ocl_error_number != CL_SUCCESS)
    {
        cout << "Error " << ocl_error_number << ": Failed to create program." << endl;
        return ocl_error_number;
    }

    ocl_error_number = clBuildProgram(ocl_program, 1, &ocl_device_id, "-Werror", NULL, NULL);
    if (ocl_error_number != CL_SUCCESS)
    {
        cout << "Error " << ocl_error_number << ": Failed to build program." << endl;

        // print out build info
        char ocl_build_info[10240];
        clGetProgramBuildInfo(ocl_program, ocl_device_id, CL_PROGRAM_BUILD_LOG, sizeof(ocl_build_info), ocl_build_info, NULL);
        cout << "OpenCL Program Build Info:" << endl;
        cout << ocl_build_info << endl;

        return ocl_error_number;
    }

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
//! Clean up OpenCL resources
///////////////////////////////////////////////////////////////////////////////
void clean()
{
    cl_int ocl_error_number = CL_SUCCESS;

    clReleaseKernel(ocl_kernel);
    clReleaseCommandQueue(ocl_command_queue);
    clReleaseProgram(ocl_program);

    ocl_error_number = clReleaseContext(ocl_context);
    if (ocl_error_number != CL_SUCCESS)
    {
        cout << "Error " << ocl_error_number << ": Can't release OpenCL context." << endl;
    }
}


///////////////////////////////////////////////////////////////////////////////
//! Load source code from file.
//!
//! @return string with source code if succeeded, 0 otherwise
//! @param filename     Name of the file with source code
//! @param file_size    returened size of the string with source code
///////////////////////////////////////////////////////////////////////////////
char * load_source(const char *filename, size_t *file_size)
{
    FILE * source_file = NULL;
    size_t real_file_size = 0;

    source_file = fopen(filename, "rb");
    if(source_file == 0)
    {
        cout << "Error: can't open file \'" << filename << "\'" << endl;
        return NULL;
    }

    // get file size
    fseek(source_file, 0, SEEK_END);
    real_file_size = ftell(source_file);
    fseek(source_file, 0, SEEK_SET);

    // get source code
    char * source_code = new (char[real_file_size+1]);
    if (fread(source_code, real_file_size, 1, source_file) != 1)
    {
        cout << "Error: can't read source code from file \'" << filename << "\'" << endl;
        fclose(source_file);
        delete source_code;
        return NULL;
    }

    fclose(source_file);
    source_code[real_file_size] = '\0';
    *file_size = real_file_size;

    return source_code;
}


///////////////////////////////////////////////////////////////////////////////
//! Program entry point
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cout << "Invalid number of parameters." << endl;
        cout << "Usage:" << endl;
        cout << "    " << argv[0] << " <filename>" << endl << endl;
        cout << "    Where <filename> is name of the file with OpenCL source code." << endl;

        return 0;
    }

    // prepare opencl
    if (prepare_opencl() != 0)
        return -1;

    // load source code
    char * source_code = NULL;
    size_t source_length;
    source_code = load_source(argv[1], &source_length);
    if (!source_code)
        return -1;

    // compile source code
    build_source(source_code, source_length);

    // clean up resources
    clean();
    if (source_code)
        delete source_code;

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
//! Prepare for using OpenCL: create device context, kernel, command queue
//!
//! @return 0 if succeeded, OpenCL error otherwise
///////////////////////////////////////////////////////////////////////////////
int prepare_opencl()
{
    cl_uint ocl_device_count = 0;
    cl_int ocl_error_number = CL_SUCCESS;

    cl_platform_id cl_platforms[2];
    cl_uint number_of_platforms = 0;

    clGetPlatformIDs(2, cl_platforms, &number_of_platforms);

    cl_context_properties context_properties[] = {
        CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(cl_platforms[0]),
        0
    };


    // try to create opencl context on GPU device
    ocl_context = clCreateContextFromType(context_properties, CL_DEVICE_TYPE_GPU, NULL, NULL, &ocl_error_number);
    if (ocl_error_number != CL_SUCCESS)
    {
        cout << "Error " << ocl_error_number << ": Failed to create OpenCL context!" << endl;
        return ocl_error_number;
    }

    // find out how many GPU's available
    //TODO: It's part of code for future. Now only first opencl compatible device will be used.
    size_t number_device_bytes;
    ocl_error_number = clGetContextInfo(ocl_context, CL_CONTEXT_DEVICES, 0, NULL, &number_device_bytes);
    ocl_device_count = static_cast<cl_uint>(number_device_bytes)/sizeof(cl_device_id);

    if (ocl_error_number != CL_SUCCESS)
    {
        cout << "Error " << ocl_error_number << ": in clGetContextInfo!" << endl;
        return ocl_error_number;
    }
    else if (ocl_device_count == 0)
    {
        cout << "Error -1: there are no devices supporting OpenCL!" << endl;
        return -1;
    }

    // getting id of the first opencl compatible device
    //TODO: see oclGetDev from NVIDIA's oclUtils.cpp
    cl_device_id *ocl_devices_id = new (cl_device_id[number_device_bytes]);
    clGetContextInfo(ocl_context, CL_CONTEXT_DEVICES, number_device_bytes, ocl_devices_id, NULL);
    // using the first opencl compatible device
    ocl_device_id = ocl_devices_id[0];
    delete ocl_devices_id;

    // creating command queue
    ocl_command_queue = clCreateCommandQueue(ocl_context, ocl_device_id, 0, &ocl_error_number);
    if (ocl_error_number != CL_SUCCESS)
    {
        cout << "Error " << ocl_error_number << " in clCreateCommandQueue call!" << endl;
        return ocl_error_number;
    }

    return 0;
}
