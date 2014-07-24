//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include <random>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <algorithm>

#include "InProcServerWrapper.h"
#include <InitializeException.h>
#include <OCReflect.h>
#include <ocstack.h>


using namespace OC::OCReflect;

using namespace std;

void entityHandler(OCEntityHandlerFlag flag, OCEntityHandlerRequest* eHandlerReq)
{
    cout << "Resource Handler: " << eHandlerReq->resource << endl;
    cout << "Method: "           << eHandlerReq->method << endl;
    cout << "reqJSONPayLoad: "   << eHandlerReq->reqJSONPayload << endl;
}

namespace OC
{
    InProcServerWrapper::InProcServerWrapper(PlatformConfig cfg)
    {
        OCStackResult result = OCInit(cfg.ipAddress.c_str(), cfg.port, OC_SERVER);

        if(OC_STACK_OK != result)
        {
            throw InitializeException("Error Initializing Stack", result);
        }
		
		m_threadRun = true;
        m_processThread = std::thread(&InProcServerWrapper::processFunc, this);
    }
	
	void InProcServerWrapper::processFunc()
    {
        while(m_threadRun)
        {
			OCStackResult result;
			{
				std::lock_guard<std::mutex> lock(m_csdkLock);
				result = OCProcess();
			}

            if(result != OC_STACK_OK)
            {
                cout << "Something wrong in OCProcess" << endl;
                // TODO: SASHI
            }

            std::this_thread::yield();
            // To minimize CPU utilization we may wish to do this with sleep
			//std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void InProcServerWrapper::registerResource(const std::string& resourceURI,
                                const std::string& resourceTypeName,
                                property_binding_vector properties)
    {
        using OC::OCReflect::property_type;
        using OC::OCReflect::property_binding;
        using namespace OC::OCReflect::to_OCStack;

        std::vector<std::string> reps { convert(properties) };

        char *resourceTypeRepresentation = flatten(reps);

        std::cout << "Resource type representation: " << resourceTypeRepresentation << "\n";

        OCResourceHandle resourceHandle;

		{
			std::lock_guard<std::mutex> lock(m_csdkLock);

			OCStackResult  result;

			result = OCCreateResource(&resourceHandle, // OCResourceHandle *handl
							resourceTypeName.c_str(), // const char * resourceTypeName
							resourceTypeRepresentation, //const char * resourceTypeRepresentation
							"core.rw", //const char * resourceInterfaceName
							OC_REST_GET | OC_REST_PUT, // uint8_t allowedMethods
							resourceURI.c_str(), // const char * uri
							entityHandler, // OCEntityHandler entityHandler
							OC_DISCOVERABLE | OC_OBSERVABLE // uint8_t resourceProperties
							);

            if(result != OC_STACK_OK)
            {
                cout << "Something wrong in OCCreateResource" << endl;
                // TODO: SASHI
            }
            else
            {
                cout << "Resource creation is successful" << endl;
            }
            
		}

    }

    InProcServerWrapper::~InProcServerWrapper()
    {
		if(m_processThread.joinable())
        {
            m_threadRun = false;
            m_processThread.join();
        }
	
        OCStop();
    }
}
