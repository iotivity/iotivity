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


// OCClient.cpp : Defines the entry point for the console application.
//
#include <string>
#include <cstdlib>
#include <pthread.h>
#include "OCPlatform.h"
#include "PluginManager.h"
#include "OCApi.h"
#include <time.h>

using namespace OC;
using namespace OIC;

std::shared_ptr<OCResource> curResource;
static ObserveType OBSERVE_TYPE_TO_USE = ObserveType::Observe;

time_t timer;                // Define the timer
struct tm *tblock;           // Define a structure for time block

class Fan
{
    public:

        bool m_state;
        int m_power;
        std::string m_name;

        Fan() : m_state(false), m_power(0), m_name("")
        {
        }
};

Fan myfan;

int observe_count()
{
    static int oc = 0;
    return ++oc;
}

void onObserve(const HeaderOptions headerOptions, const OCRepresentation &rep,
               const int &eCode, const int &sequenceNumber)
{
    if (eCode == OC_STACK_OK)
    {
        std::cout << "OBSERVE RESULT:" << std::endl;
        std::cout << "\tSequenceNumber: " << sequenceNumber << endl;

        rep.getValue("state", myfan.m_state);
        rep.getValue("power", myfan.m_power);
        rep.getValue("name", myfan.m_name);

        std::cout << "\tstate: " << myfan.m_state << std::endl;
        std::cout << "\tpower: " << myfan.m_power << std::endl;
        std::cout << "\tname: " << myfan.m_name << std::endl;

        if (observe_count() > 30)
        {
            std::cout << "Cancelling Observe..." << std::endl;
            OCStackResult result = curResource->cancelObserve();

            std::cout << "Cancel result: " << result << std::endl;
            sleep(10);
            std::cout << "DONE" << std::endl;
            std::exit(0);
        }
    }
    else
    {
        std::cout << "onObserve Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}

void onPost2(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    if (eCode == OC_STACK_OK || eCode == OC_STACK_RESOURCE_CREATED)
    {
        std::cout << "POST request was successful" << std::endl;

        if (rep.hasAttribute("createduri"))
        {
            std::cout << "\tUri of the created resource: "
                      << rep.getValue<std::string>("createduri") << std::endl;
        }
        else
        {
            rep.getValue("state", myfan.m_state);
            rep.getValue("power", myfan.m_power);
            rep.getValue("name", myfan.m_name);

            std::cout << "\tstate: " << myfan.m_state << std::endl;
            std::cout << "\tpower: " << myfan.m_power << std::endl;
            std::cout << "\tname: " << myfan.m_name << std::endl;
        }

        if (OBSERVE_TYPE_TO_USE == ObserveType::Observe)
            std::cout << endl << "Observe is used." << endl << endl;
        else if (OBSERVE_TYPE_TO_USE == ObserveType::ObserveAll)
            std::cout << endl << "ObserveAll is used." << endl << endl;

        curResource->observe(OBSERVE_TYPE_TO_USE, QueryParamsMap(), &onObserve);

    }
    else
    {
        std::cout << "onPost Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}

void onPost(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    if (eCode == OC_STACK_OK || eCode == OC_STACK_RESOURCE_CREATED)
    {
        std::cout << "POST request was successful" << std::endl;

        if (rep.hasAttribute("createduri"))
        {
            std::cout << "\tUri of the created resource: "
                      << rep.getValue<std::string>("createduri") << std::endl;
        }
        else
        {
            rep.getValue("state", myfan.m_state);
            rep.getValue("power", myfan.m_power);
            rep.getValue("name", myfan.m_name);

            std::cout << "\tstate: " << myfan.m_state << std::endl;
            std::cout << "\tpower: " << myfan.m_power << std::endl;
            std::cout << "\tname: " << myfan.m_name << std::endl;
        }

        OCRepresentation rep2;

        std::cout << "Posting fan representation..." << std::endl;

        myfan.m_state = true;
        myfan.m_power = 55;

        rep2.setValue("state", myfan.m_state);
        rep2.setValue("power", myfan.m_power);

        curResource->post(rep2, QueryParamsMap(), &onPost2);
    }
    else
    {
        std::cout << "onPost Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}

// Local function to put a different state for this resource
void postFanRepresentation(std::shared_ptr<OCResource> resource)
{
    if (resource)
    {
        OCRepresentation rep;

        std::cout << "Posting fan representation..." << std::endl;

        myfan.m_state = false;
        myfan.m_power = 105;

        rep.setValue("state", myfan.m_state);
        rep.setValue("power", myfan.m_power);

        // Invoke resource's post API with rep, query map and the callback parameter
        resource->post(rep, QueryParamsMap(), &onPost);
    }
}

// callback handler on PUT request
void onPut(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    if (eCode == OC_STACK_OK)
    {
        std::cout << "PUT request was successful" << std::endl;

        rep.getValue("state", myfan.m_state);
        rep.getValue("power", myfan.m_power);
        rep.getValue("name", myfan.m_name);

        std::cout << "\tstate: " << myfan.m_state << std::endl;
        std::cout << "\tpower: " << myfan.m_power << std::endl;
        std::cout << "\tname: " << myfan.m_name << std::endl;

        postFanRepresentation(curResource);
    }
    else
    {
        std::cout << "onPut Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}

// Local function to put a different state for this resource
void putFanRepresentation(std::shared_ptr<OCResource> resource)
{
    if (resource)
    {
        OCRepresentation rep;

        std::cout << "Putting fan representation..." << std::endl;

        myfan.m_state = true;
        myfan.m_power = 15;

        rep.setValue("state", myfan.m_state);
        rep.setValue("power", myfan.m_power);

        // Invoke resource's put API with rep, query map and the callback parameter
        resource->put(rep, QueryParamsMap(), &onPut);
    }
}

// Callback handler on GET request
void onGet(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    if (eCode == OC_STACK_OK)
    {
        std::cout << "GET request was successful" << std::endl;
        std::cout << "Resource URI: " << rep.getUri() << std::endl;

        rep.getValue("state", myfan.m_state);
        rep.getValue("power", myfan.m_power);
        rep.getValue("name", myfan.m_name);

        std::cout << "\tstate: " << myfan.m_state << std::endl;
        std::cout << "\tpower: " << myfan.m_power << std::endl;
        std::cout << "\tname: " << myfan.m_name << std::endl;

        putFanRepresentation(curResource);
    }
    else
    {
        std::cout << "onGET Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}

// Local function to get representation of fan resource
void getFanRepresentation(std::shared_ptr<OCResource> resource)
{
    if (resource)
    {
        std::cout << "Getting Fan Representation..." << std::endl;
        // Invoke resource's get API with the callback parameter

        QueryParamsMap test;
        resource->get(test, &onGet);
    }
}

void getLightRepresentation(std::shared_ptr<OCResource> resource)
{
    if (resource)
    {
        std::cout << "Getting Light Representation..." << std::endl;
        // Invoke resource's get API with the callback parameter

        QueryParamsMap test;
        resource->get(test, &onGet);
    }
}

// Callback to found resources
void foundResourceFan(std::shared_ptr<OCResource> resource)
{
    if (curResource)
    {
        std::cout << "Found another resource, ignoring" << std::endl;
    }

    std::string resourceURI;
    std::string hostAddress;
    try
    {
        // Do some operations with resource object.
        if (resource)
        {
            std::cout << "DISCOVERED Resource:" << std::endl;
            // Get the resource URI
            resourceURI = resource->uri();
            std::cout << "\tURI of the resource: " << resourceURI << std::endl;

            // Get the resource host address
            hostAddress = resource->host();
            std::cout << "\tHost address of the resource: " << hostAddress << std::endl;

            // Get the resource types
            std::cout << "\tList of resource types: " << std::endl;
            for (auto &resourceTypes : resource->getResourceTypes())
            {
                std::cout << "\t\t" << resourceTypes << std::endl;
            }

            // Get the resource interfaces
            std::cout << "\tList of resource interfaces: " << std::endl;
            for (auto &resourceInterfaces : resource->getResourceInterfaces())
            {
                std::cout << "\t\t" << resourceInterfaces << std::endl;
            }

            if (resourceURI == "/a/fan")
            {
                curResource = resource;
                // Call a local function which will internally invoke get API on the resource pointer
                getFanRepresentation(resource);
            }
        }
        else
        {
            // Resource is invalid
            std::cout << "Resource is invalid" << std::endl;
        }

    }
    catch (std::exception &e)
    {
        //log(e.what());
    }
}

// Callback to found resources
void foundResourceLight(std::shared_ptr<OCResource> resource)
{
    if (curResource)
    {
        std::cout << "Found another resource, ignoring" << std::endl;
    }

    std::string resourceURI;
    std::string hostAddress;
    try
    {
        // Do some operations with resource object.
        if (resource)
        {
            std::cout << "DISCOVERED Resource:" << std::endl;
            // Get the resource URI
            resourceURI = resource->uri();
            std::cout << "\tURI of the resource: " << resourceURI << std::endl;

            // Get the resource host address
            hostAddress = resource->host();
            std::cout << "\tHost address of the resource: " << hostAddress << std::endl;

            // Get the resource types
            std::cout << "\tList of resource types: " << std::endl;
            for (auto &resourceTypes : resource->getResourceTypes())
            {
                std::cout << "\t\t" << resourceTypes << std::endl;
            }

            // Get the resource interfaces
            std::cout << "\tList of resource interfaces: " << std::endl;
            for (auto &resourceInterfaces : resource->getResourceInterfaces())
            {
                std::cout << "\t\t" << resourceInterfaces << std::endl;
            }

            if (resourceURI == "/a/light")
            {
                curResource = resource;
                // Call a local function which will internally invoke get API on the resource pointer
                getLightRepresentation(resource);
            }
        }
        else
        {
            // Resource is invalid
            std::cout << "Resource is invalid" << std::endl;
        }

    }
    catch (std::exception &e)
    {
        //log(e.what());
    }
}

void PrintUsage()
{
    std::cout << std::endl;
    std::cout << "Usage : simpleclient <ObserveType>" << std::endl;
    std::cout << "   ObserveType : 1 - Observe" << std::endl;
    std::cout << "   ObserveType : 2 - ObserveAll" << std::endl;
}

void client1()
{
    std::cout << "in client1\n";

    // OCPlatform::findResource("", "coap://224.0.1.187/oc/core?rt=core.foo",
    //                                 foundResource1);
    std::cout << "starting findResource = core.fan" << std::endl;
    // Find all resources
    OCPlatform::findResource("", "coap://224.0.1.187/oc/core?rt=core.fan", &foundResourceFan);
    // Get time of day
    timer = time(NULL);
    // Converts date/time to a structure
    tblock = localtime(&timer);
    std::cout << "Finding Fan Resource... time : " << asctime(tblock) << std::endl;
    while (1)
    {
        // client1 related operations
    }
}

void client2()
{
    std::cout << "in client2\n";

    // OCPlatform::findResource("", "coap://224.0.1.187/oc/core?rt=core.foo",
    //             foundResource2);

    std::cout << "starting findResource = core.light" << std::endl;
    // Find all resources
    OCPlatform::findResource("", "coap://224.0.1.187/oc/core?rt=core.light", &foundResourceLight);
    // Get time of day
    timer = time(NULL);
    // Converts date/time to a structure
    tblock = localtime(&timer);
    std::cout << "Finding Light Resource... time : " << asctime(tblock) << std::endl;



    while (1)
    {
        // client2 related operations
    }
}



int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        OBSERVE_TYPE_TO_USE = ObserveType::Observe;
    }
    else if (argc == 2)
    {
        int value = atoi(argv[1]);
        if (value == 1)
            OBSERVE_TYPE_TO_USE = ObserveType::Observe;
        else if (value == 2)
            OBSERVE_TYPE_TO_USE = ObserveType::ObserveAll;
        else
            OBSERVE_TYPE_TO_USE = ObserveType::Observe;
    }
    else
    {
        PrintUsage();
        return -1;
    }

    // Create PlatformConfig object
    PlatformConfig cfg
    {
        OC::ServiceType::InProc,
        OC::ModeType::Both,
        "0.0.0.0",
        0,
        OC::QualityOfService::LowQos
    };

    OCPlatform::Configure(cfg);

    try
    {

        std::cout << "Created Platform..." << std::endl;


        PluginManager *m_pm = new PluginManager();

        std::cout << "======================" << std::endl;

        std::cout << "start light Plugin by Resource Type"  << std::endl;
        m_pm->startPlugins("ResourceType", "oic.light");

        sleep(2);

        std::cout << "======================" << std::endl;
        std::cout << "get Plugin List" << std::endl;
        std::vector<Plugin> user_plugin;
        user_plugin = m_pm->getPlugins();
        for (unsigned int i = 0; i < user_plugin.size(); i++)
        {
            printf("value Name = %s\n", user_plugin[i].getName().c_str());
            printf("value ID = %s\n", user_plugin[i].getID().c_str());
        }

        std::cout << "======================" << std::endl;
        std::cout << "start Fan Plugin by Name" << std::endl;
        m_pm->startPlugins("Name", "mqtt-fan");


        // Start each client in a seperate thread
//        std::thread t1(client1);
//        t1.detach();

///*
//        sleep(5);
        // Start each client in a seperate thread
//        std::thread t2(client2);
//        t2.detach();
//*/
        while (true)
        {
            // some operations
        }

    }
    catch (OCException &e)
    {
        //log(e.what());
    }

    return 0;
}

