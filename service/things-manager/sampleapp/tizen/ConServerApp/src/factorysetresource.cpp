/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include "factorysetresource.h"

#include <functional>
#include <dlog.h>

#include "OCPlatform.h"
#include "OCApi.h"
#include "ThingsManager.h"

using namespace OC;

FactorySetResource::FactorySetResource()
{
    m_configurationUri = "/factorySet"; // URI of the resource
    m_configurationTypes.clear();
    m_configurationTypes.push_back("factorySet"); // resource type name.
    m_configurationRep.setUri(m_configurationUri);
    m_configurationRep.setResourceTypes(m_configurationTypes);
}

FactorySetResource::~FactorySetResource() {}

// This function internally calls registerResource API.
void FactorySetResource::createResource(ResourceEntityHandler callback)
{
    using namespace OC::OCPlatform;

    if (NULL == callback)
    {
        dlog_print(DLOG_INFO, "FactorySetResource", "#### Callback should be binded");
        return;
    }

    // This will internally create and register the resource
    OCStackResult result = registerResource(m_configurationHandle, m_configurationUri,
                                            m_configurationTypes[0], m_configurationInterfaces[0],
                                            callback, OC_DISCOVERABLE | OC_OBSERVABLE);

    if (OC_STACK_OK != result)
    {
        dlog_print(DLOG_INFO, "FactorySetResource", "#### Resource creation"
                   "(configuration) was unsuccessful");
        return;
    }

    dlog_print(DLOG_INFO, "FactorySetResource", "#### Configuration Resource is Created");
}

void FactorySetResource::setFactorySetRepresentation(OCRepresentation &rep)
{
    string value;

    if (rep.getValue("loc", value))
    {
        m_location = value;
        dlog_print(DLOG_INFO, "FactorySetResource", "#### m_location: %s",
                   m_location.c_str());
    }

    if (rep.getValue("st", value))
    {
        dlog_print(DLOG_INFO, "FactorySetResource", "#### SystemTime is not"
                   "allowed to be written");
    }

    if (rep.getValue("c", value))
    {
        m_currency = value;
        dlog_print(DLOG_INFO, "FactorySetResource", "#### m_currency: %s",
                   m_currency.c_str());
    }

    if (rep.getValue("r", value))
    {
        m_region = value;
        dlog_print(DLOG_INFO, "FactorySetResource", "#### m_region: %s",
                   m_region.c_str());
    }
}

OCRepresentation FactorySetResource::getFactorySetRepresentation()
{
    m_configurationRep.setValue("loc", m_location);
    m_configurationRep.setValue("st", m_systemTime);
    m_configurationRep.setValue("c", m_currency);
    m_configurationRep.setValue("r", m_region);

    return m_configurationRep;
}

std::string FactorySetResource::getUri()
{
    return m_configurationUri;
}

// Deletes the factoryset resource which has been created using createResource()
void FactorySetResource::deleteResource()
{
    // Unregister the Configuration resource
    if (NULL != m_configurationHandle)
    {
        OCPlatform::unregisterResource(m_configurationHandle);
    }
}
