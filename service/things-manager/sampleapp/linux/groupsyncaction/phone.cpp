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

static OCResourceHandle gPhoneResourceHandle = NULL;

static std::shared_ptr< OCResource > gFindGroup = NULL;

static std::string collectionResourceType = "core.group";

static ActionSet* gPlayStart;

static ActionSet* gPlayStop;

void presenceCallback(std::string msg, OCStackResult res)
{
    std::cout << "Presence Callback: " << msg << "(" << res << ")" << std::endl;
}

void onFindGroup(std::shared_ptr< OCResource > resource)
{
    if (resource)
    {
        if (NULL == gPhoneResourceHandle)
        {
            cout
                    << "onFindGroup : Error! No resource to join group. Register resource first!"
                    << endl;
            return;
        }

        if (gFindGroup)
        {
            cout << "onFindGroup : Found group is already saved." << endl;
        }
        else
        {
            cout << "onFindGroup : Found group is saved now." << endl;
            gFindGroup = resource;
            {
                OCStackResult res;
                res = gThingManager->subscribeCollectionPresence( resource, &presenceCallback);

                std::cout << "Return Value: " << res << std::endl;
            }
        }

        gThingManager->joinGroup(gFindGroup, gPhoneResourceHandle);
    }
    else
    {
        cout << "onFindGroup : Resource is invalid. So a new Group Resource has to be created."
                << endl;
    }
}

void onAction(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode)
{
    if (eCode == OC_STACK_OK)
    {
        cout << "onAction" << endl;
    }
    else
    {
        cout << "onAction : error - " << eCode << endl;
    }
}

void onGetChild(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode)
{
    if (eCode == OC_STACK_OK)
    {
        gPlayStart = new ActionSet();
        gPlayStop = new ActionSet();

        gPlayStart->actionsetName = "playstart";
        gPlayStop->actionsetName = "playstop";

        std::vector< OCRepresentation > childList = rep.getChildren();
        OCRepresentation child;
        std::string resourceURI;

        QueryParamsMap query;
        std::string playStart;
        std::string playStop;

        std::vector< std::string > types;
        std::string resType;

        OCStackResult result;

        for (unsigned int i = 0; i < childList.size(); ++i)
        {
//            cout << "\n\tchild resource - " << i + 1 << endl;

            child = childList.at(i);
            resourceURI = child.getUri();

//            cout << "\t\tURI of the resource: " << resourceURI << endl;

//            cout << "\t\tList of resource types: " << endl;

            types = child.getResourceTypes();

            for (unsigned int j = 0; j < types.size(); ++j)
            {
                resType = types.at(j);
//                cout << "\t\t\t" << resType << endl;

                if (std::string::npos != resType.find("musicplayer"))
                {
                    Capability *pStartCapa = new Capability;
                    pStartCapa->capability = "play";
                    pStartCapa->status = "on";

                    Action* pPlayStart = new Action();
                    pPlayStart->target = resourceURI;
                    pPlayStart->listOfCapability.push_back(pStartCapa);

                    gPlayStart->listOfAction.push_back(pPlayStart);

                    Capability *pStopCapa = new Capability;
                    pStopCapa->capability = "play";
                    pStopCapa->status = "off";

                    Action* pPlayStop = new Action();
                    pPlayStop->target = resourceURI;
                    pPlayStop->listOfCapability.push_back(pStopCapa);

                    gPlayStop->listOfAction.push_back(pPlayStop);
                }
                else if (std::string::npos != resType.find("speaker"))
                {
                    Capability *pStartCapa = new Capability;
                    pStartCapa->capability = "volume";
                    pStartCapa->status = "50";

                    Action* pPlayStart = new Action();
                    pPlayStart->target = resourceURI;
                    pPlayStart->listOfCapability.push_back(pStartCapa);

                    gPlayStart->listOfAction.push_back(pPlayStart);

                    Capability *pStopCapa = new Capability;
                    pStopCapa->capability = "volume";
                    pStopCapa->status = "0";

                    Action* pPlayStop = new Action();
                    pPlayStop->target = resourceURI;
                    pPlayStop->listOfCapability.push_back(pStopCapa);

                    gPlayStop->listOfAction.push_back(pPlayStop);
                }
            }
        }

//        std::string temp = gThingManager->getStringFromActionSet (gPlayStart);
//        cout << "play start - " << temp << endl;

//        temp  = gThingManager->getStringFromActionSet (gPlayStop);
//        cout << "play stop - " << temp << endl;

        if (0 == gPlayStart->listOfAction.empty())
        {
            result = gThingManager->addActionSet(gFindGroup, gPlayStart, onAction);
            if (OC_STACK_OK == result)
            {
                cout << "addActionSet(gPlayStart) was successful\n";
            }
            else
            {
                cout << "addActionSet(gPlayStart) was unsuccessful. result = " << result << endl;
            }
        }

        if (0 == gPlayStop->listOfAction.empty())
        {
            result = gThingManager->addActionSet(gFindGroup, gPlayStop, onAction);
            if (OC_STACK_OK == result)
            {
                cout << "addActionSet(gPlayStop) was successful\n";
            }
            else
            {
                cout << "addActionSet(gPlayStop) was unsuccessful. result = " << result << endl;
            }
        }
    }
    else
    {
        cout << "onGetChild : error - " << eCode << endl;
    }
}

int main(int argc, char* argv[])
{
    // Create PlatformConfig object
    PlatformConfig cfg
    { OC::ServiceType::InProc, OC::ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos };

    OCPlatform::Configure(cfg);
    gThingManager = new ThingsManager();

    int selectedMenu;
    OCStackResult result;

    try
    {
        while (true)
        {
            // some operations
            cout << "(1) CREATE PHONE" << endl;
            cout
                    << "(11) FIND & JOIN GROUP | (12) ADD GROUP ACTION | (13) PLAY START"
                    << " | (14) PLAY STOP"<< endl;
            cout << "(15) DELETE GROUP ACTION | (16) LEAVE GROUP" << endl;
            cout << "(21) DELETE PHONE" << endl;

            std::cin >> selectedMenu;

            if (selectedMenu == 1)
            {
                if (gPhoneResourceHandle)
                {
                    cout << "Phone resource is registered already." << endl;
                    continue;
                }

                std::string resourceURi = "/core/phone";
                std::string resourceTypeName = "core.phone";
                std::string resourceInterface = DEFAULT_INTERFACE;

                result = OCPlatform::registerResource(gPhoneResourceHandle, resourceURi,
                        resourceTypeName, resourceInterface,
                        NULL, OC_DISCOVERABLE | OC_OBSERVABLE);

                if (OC_STACK_OK == result)
                {
                    cout << "To register phone resource was successful\n";
                }
                else
                {
                    cout << "To register phone resource was unsuccessful. result = " << result
                            << endl;
                }
            }
            else if (selectedMenu == 11)
            {
                std::vector< std::string > types;
                types.clear();
                types.push_back(collectionResourceType);

                result = gThingManager->findGroup(types, &onFindGroup);
                if (OC_STACK_OK == result)
                {
                    cout << "Finding group was successful\n";
                }
                else
                {
                    cout << "Finding group was unsuccessful. result = " << result << endl;
                }
            }
            else if (selectedMenu == 12)
            {
                if (!gFindGroup)
                {
                    cout << "gFindGroup is NULL. Please process step 1 and step 11 first." << endl;
                    continue;
                }

                QueryParamsMap queryParamsMap;
                result = gFindGroup->get("", DEFAULT_INTERFACE, queryParamsMap, onGetChild);
                if (OC_STACK_OK == result)
                {
                    cout << "gFindGroup->get was successful\n";
                }
                else
                {
                    cout << "gFindGroup->get was unsuccessful. result = " << result << endl;
                }
            }
            else if (selectedMenu == 13)
            {
                if (!gFindGroup)
                {
                    cout << "gFindGroup is NULL. Please process step 1 and step 11 first." << endl;
                    continue;
                }

                if (!gPlayStart)
                {
                    cout << "gPlayStart is NULL. Please process step 12 first." << endl;
                    continue;
                }

                result = gThingManager->executeActionSet(gFindGroup, "playstart", onAction);
                if (OC_STACK_OK == result)
                {
                    cout << "DoAction(playstart) was successful\n";
                }
                else
                {
                    cout << "DoAction(playstart) was unsuccessful. result = " << result << endl;
                }
            }
            else if (selectedMenu == 14)
            {
                if (!gFindGroup)
                {
                    cout << "gFindGroup is NULL. Please process step 1 and step 11 first." << endl;
                    continue;
                }

                if (!gPlayStop)
                {
                    cout << "gPlayStop is NULL. Please process step 12 first." << endl;
                    continue;
                }

                result = gThingManager->executeActionSet(gFindGroup, "playstop", onAction);
                if (OC_STACK_OK == result)
                {
                    cout << "DoAction(playstop) was successful\n";
                }
                else
                {
                    cout << "DoAction(playstop) was unsuccessful. result = " << result << endl;
                }
            }
            else if (selectedMenu == 15)
            {
                if (!gFindGroup)
                {
                    cout << "gFindGroup is NULL. Please process step 1 and step 11 first." << endl;
                    continue;
                }

                if (!gPlayStart)
                {
                    cout << "gPlayStart is NULL. Please process step 12 first." << endl;
                    continue;
                }

                if (!gPlayStop)
                {
                    cout << "gPlayStop is NULL. Please process step 12 first." << endl;
                    continue;
                }

                result = gThingManager->deleteActionSet(gFindGroup, "playstart", onAction);
                if (OC_STACK_OK == result)
                {
                    cout << "Delete Action(playstart) was successful\n";
                }
                else
                {
                    cout << "Delete Action(playstart) was unsuccessful. result = " << result
                            << endl;
                }

                result = gThingManager->deleteActionSet(gFindGroup, "playstop", onAction);
                if (OC_STACK_OK == result)
                {
                    cout << "Delete Action(playstop) was successful\n";
                }
                else
                {
                    cout << "Delete Action(playstop) was unsuccessful. result = " << result << endl;
                }

                Action* a;
                Capability* c;

                for (auto actionIter = gPlayStart->listOfAction.begin();
                        actionIter != gPlayStart->listOfAction.end(); ++actionIter)
                {
                    a = (*actionIter);

                    for (auto capaIter = a->listOfCapability.begin();
                            capaIter != a->listOfCapability.end(); ++capaIter)
                    {
                        c = (*capaIter);
                        delete c;
                    }

                    delete a;
                }

                delete gPlayStart;
                gPlayStart = NULL;

                for (auto actionIter = gPlayStop->listOfAction.begin();
                        actionIter != gPlayStop->listOfAction.end(); ++actionIter)
                {
                    a = (*actionIter);

                    for (auto capaIter = a->listOfCapability.begin();
                            capaIter != a->listOfCapability.end(); ++capaIter)
                    {
                        c = (*capaIter);
                        delete c;
                    }

                    delete a;
                }

                delete gPlayStop;
                gPlayStop = NULL;
            }
            else if (selectedMenu == 16)
            {
                if (NULL == gPhoneResourceHandle)
                {
                    cout
                            << "Error! No resource to leave group. Register resource first!"
                            << endl;
                    continue;
                }

                result = gThingManager->leaveGroup(gFindGroup, collectionResourceType,  gPhoneResourceHandle);
                if (OC_STACK_OK == result)
                {
                    cout << "Leaving group was successful\n";
                }
                else
                {
                    cout << "Leaving group was unsuccessful. result = " << result << endl;
                }
            }
            else if (selectedMenu == 21)
            {
                if (NULL == gPhoneResourceHandle)
                {
                    cout
                            << "Error! No resource to unregister. Register resource first!"
                            << endl;
                    continue;
                }

                result = OCPlatform::unregisterResource(gPhoneResourceHandle);
                if (OC_STACK_OK == result)
                {
                    cout << "To unregister phone resource was successful\n";
                }
                else
                {
                    cout << "To unregister phone resource was unsuccessful. result = " << result
                            << endl;
                }
                gPhoneResourceHandle = NULL;
            }
        }

    }
    catch (OCException& e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
    }

    return 0;
}

