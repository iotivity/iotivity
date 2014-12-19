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

#include <OCPlatform.h>
#include <OCApi.h>

#include <functional>
#include <pthread.h>
#include <iostream>

#include <ThingsManager.h>

using namespace std;
using namespace OC;
using namespace OIC;
namespace PH = std::placeholders;

bool isReady = false;

OCResourceHandle resourceHandle;
std::vector< OCResourceHandle > resourceHandleVector;

shared_ptr< OCResource > g_resource;
vector< string > lights;

ThingsManager *thingsMgr = new ThingsManager();

void onGet(const HeaderOptions& opt, const OCRepresentation &rep, const int eCode);

void onPut(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode);

void onPost(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode);

void onObserve(const HeaderOptions headerOptions, const OCRepresentation& rep, const int& eCode,
        const int& sequenceNumber);

void allBulbOn();
void allBulbOff();

void foundResources(std::vector< std::shared_ptr< OC::OCResource > > listOfResource)
{

    for (auto rsrc = listOfResource.begin(); rsrc != listOfResource.end(); ++rsrc)
    {
        std::string resourceURI = (*rsrc)->uri();
        std::string hostAddress = (*rsrc)->host();

        if (resourceURI == "/a/light")
        {

            cout << "\tResource URI : " << resourceURI << endl;
            cout << "\tResource Host : " << hostAddress << endl;

            OCResourceHandle foundResourceHandle;
            OCStackResult result = OCPlatform::registerResource(foundResourceHandle, (*rsrc));
            cout << "\tresource registed!" << endl;
            if (result == OC_STACK_OK)
            {
                OCPlatform::bindResource(resourceHandle, foundResourceHandle);
                resourceHandleVector.push_back(foundResourceHandle);
            }
            else
            {
                cout << "\tresource Error!" << endl;
            }

            lights.push_back((hostAddress + resourceURI));
        }
    }

    isReady = true;
}

void foundResource(std::shared_ptr< OCResource > resource)
{
    std::string resourceURI;
    std::string hostAddress;

    try
    {
        cout << "FOUND RESOURCE" << endl;

        if (resource)
        {
            resourceURI = resource->uri();
            hostAddress = resource->host();
            if (resourceURI == "/core/a/collection")
            {
                g_resource = resource;

                // g_resource->get("", DEFAULT_INTERFACE, QueryParamsMap(), onGet);

                printf("\tHOST :: %s\n", resource->host().c_str());
            }
            else if (resourceURI == "/core/bookmark")
            {
                resource->observe(ObserveType::Observe, QueryParamsMap(), &onObserve);
            }

            // p_platform.bindResource(resourceHandle, foundResourceHandle);

        }
    }
    catch (std::exception& e)
    {
        std::cout << "" << std::endl;
    }
}

void onGet(const HeaderOptions& opt, const OCRepresentation &rep, const int eCode)
{
    // std::vector<OCRepresentation> children = rep.getChildren();

    // cout << "\n\n\nCHILD RESOURCE OF GROUP" << endl;
    // for( auto iter = children.begin(); iter != children.end();  ++iter )
    // {
    //     lights.push_back((*iter).getUri());
    //     cout << "\tURI :: " << (*iter).getUri() << endl;
    // }
}

void onPut(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode)
{
    printf("\nonPut\n");
}

void onPost(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode)
{
    printf("\nonPost\n");

    if (rep.hasAttribute("ActionSet"))
    {
        std::string plainText;

        if (rep.getValue("ActionSet", plainText))
        {
            ActionSet *actionset = thingsMgr->getActionSetfromString(plainText);
            if (actionset != NULL)
            {
                cout << endl << "\tACTIONSET NAME :: " << actionset->actionsetName << endl;
                for (auto actIter = actionset->listOfAction.begin();
                        actIter != actionset->listOfAction.end(); ++actIter)
                {
                    cout << "\t\tTARGET :: " << (*actIter)->target << endl;

                    for (auto capaIter = (*actIter)->listOfCapability.begin();
                            capaIter != (*actIter)->listOfCapability.end(); ++capaIter)
                    {
                        cout << "\t\t\t" << (*capaIter)->capability << " :: " << (*capaIter)->status
                                << endl;
                    }
                }
            }
            delete actionset;
        }

        // printf( "\tPlain Text :: %s\n", plainText.c_str() );
    }
    else if (rep.hasAttribute("DoAction"))
    {
        std::string plainText;
        if (rep.getValue("DoAction", plainText))
        {
            cout << "\t" << plainText << endl;
        }
    }
    else
    {

    }
}

void allBulbOff()
{
    OCRepresentation rep;

    rep.setValue("DoAction", std::string("AllBulbOff"));

    if (g_resource)
    {
        g_resource->post("a.collection", GROUP_INTERFACE, rep, QueryParamsMap(), &onPost);
    }
}

void allBulbOn()
{
    OCRepresentation rep;

    rep.setValue("DoAction", std::string("AllBulbOn"));

    if (g_resource)
    {
        g_resource->post("a.collection", GROUP_INTERFACE, rep, QueryParamsMap(), &onPost);
    }
}

void onObserve(const HeaderOptions headerOptions, const OCRepresentation& rep, const int& eCode,
        const int& sequenceNumber)
{
    if (eCode == OC_STACK_OK)
    {
        int level;

        std::cout << "OBSERVE RESULT:" << std::endl;
        std::cout << "\tSequenceNumber: " << sequenceNumber << endl;

        if (rep.getValue("level", level))
        {
            if (level == 0)
            {
                allBulbOn();
            }
            else
            {
                allBulbOff();
            }
        }
        std::cout << "\tlevel: " << level << std::endl;
    }
    else
    {
        std::cout << "onObserve Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}

void createActionSet_AllBulbOff()
{
    string actionsetDesc;
    ActionSet *allBulbOff = new ActionSet();
    allBulbOff->actionsetName = "AllBulbOff";

    for (auto iter = lights.begin(); iter != lights.end(); ++iter)
    {
        Action *action = new Action();
        action->target = (*iter);

        Capability *capa = new Capability();
        capa->capability = "power";
        capa->status = "off";

        action->listOfCapability.push_back(capa);
        allBulbOff->listOfAction.push_back(action);
    }
    // actionsetDesc = thingsMgr->getStringFromActionSet(allBulbOff);

    // cout << "ActionSet :: " << actionsetDesc << endl;

    // OCRepresentation rep;
    // rep.setValue("ActionSet", actionsetDesc);

    if (g_resource)
    {
        thingsMgr->addActionSet(g_resource, allBulbOff, onPut);
        // g_resource->put("a.collection", GROUP_INTERFACE, rep,
        //     QueryParamsMap(), &onPut);
    }

    delete allBulbOff;
}

void createActionSet_AllBulbOn()
{
    string actionsetDesc;
    ActionSet *allBulbOff = new ActionSet();
    allBulbOff->actionsetName = "AllBulbOn";

    for (auto iter = lights.begin(); iter != lights.end(); ++iter)
    {
        Action *action = new Action();
        action->target = (*iter);

        Capability *capa = new Capability();
        capa->capability = "power";
        capa->status = "on";

        action->listOfCapability.push_back(capa);
        allBulbOff->listOfAction.push_back(action);
    }
    // actionsetDesc = thingsMgr->getStringFromActionSet(allBulbOff);

    // cout << "ActionSet :: " << actionsetDesc << endl;

    // OCRepresentation rep;
    // rep.setValue("ActionSet", actionsetDesc);

    if (g_resource)
    {
        thingsMgr->addActionSet(g_resource, allBulbOff, onPut);
        // g_resource->put("a.collection", GROUP_INTERFACE, rep,
        //     QueryParamsMap(), &onPut);
    }

    delete allBulbOff;
}

int main()
{
    PlatformConfig config
    { OC::ServiceType::InProc, ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos };

    try
    {
        string resourceURI = "/core/a/collection";
        string resourceTypeName = "a.collection";
        string resourceInterface = BATCH_INTERFACE;
        OCPlatform::Configure(config);

        // Find lights for group creation.
        vector< string > types;
        types.push_back("core.light");
        thingsMgr->findCandidateResources(types, &foundResources, 5);

        OCPlatform::registerResource(resourceHandle, resourceURI, resourceTypeName,
                resourceInterface, NULL,
                //&entityHandler, // entityHandler
                OC_DISCOVERABLE);

        cout << "registerResource is called." << endl;

        OCPlatform::bindInterfaceToResource(resourceHandle, GROUP_INTERFACE);
        OCPlatform::bindInterfaceToResource(resourceHandle, DEFAULT_INTERFACE);

        bool isRun = true;

        while (isRun)
        {
            while (isReady)
            {
                int n;

                cout << endl;
                cout << "1 :: CREATE ACTIONSET 2 :: EXECUTE ACTIONSET(ALLBULBON)"
                        << "3 :: EXECUTE ACTIONSET(ALLBULBOFF)" << endl;
                cout << "4 :: GET ACTIONSET 5 :: DELETE ACTIONSET 6 :: QUIT" << endl;
                cout << "9 :: FIND GROUP 0 :: FIND BOOKMARK TO OBSERVE" << endl;

                fflush(stdin);
                cin >> n;

                if (n == 9)
                {
                    OCPlatform::findResource("", "coap://224.0.1.187/oc/core?rt=a.collection",
                            &foundResource);
                }
                else if (n == 0)
                {
                    OCPlatform::findResource("", "coap://224.0.1.187/oc/core?rt=core.bookmark",
                            &foundResource);
                }
                else if (n == 1)
                {

                    // Craete Action Set
                    // "AllBulbOff"
                    //"movieTime*uri=coap://10.251.44.228:49858/a/light|power=10";
                    createActionSet_AllBulbOff();
                    createActionSet_AllBulbOn();

                }
                else if (n == 2)
                {

                    allBulbOn();
                    // thingsMgr->executeActionSet(g_resource, "AllBulbOn", onPost);

                }
                else if (n == 3)
                {

                    allBulbOff();
                    // thingsMgr->executeActionSet(g_resource, "AllBulbOff", onPost);

                }
                else if (n == 4)
                {
                    // OCRepresentation rep;

                    // rep.setValue("GetActionSet", std::string("AllBulbOff"));

                    // if(g_resource)
                    // {
                    //     g_resource->post("a.collection", GROUP_INTERFACE, rep,
                    //         QueryParamsMap(), &onPost);
                    // }

                    thingsMgr->getActionSet(g_resource, "AllBulbOff", onPost);
                }
                else if (n == 5)
                {
                    // OCRepresentation rep;

                    // rep.setValue("DelActionSet", std::string("AllBulbOff"));

                    // if(g_resource)
                    // {
                    //     g_resource->put("a.collection", GROUP_INTERFACE, rep,
                    //         QueryParamsMap(), &onPut);
                    // }
                    thingsMgr->deleteActionSet(g_resource, "AllBulbOff", onPut);
                }
                else if (n == 6)
                {
                    isRun = false;
                    break;
                }
            }
        }
    }
    catch (OCException& e)
    {

    }

    return 0;
}
