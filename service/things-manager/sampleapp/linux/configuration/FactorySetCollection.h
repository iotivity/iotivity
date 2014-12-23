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

///
/// This sample shows how one could create a resource (collection) with children.
///

#include <functional>
#include <thread>

#include "OCPlatform.h"
#include "OCApi.h"
#include "ThingsManager.h"
#include "ConfigurationCollection.h"

#pragma once

using namespace OC;

typedef std::function< OCEntityHandlerResult(std::shared_ptr< OCResourceRequest > request) > ResourceEntityHandler;

extern std::string defaultFactorySetValue;
static std::string defaultConfigurationCollectionLink = "/factorySet/oic/con";

static std::string defaultConfigurationURIPrefix = "/factorySet/oic/con";
static std::string defaultConfigurationResourceTypePrefix = "factorySet.oic.con";

class FactorySetCollection
{
public:

    ConfigurationCollection *defaultConfigurationCollection;

public:

    // diagnostics members
    std::string m_factorySetUri;
    std::string m_factorySetValue;
    std::vector< std::string > m_factorySetTypes;
    std::vector< std::string > m_factorySetInterfaces;
    OCResourceHandle m_factorySetHandle;
    OCRepresentation m_factorySetRep;

    // Configuration members
    std::string m_configurationCollectionUri;
    std::string m_configurationCollectionLink;
    std::vector< std::string > m_configurationCollectionTypes;
    std::vector< std::string > m_configurationCollectionInterfaces;
    OCResourceHandle m_configurationCollectionHandle;
    OCRepresentation m_configurationCollectionRep;

public:
    /// Constructor
    FactorySetCollection() :
            m_factorySetValue(defaultFactorySetValue), m_configurationCollectionLink(
                    defaultConfigurationCollectionLink)
    {
        m_configurationCollectionUri = "/factorySet/0/con"; // URI of the resource
        m_configurationCollectionTypes.push_back("factorySet.con"); // resource type name.
        m_configurationCollectionInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.

        m_configurationCollectionRep.setUri(m_configurationCollectionUri);
        m_configurationCollectionRep.setResourceTypes(m_configurationCollectionTypes);
        m_configurationCollectionRep.setResourceInterfaces(m_configurationCollectionInterfaces);
        m_configurationCollectionRep.setValue("link", m_configurationCollectionLink);
        m_configurationCollectionHandle = NULL;

        m_factorySetUri = "/factorySet"; // URI of the resource
        m_factorySetTypes.push_back("factorySet"); // resource type name.
        m_factorySetInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.
        m_factorySetInterfaces.push_back(BATCH_INTERFACE); // resource interface.
        m_factorySetInterfaces.push_back(LINK_INTERFACE); // resource interface.
        m_factorySetRep.setValue("value", m_factorySetValue);
        m_factorySetRep.setUri(m_factorySetUri);
        m_factorySetRep.setResourceTypes(m_factorySetTypes);
        m_factorySetRep.setResourceInterfaces(m_factorySetInterfaces);
        m_factorySetHandle = NULL;

        defaultConfigurationCollection = NULL;
    }
    ;

    ~FactorySetCollection()
    {
        if (defaultConfigurationCollection != NULL)
            free(defaultConfigurationCollection);
    }
    ;

    /// This function internally calls registerResource API.
    void createResources(ResourceEntityHandler callback);

    void setFactorySetRepresentation(OCRepresentation& rep);
    void setConfigurationCollectionRepresentation(OCRepresentation& rep);

    OCRepresentation getFactorySetRepresentation();
    OCRepresentation getConfigurationCollectionRepresentation();

    std::string getFactorySetUri();
    std::string getConfigurationCollectionUri();

};

