#include <fstream>
#include <iostream>
using namespace std;

#include <CL/cl.h>

///////////////////////////////////////////////////////////////////////////////
//! Global variables
//
cl_context          ocl_context;
cl_kernel           ocl_kernel;
cl_command_queue    ocl_command_queue;
cl_uint             ocl_device_count;
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//! Functions definitions
//
int     prepare_opencl();
char *  load_source(const char *filename, size_t &file_size);
//
///////////////////////////////////////////////////////////////////////////////

char * load_source(const char *filename, size_t *file_size)
{
    FILE * source_file = NULL;
    size_t real_file_size = 0;

#ifdef _WIN32   // Windows version
    if(fopen_s(&source_file, filename, "rb") != 0)
    {
        cout << "Error: can't open file \'" << filename << "\'" << endl;
        return NULL;
    }
#else           // Linux version
    source_file = fopen(filename, "rb");
    if(source_file == 0)
    {
        cout << "Error: can't open file \'" << filename << "\'" << endl;
        return NULL;
    }
#endif

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

void main(int argc, char* argv[])
{
}

///////////////////////////////////////////////////////////////////////////////
//! Prepare for using OpenCL: create device context, kernel, command queue
///////////////////////////////////////////////////////////////////////////////
int prepare_opencl()
{
    cl_uint ocl_device_count = 0;
    cl_int ocl_error_number = CL_SUCCESS;

    // try to create opencl context on GPU device
    ocl_context = clCreateContextFromType(NULL, CL_DEVICE_TYPE_GPU, NULL, NULL, &ocl_error_number);
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
    cl_device_id* ocl_devices_id = new (cl_device_id[number_device_bytes]);
    clGetContextInfo(ocl_context, CL_CONTEXT_DEVICES, number_device_bytes, ocl_devices_id, NULL);
    // using the first opencl compatible device
    cl_device_id ocl_device_id = ocl_devices_id[0];
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
