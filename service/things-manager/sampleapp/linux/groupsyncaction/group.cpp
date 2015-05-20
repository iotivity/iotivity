//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

#include <string>
#include <cstdlib>
#include <pthread.h>
#include "OCPlatform.h"
#include "OCApi.h"
#include "ThingsManager.h"

using namespace OC;
using namespace OIC;

static ThingsManager* gThingManager = NULL;

static std::vector< OCResourceHandle > gResourceHandleList;

static std::string collectionResourceType = "core.group";

void onFindResource(std::shared_ptr< OCResource > resource)
{
    cout << "onFindResource" << endl;

    if (resource)
    {
        OCResourceHandle resourceHandle;
        OCStackResult result = OCPlatform::registerResource(resourceHandle, resource);
        if (OC_STACK_OK == result)
        {
            cout << "onFindResource : Resource creation was successful\n";
        }
        else
        {
            cout << "onFindResource : Resource creation was unsuccessful\n";
            return;
        }

        result = gThingManager->joinGroup(collectionResourceType, resourceHandle);
        if (OC_STACK_OK == result)
        {
            cout << "onFindResource : Joining group was successful\n";
        }
        else
        {
            cout << "onFindResource : Joining group was unsuccessful\n";

            OCPlatform::unregisterResource(resourceHandle);
            return;
        }

        gResourceHandleList.push_back(resourceHandle);
    }
    else
    {
        cout << "onFindResource : There is no found resource." << endl;
    }
}

int main(int argc, char* argv[])
{
    // Create PlatformConfig object
    PlatformConfig cfg
    { OC::ServiceType::InProc, OC::ModeType::Both/*OC::ModeType::Server*/, "0.0.0.0", 0,
            OC::QualityOfService::LowQos };

    OCPlatform::Configure(cfg);
    gThingManager = new ThingsManager();

    int selectedMenu;
    OCStackResult result;

    try
    {
        while (true)
        {
            // some operations
            cout << "(1) CREATE GROUP " << endl;
            cout << "(11) FIND MUSIC PLAYER & JOIN GROUP | (12) FIND SPEAKER & JOIN GROUP" << endl;
            cout << "(21) LEAVE GROUP - MUSIC PLAYER  | (22) LEAVE GROUP - SPEAKER" << endl;
            cout << "(31) DELETE GROUP " << endl;

            std::cin >> selectedMenu;

            if (selectedMenu == 1)
            {
                result = gThingManager->createGroup(collectionResourceType);
                if (OC_STACK_OK == result)
                {
                    cout << "Group creation was successful\n";
                }
                else
                {
                    cout << "Group creation was unsuccessful\n";
                }
            }
            else if (selectedMenu == 11)
            {
                ostringstream query;
                query << OC_MULTICAST_DISCOVERY_URI << "?rt=core.musicplayer";

                cout << query.str() << endl;
                result = OCPlatform::findResource("",
                            query.str(),
                            OC_ALL,
                            onFindResource);

                result = OCPlatform::findResource("",
                            "coap://224.0.1.187/oc/core?rt=core.musicplayer",
                            OC_ALL,
                            onFindResource);

                if (OC_STACK_OK == result)
                {
                    cout << "Finding music player was successful\n";
                }
                else
                {
                    cout << "Finding music player was unsuccessful\n";
                }
            }
            else if (selectedMenu == 12)
            {
                ostringstream query;
                query << OC_MULTICAST_DISCOVERY_URI << "?rt=core.speaker";
                result = OCPlatform::findResource("",
                            query.str(),
                            OC_ALL,
                            onFindResource);

                if (OC_STACK_OK == result)
                {
                    cout << "Finding speaker was successful\n";
                }
                else
                {
                    cout << "Finding speaker was unsuccessful\n";
                }
            }
            else if (selectedMenu == 21)
            {
                std::vector< OCResourceHandle >::iterator It;
                OCResourceHandle resourceHandle;
                for (It = gResourceHandleList.begin(); It != gResourceHandleList.end(); ++It)
                {
                    resourceHandle = (*It);
                    std::string mpType = "core.musicplayer";
                    std::string type = OCGetResourceTypeName(resourceHandle, 0);
                    if (0 == mpType.compare(type))
                    {
                        result = gThingManager->leaveGroup(collectionResourceType, resourceHandle);
                        if (OC_STACK_OK == result)
                        {
                            cout << "Leaving group of music player was successful\n";
                        }
                        else
                        {
                            cout << "Leaving group of music player was unsuccessful\n";
                        }
                        break;
                    }
                }

                if(It == gResourceHandleList.end())
                    continue;

                gResourceHandleList.erase(It);
                result = OCPlatform::unregisterResource(resourceHandle);
                if (OC_STACK_OK == result)
                {
                    cout << "Unregistering music player was successful\n";
                }
                else
                {
                    cout << "Unregistering music player was unsuccessful\n";
                }
            }
            else if (selectedMenu == 22)
            {
                std::vector< OCResourceHandle >::iterator It;
                OCResourceHandle resourceHandle;
                for (It = gResourceHandleList.begin(); It != gResourceHandleList.end(); ++It)
                {
                    resourceHandle = (*It);
                    std::string mpType = "core.speaker";
                    std::string type = OCGetResourceTypeName(resourceHandle, 0);
                    if (0 == mpType.compare(type))
                    {
                        result = gThingManager->leaveGroup(collectionResourceType, resourceHandle);
                        if (OC_STACK_OK == result)
                        {
                            cout << "Leaving group of speaker was successful\n";
                        }
                        else
                        {
                            cout << "Leaving group of speaker was unsuccessful\n";
                        }
                        break;
                    }
                }

                if(It == gResourceHandleList.end())
                    continue;

                gResourceHandleList.erase(It);
                result = OCPlatform::unregisterResource(resourceHandle);
                if (OC_STACK_OK == result)
                {
                    cout << "Unregistering speaker was successful\n";
                }
                else
                {
                    cout << "Unregistering speaker was unsuccessful\n";
                }
            }
            else if (selectedMenu == 31)
            {
                gThingManager->deleteGroup(collectionResourceType);
            }
        }
    }
    catch (OCException& e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
    }

    return 0;
}

