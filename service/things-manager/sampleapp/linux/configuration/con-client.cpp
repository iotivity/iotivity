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
#include <map>
#include <vector>
#include "OCPlatform.h"
#include "OCApi.h"
#include "ThingsManager.h"

using namespace OC;
using namespace OIC;

int g_Steps = 0;
int isWaiting = 0; //0: none to wait, 1: wait for the response of "getConfigurationValue"

static ThingsManager* g_thingsmanager;

OCResourceHandle configurationCollectionHandle;
std::shared_ptr< OCResource > g_configurationCollection; // for a group of multiple resources
std::shared_ptr< OCResource > g_configurationResource; // For a single resource

OCResourceHandle diagnosticsCollectionHandle;
std::shared_ptr< OCResource > g_diagnosticsCollection; // for a group of multiple resources
std::shared_ptr< OCResource > g_diagnosticsResource; // For a single resource

OCResourceHandle setCollectionHandle;
std::shared_ptr< OCResource > g_setCollection; // for a group of multiple resources
std::shared_ptr< OCResource > g_setResource; // For a single resource

std::map< std::string, std::shared_ptr< OCResource > > resourceTable;
std::vector< OCResourceHandle > resourceHandleVector;

typedef std::function<
        void(const HeaderOptions& headerOptions,
                const OCRepresentation& rep, const int eCode) > ConfigurationCallback;
typedef std::function<
        void(const HeaderOptions& headerOptions,
                const OCRepresentation& rep, const int eCode) > DiagnosticsCallback;

typedef std::string ConfigurationName;
typedef std::string ConfigurationValue;

void onReboot(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode)
{
    std::cout << "\tResource URI: " << rep.getUri() << std::endl;

    std::cout << "\t\tReboot:" << rep.getValue< std::string >("value") << std::endl;

    isWaiting = 0;
}

void onFactoryReset(const HeaderOptions& headerOptions, const OCRepresentation& rep,
        const int eCode)
{
    std::cout << "\tResource URI: " << rep.getUri() << std::endl;

    std::cout << "\t\tFactoryReset:" << rep.getValue< std::string >("value") << std::endl;
    isWaiting = 0;
}

void onUpdate(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode)
{
    std::cout << "\tResource URI: " << rep.getUri() << std::endl;

    std::cout << "\t\tvalue:" << rep.getValue< std::string >("value") << std::endl;

    isWaiting = 0;
}

void onGet(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode)
{
    std::cout << "\tResource URI: " << rep.getUri() << std::endl;

    if (rep.hasAttribute("value"))
        std::cout << "\t\tvalue:" << rep.getValue< std::string >("value") << std::endl;
    else if (rep.hasAttribute("link"))
        std::cout << "\t\tlink:" << rep.getValue< std::string >("link") << std::endl;

    std::vector< OCRepresentation > children = rep.getChildren();

    for (auto oit = children.begin(); oit != children.end(); ++oit)
    {
        std::cout << "\t\tChild Resource URI: " << oit->getUri() << std::endl;

        if (oit->hasAttribute("value"))
            std::cout << "\t\tvalue:" << oit->getValue< std::string >("value") << std::endl;
        else if (oit->hasAttribute("link"))
            std::cout << "\t\tlink:" << oit->getValue< std::string >("link") << std::endl;
    }

    isWaiting = 0;
}

// Callback to found collection resource
void onFoundCollectionResource(std::vector< std::shared_ptr< OCResource > > resources)
{

    std::string resourceURI;
    std::string hostAddress;
    try
    {
        // Do some operations with resource object.
        for (unsigned int i = 0; i < resources.size(); ++i)
        {
            std::shared_ptr< OCResource > resource = resources.at(i);

            if (resource)
            {
                if (resource->uri() == "/core/a/configuration/resourceset")
                    g_configurationCollection = resource;
                else if (resource->uri() == "/core/a/diagnostics/resourceset")
                    g_diagnosticsCollection = resource;
                else if (resource->uri() == "/core/a/factoryset/resourceset")
                    g_setCollection = resource;
                else
                {
                    isWaiting = 0;
                    return;
                }
            }
            else
            {
                // Resource is invalid
                std::cout << "Resource is invalid" << std::endl;
            }
        }

    }
    catch (std::exception& e)
    {
        //log(e.what());
    }

    if (g_configurationCollection != NULL && g_diagnosticsCollection != NULL
            && g_setCollection != NULL)
        isWaiting = 0;
}

// Callback to found resources
void onFoundCandidateCollection(std::vector< std::shared_ptr< OCResource > > resources)
{

    std::string resourceURI;
    std::string hostAddress;

    static bool flagForCon = false, flagForDiag = false, flagForSet = false;

    try
    {
        // Do some operations with resource object.
        for (unsigned int i = 0; i < resources.size(); ++i)
        {
            std::shared_ptr< OCResource > resource = resources.at(i);

            if (resource)
            {
                // Check if the resource is new one. If so, store it.

                std::map< std::string, std::shared_ptr< OCResource > >::iterator iter =
                        resourceTable.find(resource->host() + resource->uri());

                if (iter == resourceTable.end()) // new one
                {
                    resourceTable[resource->host() + resource->uri()] = resource;

                    OCResourceHandle foundResourceHandle;
                    OCStackResult result = OCPlatform::registerResource(foundResourceHandle,
                            resource);
                    std::cout << "\tResource ( " << resource->host() << " ) is registed!\t"
                            << std::endl;
                    if (result == OC_STACK_OK)
                    {
                        if (resource->uri() == "/oic/con")
                        {
                            flagForCon = true;
                            OCPlatform::bindResource(configurationCollectionHandle,
                                    foundResourceHandle);
                            if (g_configurationResource == NULL)
                                g_configurationResource = resource;
                        }
                        else if (resource->uri() == "/oic/diag")
                        {
                            flagForDiag = true;
                            OCPlatform::bindResource(diagnosticsCollectionHandle,
                                    foundResourceHandle);
                            if (g_diagnosticsResource == NULL)
                                g_diagnosticsResource = resource;
                        }
                        else if (resource->uri() == "/factorySet")
                        {
                            flagForSet = true;
                            OCPlatform::bindResource(setCollectionHandle, foundResourceHandle);
                            if (g_setResource == NULL)
                                g_setResource = resource;
                        }

                        resourceHandleVector.push_back(foundResourceHandle);
                    }
                    else
                    {
                        cout << "\tresource Error!" << endl;
                    }

                }

            }
            else
            {
                // Resource is invalid
                std::cout << "Resource is invalid" << std::endl;
            }
        }

    }
    catch (std::exception& e)
    {
        //log(e.what());
    }

    if (flagForCon && flagForDiag && flagForSet)
        isWaiting = 0;
}

int main(int argc, char* argv[])
{

    //**************************************************************
    // STEP 0
    PlatformConfig cfg
    { OC::ServiceType::InProc, OC::ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos };

    OCPlatform::Configure(cfg);
    g_thingsmanager = new ThingsManager();

    //**************************************************************

    while (true)
    {

        if (isWaiting > 0)
            continue;

        isWaiting = 0;

        cout << endl << endl << "(0) Quit" << std::endl;
        cout << "(1) Find all resources(URI: /oic/con, /oic/diag, /factoryset)" << std::endl;
        cout << "(2) Find all groups" << std::endl;
        cout << "(3) Get a new value (of \"Configuration\" Collection)" << std::endl;
        cout << "(4) Update a value (of \"Region\" Resource)" << std::endl;
        cout << "(5) Get a value (for \"Region\" Resource)" << std::endl;
        cout << "(6) FactoryReset (for the group)" << std::endl;
        cout << "(7) Reboot (for the group)" << std::endl;
        cout << "(10) Show Configuration Units" << std::endl;

        cin >> g_Steps;
        //
        if (g_Steps == 0)
            break;
        else if (g_Steps == 1)
        {
            std::vector< std::string > types;
            { // For Registering a collection resource for configuration resources

                string resourceURI = "/core/a/configuration/resourceset";
                string resourceTypeName = "core.configuration.resourceset";
                string resourceInterface = BATCH_INTERFACE;

                if (configurationCollectionHandle != NULL)
                {
                    std::cout << "already exists" << std::endl;
                    continue;
                }

                OCPlatform::registerResource(configurationCollectionHandle, resourceURI,
                        resourceTypeName, resourceInterface, NULL,
                        //&entityHandler, // entityHandler
                        OC_DISCOVERABLE);

                OCPlatform::bindInterfaceToResource(configurationCollectionHandle, GROUP_INTERFACE);
                OCPlatform::bindInterfaceToResource(configurationCollectionHandle,
                        DEFAULT_INTERFACE);

                // instead of registration
                types.push_back("oic.con");
                std::cout << "Finding Configuration Resource... " << std::endl;
            }

            { // For Registering a collection resource for diagnostics resources

                string resourceURI = "/core/a/diagnostics/resourceset";
                string resourceTypeName = "core.diagnostics.resourceset";
                string resourceInterface = BATCH_INTERFACE;

                if (diagnosticsCollectionHandle != NULL)
                {
                    std::cout << "already exists" << std::endl;
                    continue;
                }

                OCPlatform::registerResource(diagnosticsCollectionHandle, resourceURI,
                        resourceTypeName, resourceInterface, NULL,
                        //&entityHandler, // entityHandler
                        OC_DISCOVERABLE);

                OCPlatform::bindInterfaceToResource(diagnosticsCollectionHandle, GROUP_INTERFACE);
                OCPlatform::bindInterfaceToResource(diagnosticsCollectionHandle, DEFAULT_INTERFACE);

                // instead of registration
                types.push_back("oic.diag");
                std::cout << "Finding Diagnostics Resource... " << std::endl;

            }

            { // For Registering a collection resource for set resources

                string resourceURI = "/core/a/factoryset/resourceset";
                string resourceTypeName = "core.factoryset.resourceset";
                string resourceInterface = BATCH_INTERFACE;

                if (setCollectionHandle != NULL)
                {
                    std::cout << "already exists" << std::endl;
                    continue;
                }

                OCPlatform::registerResource(setCollectionHandle, resourceURI, resourceTypeName,
                        resourceInterface, NULL,
                        //&entityHandler, // entityHandler
                        OC_DISCOVERABLE);

                OCPlatform::bindInterfaceToResource(setCollectionHandle, GROUP_INTERFACE);
                OCPlatform::bindInterfaceToResource(setCollectionHandle, DEFAULT_INTERFACE);

                // instead of registration
                types.push_back("factorySet");
                std::cout << "Finding Set Resource... " << std::endl;
            }

            g_thingsmanager->findCandidateResources(types, &onFoundCandidateCollection, 5);

            isWaiting = 1;
        }
        else if (g_Steps == 2) // make a group with found things
        {
            std::vector< std::string > types;
            types.push_back("core.configuration.resourceset");
            types.push_back("core.diagnostics.resourceset");
            types.push_back("core.factoryset.resourceset");

            g_thingsmanager->findCandidateResources(types, &onFoundCollectionResource, 5);

            std::cout << "Finding Collection resource... " << std::endl;
            isWaiting = 1;

        }
        else if (g_Steps == 3)
        {
            // get a value

            ConfigurationName name = "configuration";

            std::cout << "For example, get configuration collection's value" << std::endl;

            std::vector< ConfigurationName > configurations;

            configurations.push_back(name);

            if (g_thingsmanager->getConfigurations(g_configurationResource, configurations, &onGet)
                    != OC_STACK_ERROR)
                isWaiting = 1;
        }
        else if (g_Steps == 4)
        {
            ConfigurationName name = "region";
            ConfigurationValue value = "U.S.A (new region)";

            std::cout << "For example, change region resource's value" << std::endl;
            std::cout << g_configurationCollection->uri() << std::endl;

            std::map< ConfigurationName, ConfigurationValue > configurations;

            configurations.insert(std::make_pair(name, value));

            if (g_thingsmanager->updateConfigurations(g_configurationCollection, configurations,
                    &onUpdate) != OC_STACK_ERROR)
                isWaiting = 1;
        }
        else if (g_Steps == 5)
        {
            // get a value

            ConfigurationName name = "region";

            std::cout << "For example, get region resource's value" << std::endl;

            std::vector< ConfigurationName > configurations;

            configurations.push_back(name);

            if (g_thingsmanager->getConfigurations(g_configurationCollection, configurations,
                    &onGet) != OC_STACK_ERROR)
                isWaiting = 1;
        }
        else if (g_Steps == 6)
        {
            // factory reset
            if (g_thingsmanager->factoryReset(g_diagnosticsCollection, &onFactoryReset)
                    != OC_STACK_ERROR)
                isWaiting = 1;
        }
        else if (g_Steps == 7)
        {
            // reboot
            if (g_thingsmanager->reboot(g_diagnosticsCollection, &onReboot) != OC_STACK_ERROR)
                isWaiting = 1;
        }
        else if (g_Steps == 10)
        {
            std::cout << g_thingsmanager->getListOfSupportedConfigurationUnits() << std::endl;

        }

    }

    return 0;
}

