#ifndef OCL_H
#define OCL_H

#include <CL/cl.h>

#include <QString>

class Ocl
{
public:
    Ocl();


public:
    void    add_definition(QString name, QString value);
    int     build_source();
    bool    load_from_file(QString filename);
    int     prepare_opencl();
    virtual void run();


private:
    QString definitions;
    QString source_code;

    // OpenCL uses
    cl_context          ocl_context;
    cl_kernel           ocl_kernel;
    cl_command_queue    ocl_command_queue;
    cl_uint             ocl_device_count;
    cl_program          ocl_program;
    cl_device_id        ocl_device_id;
};

#endif // OCL_H