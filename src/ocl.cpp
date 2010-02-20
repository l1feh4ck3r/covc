#include "ocl.h"

#include <QFile>
#include <QTextStream>

Ocl::Ocl()
{
}


///////////////////////////////////////////////////////////////////////////////
//! Build opencl source code
//!
//! @return 0 if succeeded, OpenCL error number otherwise
///////////////////////////////////////////////////////////////////////////////
int Ocl::build_source()
{
    // BUG: I think there is a very buggy code,
    // 'case too many hacks
    cl_int ocl_error_number = CL_SUCCESS;
    const char * source_code_data = source_code.toAscii().data();
    size_t source_code_size = source_code.size();

    ocl_program = clCreateProgramWithSource(ocl_context, 1,
                                            &source_code_data,
                                            &source_code_size,
                                            &ocl_error_number);
    if (ocl_error_number != CL_SUCCESS)
        // failed to create program.
        return ocl_error_number;

    ocl_error_number = clBuildProgram(ocl_program, 1, &ocl_device_id, "-cl-mad-enable", NULL, NULL);
    if (ocl_error_number != CL_SUCCESS)
        // failed to build program.
        return ocl_error_number;

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
//! Load opencl source code from file.
//!
//! @return true if succeeded, false otherwise
//! @param filename Name of the file with opencl source code
///////////////////////////////////////////////////////////////////////////////
bool Ocl::load_from_file(QString filename)
{
    QFile source_code_file(filename);
    if (!source_code_file.open(QFile::ReadOnly))
        return false;

    QTextStream input_stream(&source_code_file);
    source_code = input_stream.readAll();

    if (source_code.isEmpty())
        return false;

    return true;
}


///////////////////////////////////////////////////////////////////////////////
//! Prepare for using OpenCL: create device context, kernel, command queue
//!
//! @return 0 if succeeded, OpenCL error otherwise
///////////////////////////////////////////////////////////////////////////////
int Ocl::prepare_opencl()
{
    cl_uint ocl_device_count = 0;
    cl_int ocl_error_number = CL_SUCCESS;

    // try to create opencl context on GPU device
    ocl_context = clCreateContextFromType(NULL, CL_DEVICE_TYPE_GPU, NULL, NULL, &ocl_error_number);
    if (ocl_error_number != CL_SUCCESS)
        // context not created
        return ocl_error_number;

    // find out how many GPU's available
    //TODO: It's part of code for future. Now only first opencl compatible device will be used.
    size_t number_device_bytes;
    ocl_error_number = clGetContextInfo(ocl_context, CL_CONTEXT_DEVICES, 0, NULL, &number_device_bytes);
    ocl_device_count = static_cast<cl_uint>(number_device_bytes)/sizeof(cl_device_id);

    if (ocl_error_number != CL_SUCCESS)
        // can't get info about context
        return ocl_error_number;
    else if (ocl_device_count == 0)
        // no OpenCL supported device found
        return -1;

    // getting id of the first opencl compatible device
    //TODO: see oclGetDev from NVIDIA's oclUtils.cpp
    cl_device_id* ocl_devices_id = new (cl_device_id[number_device_bytes]);
    clGetContextInfo(ocl_context, CL_CONTEXT_DEVICES, number_device_bytes, ocl_devices_id, NULL);
    // using the first opencl compatible device
    cl_device_id ocl_device_id = ocl_devices_id[0];
    delete ocl_devices_id;

    // creating command queue
    ocl_command_queue = clCreateCommandQueue(ocl_context, ocl_device_id, 0, &ocl_error_number);
    if (ocl_error_number != CL_SUCCESS)
        // can't create opencl command queue
        return ocl_error_number;

    return 0;
}


void Ocl::run()
{
}
