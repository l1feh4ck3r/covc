#include "colorer.h"

#include <QFile>
#include <QTextStream>

Colorer::Colorer()
{
}

int Colorer::build_source()
{
    return 0;
}

bool Colorer::load_from_file(QString filename)
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

int Colorer::prepare_opencl()
{
    cl_uint ocl_device_count = 0;
    cl_int ocl_error_number = CL_SUCCESS;

    // try to create opencl context on GPU device
    ocl_context = clCreateContextFromType(NULL, CL_DEVICE_TYPE_GPU, NULL, NULL, &ocl_error_number);
    if (ocl_error_number != CL_SUCCESS)
        // context not created
        return ocl_error_number;

    // find out how many GPU's available
    //TODO: It's part of code for future. Now only one GPU will be used.
    size_t number_device_bytes;
    ocl_error_number = clGetContextInfo(ocl_context, CL_CONTEXT_DEVICES, 0, NULL, &number_device_bytes);
    ocl_device_count = (cl_uint)number_device_bytes/sizeof(cl_device_id);

    if (ocl_error_number != CL_SUCCESS)
        // can't get info about context
        return ocl_error_number;
    else if (ocl_device_count == 0)
        // no OpenCL supported device found
        return -1;

    //creating command queue
    //TODO: see oclGetDev from NVIDIA's oclUtils.cpp
    cl_device_id* ocl_devices_id = new (cl_device_id[number_device_bytes]);
    clGetContextInfo(ocl_context, CL_CONTEXT_DEVICES, number_device_bytes, ocl_devices_id, NULL);
    cl_device_id ocl_device_id = ocl_devices_id[0];
    delete ocl_devices_id;

    ocl_command_queue = clCreateCommandQueue(ocl_context, ocl_device_id, 0, &ocl_error_number);
    if (ocl_error_number != CL_SUCCESS)
        // can't create opencl command queue
        return ocl_error_number;

    return 0;
}

void Colorer::run()
{
}
