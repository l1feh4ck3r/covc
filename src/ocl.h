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
// getters
    cl_program get_program() { return ocl_program; }


private:
    QString definitions;
    QString source_code;

    // OpenCL uses
    cl_command_queue    ocl_command_queue;
    cl_context          ocl_context;
    cl_uint             ocl_device_count;
    cl_device_id        ocl_device_id;
    cl_kernel           ocl_kernel;
    cl_program          ocl_program;
};

#endif // OCL_H
