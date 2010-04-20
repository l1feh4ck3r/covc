#include "Context.h"
#include "Exception.h"
#include "Host.h"
#include "Program.h"
#include <map>
//#include <thread>


namespace CLxx
{
    namespace
    {
        void contextErrorCallback(const char *errinfo, const void *private_info, size_t cb, void *user_data)
        {
            Context* ctx = (Context*) user_data;
            ctx->getErrorSignal()(errinfo,private_info,cb);
        }
    }

    boost::shared_ptr<Context> Context::createContextImpl( const std::vector<boost::shared_ptr<Device> >& devices, const std::vector<Property>* properties)
    {
        cl_int errorCode;
        cl_context_properties* prop = NULL;

        boost::shared_ptr< std::vector<cl_context_properties> > propertiesWithZero;

        if  ( properties != NULL &&  !properties->empty() )
        {
            propertiesWithZero = boost::shared_ptr<std::vector<cl_context_properties> >( new std::vector<cl_context_properties> () );
            for(std::vector<Property>::const_iterator it= properties->begin(); it != properties->end(); it++)
            {
                propertiesWithZero->push_back( it->first );
                propertiesWithZero->push_back( it->second );
            }

            if ( propertiesWithZero->back() != 0 )
                propertiesWithZero->push_back(0);

            prop = &(*propertiesWithZero)[0];
        }

        std::vector<cl_device_id> dev(devices.size());

        for( size_t i = 0; i < devices.size(); i++ )
            dev[i] = devices[i]->getHandle();


        boost::shared_ptr<Context> ctx(new Context());
        ctx->_this = ctx;
        ctx->_handle = clCreateContext(prop, dev.size(), &dev[0], contextErrorCallback, ctx.get() , &errorCode );

        if ( errorCode != CL_SUCCESS )
            throw Exception(errorCode);

        if ( properties )
            ctx->_properties = *properties;

        for(size_t i = 0 ;i < devices.size(); i++)
            ctx->_devices.push_back(devices[i]);

        return ctx;
    }

    boost::shared_ptr<Context> Context::createContextImpl( Device::DeviceType deviceType, const std::vector<Property>* properties)
    {
        cl_int errorCode;
        cl_context_properties* prop = NULL;

        boost::shared_ptr< std::vector<cl_context_properties> > propertiesWithZero;

        if  ( properties != NULL &&  !properties->empty() )
        {
            propertiesWithZero = boost::shared_ptr<std::vector<cl_context_properties> >( new std::vector<cl_context_properties> () );
            for(std::vector<Property>::const_iterator it= properties->begin(); it != properties->end(); it++)
            {
                propertiesWithZero->push_back( it->first );
                propertiesWithZero->push_back( it->second );
            }

            if ( propertiesWithZero->back() != 0 )
                propertiesWithZero->push_back(0);

            prop = &(*propertiesWithZero)[0];
        }

        boost::shared_ptr<Context> ctx(new Context());
        ctx->_this = ctx;
        ctx->_handle = clCreateContextFromType(prop, (cl_device_type) deviceType, contextErrorCallback, ctx.get() , &errorCode );

        if ( errorCode != CL_SUCCESS )
            throw Exception(errorCode);

        if ( properties )
            ctx->_properties = *properties;

        ctx->retriveDevices(deviceType);

        return ctx;
    }


    cl_uint Context::getContextReferenceCount() const
    {
        cl_uint value;
        clGetContextInfo (_handle, CL_CONTEXT_REFERENCE_COUNT, sizeof(cl_uint), &value, NULL);

        return value;
    }

    void Context::retriveDevices(Device::DeviceType deviceType)
    {
        cl_uint numContext = 0;
        cl_int ciErrNum = clGetContextInfo (_handle, CL_CONTEXT_DEVICES, 0, NULL, &numContext);

        if ( ciErrNum == CL_SUCCESS )
        {
            std::vector<cl_device_id> deviceIds(numContext);

            ciErrNum = clGetContextInfo (_handle, CL_CONTEXT_DEVICES, numContext, &deviceIds[0], NULL);

            for(size_t i = 0; i < numContext; i++)
            {
                boost::shared_ptr<Device> dev( new Device(deviceIds[i]) );

                Device::DeviceType type;

                try { type = dev->getDeviceType(); }
                catch(Exception ex) { continue; }

                if ( type & deviceType )
                    _devices.push_back(dev);
            }
        }

    }

    void Context::addCommandQueue(boost::shared_ptr<CommandQueue>& cmd)
    {
        _commandQueues.insert(cmd);
    }

    void Context::removeCommandQueue(boost::shared_ptr<CommandQueue>& cmd)
    {
        _commandQueues.erase(cmd);
    }

    boost::shared_ptr<CommandQueue> Context::createCommandQueue( CommandQueue::Properties props, boost::shared_ptr<Device> dev )
    {
        return CommandQueue::createCommandQueue(_this.lock(), props, dev);
    }

    boost::shared_ptr<Program> Context::createProgram(const char* source)
    {
        return Program::createProgram(_this.lock(), source);
    }

    boost::shared_ptr<Program> Context::createProgramFromFile(const std::string & path)
    {
        return Program::createProgramFromFile(_this.lock(), path);
    }

    boost::shared_ptr<Buffer> Context::createBuffer(Memory::Flags props, size_t mem_size, void* host_ptr)
    {
        return Buffer::createBuffer(_this.lock(), props, mem_size, host_ptr);
    }

}
