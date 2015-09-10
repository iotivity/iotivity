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

/// @file mqttclient.cpp

/// @brief Samplecode which controls MQTT-fan plugin using Protocol Plugin Manager.

#include <string>
#include <cstdlib>
#include <pthread.h>
#include "OCPlatform.h"
#include "PluginManager.h"
#include "OCApi.h"

using namespace OC;
using namespace OIC;

const int SUCCESS_RESPONSE = 0;
std::shared_ptr<OCResource> curFanResource;
static ObserveType OBSERVE_TYPE_TO_USE = ObserveType::Observe;
int count = 0;
void putFanRepresentation(std::shared_ptr<OCResource> resource);

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
        std::cout << "\tSequenceNumber: " << sequenceNumber << std::endl;

        rep.getValue("state", myfan.m_state);
        rep.getValue("power", myfan.m_power);
        rep.getValue("name", myfan.m_name);

        std::cout << "\tstate: " << myfan.m_state << std::endl;
        std::cout << "\tpower: " << myfan.m_power << std::endl;
        std::cout << "\tname: " << myfan.m_name << std::endl;

        if (observe_count() > 30)
        {
            std::cout << "Cancelling Observe..." << std::endl;
            OCStackResult result = curFanResource->cancelObserve();

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
            std::cout << std::endl << "Observe is used." << std::endl << std::endl;
        else if (OBSERVE_TYPE_TO_USE == ObserveType::ObserveAll)
            std::cout << std::endl << "ObserveAll is used." << std::endl << std::endl;

        //curFanResource->observe(OBSERVE_TYPE_TO_USE, QueryParamsMap(), &onObserve);

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

        curFanResource->post(rep2, QueryParamsMap(), &onPost2);
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

        putFanRepresentation(curFanResource);
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
        if (myfan.m_power == 1)
            myfan.m_power = 0;
        else
            myfan.m_power = 1;
        sleep(5);
        rep.setValue("state", myfan.m_state);
        rep.setValue("power", myfan.m_power);

        // Invoke resource's put API with rep, query map and the callback parameter
        resource->put(rep, QueryParamsMap(), &onPut);
    }
}

// Callback handler on GET request
void onFanGet(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    if (eCode == OC_STACK_OK)
    {
        std::cout << "GET Fan request was successful" << std::endl;
        std::cout << "Resource URI: " << rep.getUri() << std::endl;

        rep.getValue("state", myfan.m_state);
        rep.getValue("power", myfan.m_power);
        rep.getValue("name", myfan.m_name);

        std::cout << "\tstate: " << myfan.m_state << std::endl;
        std::cout << "\tpower: " << myfan.m_power << std::endl;
        std::cout << "\tname: " << myfan.m_name << std::endl;

        putFanRepresentation(curFanResource);
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
        resource->get(test, &onFanGet);
    }
}

// Callback to found resources
void foundResourceFan(std::shared_ptr<OCResource> resource)
{
    if (curFanResource)
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
                curFanResource = resource;
                // Call a local function which will internally invoke get API on the resource pointer
                putFanRepresentation(curFanResource);
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
        std::cout << "Exception: " << e.what() << std::endl;
    }
}

void PrintUsage()
{
    std::cout << std::endl;
    std::cout << "Usage : simpleclient <ObserveType>" << std::endl;
    std::cout << "   ObserveType : 1 - Observe" << std::endl;
    std::cout << "   ObserveType : 2 - ObserveAll" << std::endl;
}



int main(int argc, char *argv[])
{
    std::string name;
    std::string key = "Name";
    std::string  state = "";
    std::string  id = "";
    std::ostringstream requestURI;

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
        PluginManager *m_pm = new PluginManager();
        std::cout << "start light Plugin by Resource Type"  << std::endl;
        m_pm->startPlugins("ResourceType", "oic.fan");
        sleep(2);
        // makes it so that all boolean values are printed as 'true/false' in this stream
        std::cout.setf(std::ios::boolalpha);
        // Find all resources
        requestURI << OC_RSRVD_WELL_KNOWN_URI << "?rt=core.fan";
        OCPlatform::findResource("", requestURI.str(), CT_DEFAULT, &foundResourceFan);
        std::cout << "Finding Resource... " << std::endl;
        while (true)
        {
            // some operations
        }

    }
    catch (OCException &e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
