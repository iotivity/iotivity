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
#include "FactorySetCollection.h"

using namespace OC;

/// This function internally calls registerResource API.
void FactorySetCollection::createResources(ResourceEntityHandler callback)
{
    using namespace OC::OCPlatform;

    if (callback == NULL)
    {
        std::cout << "callback should be binded\t";
        return;
    }

    // This will internally create and register the resource.
    OCStackResult result = registerResource(m_factorySetHandle, m_factorySetUri,
            m_factorySetTypes[0], m_factorySetInterfaces[0], callback,
            OC_DISCOVERABLE | OC_OBSERVABLE);

    if (OC_STACK_OK != result)
    {
        std::cout << "Resource creation (configuration) was unsuccessful\n";
    }

    result = bindInterfaceToResource(m_factorySetHandle, m_factorySetInterfaces[1]);
    if (OC_STACK_OK != result)
    {
        std::cout << "Binding TypeName to Resource was unsuccessful\n";
    }

    result = bindInterfaceToResource(m_factorySetHandle, m_factorySetInterfaces[2]);
    if (OC_STACK_OK != result)
    {
        std::cout << "Binding TypeName to Resource was unsuccessful\n";
    }

    result = registerResource(m_configurationCollectionHandle, m_configurationCollectionUri,
            m_configurationCollectionTypes[0], m_configurationCollectionInterfaces[0], callback,
            OC_DISCOVERABLE | OC_OBSERVABLE);

    if (OC_STACK_OK != result)
    {
        std::cout << "Resource creation (installedLocation) was unsuccessful\n";
    }

    result = bindResource(m_factorySetHandle, m_configurationCollectionHandle);
    if (OC_STACK_OK != result)
    {
        std::cout << "Binding installedLocation resource to room was unsuccessful\n";
    }

    defaultConfigurationCollection = new ConfigurationCollection(defaultConfigurationURIPrefix,
            defaultConfigurationResourceTypePrefix);
    //defaultConfigurationCollection->bindEntityHander(callback);
    defaultConfigurationCollection->createResources(callback);

    std::cout << "FactorySet Collection is Created!\n";
}

void FactorySetCollection::setFactorySetRepresentation(OCRepresentation& rep)
{
    string value;

    if (rep.getValue("value", value))
    {
        m_factorySetValue = value;

        std::cout << "\t\t\t\t" << "m_factorySetValue: " << m_factorySetValue << std::endl;
    }
}

void FactorySetCollection::setConfigurationCollectionRepresentation(OCRepresentation& rep)
{
    string value;

    if (rep.getValue("link", value))
    {
        // NOT ALLOWED

        std::cout << "\t\t\t\t" << "link: " << m_configurationCollectionLink << std::endl;
    }
}

OCRepresentation FactorySetCollection::getConfigurationCollectionRepresentation()
{
    m_configurationCollectionRep.setValue("link", m_configurationCollectionLink);

    return m_configurationCollectionRep;
}

OCRepresentation FactorySetCollection::getFactorySetRepresentation()
{
    m_factorySetRep.clearChildren();

    m_factorySetRep.addChild(getConfigurationCollectionRepresentation());

    m_factorySetRep.setValue("value", m_factorySetValue);

    return m_factorySetRep;
}

std::string FactorySetCollection::getFactorySetUri()
{
    return m_factorySetUri;
}

std::string FactorySetCollection::getConfigurationCollectionUri()
{
    return m_configurationCollectionUri;
}
