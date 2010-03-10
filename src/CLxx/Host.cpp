#include "Host.h"
#include "Platform.h"
#include "Device.h"
#include "Context.h"
#include "Sampler.h"
#include "Program.h"
#include "Kernel.h"
#include "Exception.h"
#include <sstream>
#include <map>

namespace CLxx
{
    namespace
    {
        bool  platformQueried = false;
        static PlatformList platforms;
    }

    void Host::queryPlatforms()
    {
        platformQueried = true;

        cl_uint numPlatforms;

        // Number of platforms
        cl_int ciErrNum = clGetPlatformIDs (0, NULL, &numPlatforms);

        if (ciErrNum != CL_SUCCESS)
            throw Exception(ciErrNum);

        // Get of platforms info
        std::vector<cl_platform_id> clPlatformIDs(numPlatforms);

        // get platform info for each platform
        ciErrNum = clGetPlatformIDs (numPlatforms, &clPlatformIDs[0], NULL);

        for(cl_uint i = 0; i < numPlatforms; ++i)
        {
            cl_platform_id platformId = clPlatformIDs[i];

            boost::shared_ptr<Platform> platform = boost::shared_ptr<Platform>(new Platform(platformId));

            platforms.push_back(platform);
        }
    }

    const PlatformList& Host::getPlatformList()
    {
        if ( !platformQueried )
            queryPlatforms();

        return platforms;
    }

    void Host::unloadCompiler()
    {
        cl_int errorCode = clUnloadCompiler();

        if ( errorCode != CL_SUCCESS )
            throw Exception(errorCode);
    }
}
