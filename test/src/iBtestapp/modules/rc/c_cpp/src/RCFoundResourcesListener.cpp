/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include "RCFoundResourcesListener.h"

RCFoundResourcesListener* RCFoundResourcesListener::s_rcFoundResourceListener = NULL;

using namespace OC;
using namespace std;

bool RCFoundResourcesListener::g_foundResource = false;
std::mutex RCFoundResourcesListener::s_mutexFoundListener;

RCFoundResourcesListener::RCFoundResourcesListener()
{

}

RCFoundResourcesListener* RCFoundResourcesListener::getInstance(void)
{
    s_rcFoundResourceListener = new RCFoundResourcesListener();
    return s_rcFoundResourceListener;
}

RCFoundResourcesListener::~RCFoundResourcesListener()
{
    delete s_rcFoundResourceListener;
}

void RCFoundResourcesListener::rcFoundResourceCB(std::shared_ptr< OC::OCResource > resource)
{
    s_mutexFoundListener.lock();

    std::string hostAddress;
    std::string resourceURI;

    try
    {
        if (resource)
        {
            for (auto &resourceTypes : resource->getResourceTypes())
            {
                if ((resourceTypes == TEST_RESOURCE_TYPE_HUESAMPLE)
                        || (resourceTypes == TEST_RESOURCE_TYPE_BMISENSOR)
                        || (resourceTypes == TEST_RESOURCE_TYPE_HUMIDITY)
                        || (resourceTypes == TEST_RESOURCE_TYPE_TEMPERATURE)
                        || (resourceTypes == TEST_RESOURCE_TYPE_WEIGHT)
                        || (resourceTypes == TEST_RESOURCE_TYPE_HEIGHT))
                {
                    IBtestappCommonUtil::isTestAppCommonUtilServerResponsed = true;
                    g_foundResource = true;

                    std::cout << "\t Founded resource type : " << resourceTypes << std::endl;

                    resourceURI = resource->uri();
                    std::cout << "\t URI of the founded resource : " << resourceURI << std::endl;

                    hostAddress = resource->host();
                    std::cout << "\t Host address of the resource: " << hostAddress << std::endl;
                }
            }
        }
        else
        {
            std::cout << "Founded resource is invalid.." << std::endl;
            g_foundResource = false;
        }

    }
    catch (std::exception &e)
    {
        g_foundResource = false;
        cout << "Exception in rcFoundResourceCB: " << e.what() << std::endl;
    }
    s_mutexFoundListener.unlock();
}
