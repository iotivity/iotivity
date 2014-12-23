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

using namespace OC;

/// This function internally calls registerResource API.
void ConfigurationCollection::createResources(ResourceEntityHandler callback)
{
    using namespace OC::OCPlatform;

    if (callback == NULL)
    {
        std::cout << "callback should be binded\t";
        return;
    }

    // This will internally create and register the resource.
    OCStackResult result = registerResource(m_configurationHandle, m_configurationUri,
            m_configurationTypes[0], m_configurationInterfaces[0], callback,
            OC_DISCOVERABLE | OC_OBSERVABLE);

    if (OC_STACK_OK != result)
    {
        std::cout << "Resource creation (configuration) was unsuccessful\n";
    }

    result = bindInterfaceToResource(m_configurationHandle, m_configurationInterfaces[1]);
    if (OC_STACK_OK != result)
    {
        std::cout << "Binding TypeName to Resource was unsuccessful\n";
    }

    result = bindInterfaceToResource(m_configurationHandle, m_configurationInterfaces[2]);
    if (OC_STACK_OK != result)
    {
        std::cout << "Binding TypeName to Resource was unsuccessful\n";
    }

    result = registerResource(m_regionHandle, m_regionUri, m_regionTypes[0], m_regionInterfaces[0],
            callback, OC_DISCOVERABLE | OC_OBSERVABLE);

    if (OC_STACK_OK != result)
    {
        std::cout << "Resource creation (region) was unsuccessful\n";
    }

    result = registerResource(m_timeHandle, m_timeUri, m_timeTypes[0], m_timeInterfaces[0],
            callback, OC_DISCOVERABLE | OC_OBSERVABLE);

    if (OC_STACK_OK != result)
    {
        std::cout << "Resource creation (time) was unsuccessful\n";
    }

    result = registerResource(m_networkHandle, m_networkUri, m_networkTypes[0],
            m_networkInterfaces[0], callback, OC_DISCOVERABLE | OC_OBSERVABLE);

    if (OC_STACK_OK != result)
    {
        std::cout << "Resource creation (network) was unsuccessful\n";
    }

    result = registerResource(m_securityHandle, m_securityUri, m_securityTypes[0],
            m_securityInterfaces[0], callback, OC_DISCOVERABLE | OC_OBSERVABLE);

    if (OC_STACK_OK != result)
    {
        std::cout << "Resource creation (security) was unsuccessful\n";
    }

    result = bindResource(m_configurationHandle, m_regionHandle);
    if (OC_STACK_OK != result)
    {
        std::cout << "Binding region resource to room was unsuccessful\n";
    }

    result = bindResource(m_configurationHandle, m_timeHandle);
    if (OC_STACK_OK != result)
    {
        std::cout << "Binding time resource to room was unsuccessful\n";
    }

    result = bindResource(m_configurationHandle, m_networkHandle);
    if (OC_STACK_OK != result)
    {
        std::cout << "Binding network resource to room was unsuccessful\n";
    }

    result = bindResource(m_configurationHandle, m_securityHandle);
    if (OC_STACK_OK != result)
    {
        std::cout << "Binding security resource to room was unsuccessful\n";
    }

    std::cout << "Configuration Collection is Created!(URI: " << m_configurationUri << ") \n";

    myTimeCollection->createResources(callback);
    myNetworkCollection->createResources(callback);
    mySecurityCollection->createResources(callback);

}

void ConfigurationCollection::setConfigurationRepresentation(OCRepresentation& rep)
{
    string value;

    if (rep.getValue("value", value))
    {
        m_configurationValue = value;

        std::cout << "\t\t\t\t" << "m_configurationValue: " << m_configurationValue << std::endl;
    }
}

void ConfigurationCollection::setTimeRepresentation(OCRepresentation& rep)
{
    string value;

    if (rep.getValue("link", value))
    {
        // NOT ALLOWED

        std::cout << "\t\t\t\t" << "link: " << m_timeLink << std::endl;
    }
}

void ConfigurationCollection::setNetworkRepresentation(OCRepresentation& rep)
{
    string value;

    if (rep.getValue("link", value))
    {
        // NOT ALLOWED

        std::cout << "\t\t\t\t" << "link: " << m_networkLink << std::endl;
    }
}

void ConfigurationCollection::setSecurityRepresentation(OCRepresentation& rep)
{
    string value;

    if (rep.getValue("link", value))
    {
        // NOT ALLOWED

        std::cout << "\t\t\t\t" << "link: " << m_securityLink << std::endl;
    }
}

void ConfigurationCollection::setRegionRepresentation(OCRepresentation& rep)
{
    string value;

    if (rep.getValue("value", value))
    {
        m_regionValue = value;

        std::cout << "\t\t\t\t" << "value: " << m_regionValue << std::endl;
    }
}

OCRepresentation ConfigurationCollection::getTimeRepresentation()
{
    m_timeRep.setValue("link", m_timeLink);

    return m_timeRep;
}

OCRepresentation ConfigurationCollection::getNetworkRepresentation()
{
    m_networkRep.setValue("link", m_networkLink);

    return m_networkRep;
}

OCRepresentation ConfigurationCollection::getSecurityRepresentation()
{
    m_securityRep.setValue("link", m_securityLink);

    return m_securityRep;
}

OCRepresentation ConfigurationCollection::getRegionRepresentation()
{
    m_regionRep.setValue("value", m_regionValue);

    return m_regionRep;
}

OCRepresentation ConfigurationCollection::getConfigurationRepresentation()
{
    m_configurationRep.clearChildren();

    m_configurationRep.addChild(getRegionRepresentation());
    m_configurationRep.addChild(getTimeRepresentation());
    m_configurationRep.addChild(getNetworkRepresentation());
    m_configurationRep.addChild(getSecurityRepresentation());

    m_configurationRep.setValue("value", m_configurationValue);

    return m_configurationRep;
}

std::string ConfigurationCollection::getConfigurationUri()
{
    return m_configurationUri;
}

std::string ConfigurationCollection::getTimeUri()
{
    return m_timeUri;
}

std::string ConfigurationCollection::getNetworkUri()
{
    return m_networkUri;
}

std::string ConfigurationCollection::getSecurityUri()
{
    return m_securityUri;
}

std::string ConfigurationCollection::getRegionUri()
{
    return m_regionUri;
}

void ConfigurationCollection::factoryReset()
{
    m_configurationValue = defaultConfigurationValue;
    m_regionValue = defaultRegionValue;
    m_timeLink = defaultTimeLink;
    m_networkLink = defaultNetworkLink;
    m_securityLink = defaultSecurityLink;

    myTimeCollection->factoryReset();
    myNetworkCollection->factoryReset();
    mySecurityCollection->factoryReset();
}

/// This function internally calls registerResource API.
void TimeCollection::createResources(ResourceEntityHandler callback)
{
    using namespace OC::OCPlatform;

    if (callback == NULL)
    {
        std::cout << "callback should be binded\t";
        return;
    }

    // This will internally create and register the resource.
    OCStackResult result = registerResource(m_timeHandle, m_timeUri, m_timeTypes[0],
            m_timeInterfaces[0], callback, OC_DISCOVERABLE | OC_OBSERVABLE);

    if (OC_STACK_OK != result)
    {
        std::cout << "Resource creation (time) was unsuccessful\n";
    }

    result = bindInterfaceToResource(m_timeHandle, m_timeInterfaces[1]);
    if (OC_STACK_OK != result)
    {
        std::cout << "Binding TypeName to Resource was unsuccessful\n";
    }

    result = bindInterfaceToResource(m_timeHandle, m_timeInterfaces[2]);
    if (OC_STACK_OK != result)
    {
        std::cout << "Binding TypeName to Resource was unsuccessful\n";
    }

    result = registerResource(m_currentTimeHandle, m_currentTimeUri, m_currentTimeTypes[0],
            m_currentTimeInterfaces[0], callback, OC_DISCOVERABLE | OC_OBSERVABLE);

    if (OC_STACK_OK != result)
    {
        std::cout << "Resource creation (currentTime) was unsuccessful\n";
    }

    result = bindResource(m_timeHandle, m_currentTimeHandle);
    if (OC_STACK_OK != result)
    {
        std::cout << "Binding currentTime resource to room was unsuccessful\n";
    }

    std::cout << "Time Collection is Created!(URI: " << m_timeUri << ") \n";
}

void TimeCollection::setTimeRepresentation(OCRepresentation& rep)
{
    string value;

    if (rep.getValue("value", value))
    {
        m_timeValue = value;

        std::cout << "\t\t\t\t" << "m_timeValue: " << m_timeValue << std::endl;
    }
}

void TimeCollection::setCurrentTimeRepresentation(OCRepresentation& rep)
{
    string value;

    if (rep.getValue("currentTime", value))
    {
        m_currentTimeValue = value;

        std::cout << "\t\t\t\t" << "value: " << m_currentTimeValue << std::endl;
    }
}

OCRepresentation TimeCollection::getCurrentTimeRepresentation()
{
    m_currentTimeRep.setValue("value", m_currentTimeValue);

    return m_currentTimeRep;
}

OCRepresentation TimeCollection::getTimeRepresentation()
{
    m_timeRep.clearChildren();

    m_timeRep.addChild(getCurrentTimeRepresentation());

    m_timeRep.setValue("value", m_timeValue);

    return m_timeRep;
}

std::string TimeCollection::getTimeUri()
{
    return m_timeUri;
}

std::string TimeCollection::getCurrentTimeUri()
{
    return m_currentTimeUri;
}

void TimeCollection::factoryReset()
{
    m_timeValue = defaultTimeValue;
    m_currentTimeValue = defaultCurrentTimeValue;
}

/// This function internally calls registerResource API.
void NetworkCollection::createResources(ResourceEntityHandler callback)
{
    using namespace OC::OCPlatform;

    if (callback == NULL)
    {
        std::cout << "callback should be binded\t";
        return;
    }

    // This will internally create and register the resource.
    OCStackResult result = registerResource(m_networkHandle, m_networkUri, m_networkTypes[0],
            m_networkInterfaces[0], callback, OC_DISCOVERABLE | OC_OBSERVABLE);

    if (OC_STACK_OK != result)
    {
        std::cout << "Resource creation (network) was unsuccessful\n";
    }

    result = bindInterfaceToResource(m_networkHandle, m_networkInterfaces[1]);
    if (OC_STACK_OK != result)
    {
        std::cout << "Binding TypeName to Resource was unsuccessful\n";
    }

    result = bindInterfaceToResource(m_networkHandle, m_networkInterfaces[2]);
    if (OC_STACK_OK != result)
    {
        std::cout << "Binding TypeName to Resource was unsuccessful\n";
    }

    result = registerResource(m_IPAddressHandle, m_IPAddressUri, m_IPAddressTypes[0],
            m_IPAddressInterfaces[0], callback, OC_DISCOVERABLE | OC_OBSERVABLE);

    if (OC_STACK_OK != result)
    {
        std::cout << "Resource creation (IPAddress) was unsuccessful\n";
    }

    result = bindResource(m_networkHandle, m_IPAddressHandle);
    if (OC_STACK_OK != result)
    {
        std::cout << "Binding IPAddress resource to room was unsuccessful\n";
    }

    std::cout << "Network Collection is Created!(URI: " << m_networkUri << ") \n";
}

void NetworkCollection::setNetworkRepresentation(OCRepresentation& rep)
{
    string value;

    if (rep.getValue("value", value))
    {
        m_networkValue = value;

        std::cout << "\t\t\t\t" << "m_networkValue: " << m_networkValue << std::endl;
    }
}

void NetworkCollection::setIPAddressRepresentation(OCRepresentation& rep)
{
    string value;

    if (rep.getValue("IPAddress", value))
    {
        m_IPAddressValue = value;

        std::cout << "\t\t\t\t" << "value: " << m_IPAddressValue << std::endl;
    }
}
OCRepresentation NetworkCollection::getIPAddressRepresentation()
{
    m_IPAddressRep.setValue("value", m_IPAddressValue);

    return m_IPAddressRep;
}

OCRepresentation NetworkCollection::getNetworkRepresentation()
{
    m_networkRep.clearChildren();

    m_networkRep.addChild(getIPAddressRepresentation());

    m_networkRep.setValue("value", m_networkValue);

    return m_networkRep;
}

std::string NetworkCollection::getNetworkUri()
{
    return m_networkUri;
}

std::string NetworkCollection::getIPAddressUri()
{
    return m_IPAddressUri;
}

void NetworkCollection::factoryReset()
{
    m_networkValue = defaultNetworkValue;
    m_IPAddressValue = defaultIPAddressValue;
}

/// This function internally calls registerResource API.
void SecurityCollection::createResources(ResourceEntityHandler callback)
{
    using namespace OC::OCPlatform;

    if (callback == NULL)
    {
        std::cout << "callback should be binded\t";
        return;
    }

    // This will internally create and register the resource.
    OCStackResult result = registerResource(m_securityHandle, m_securityUri, m_securityTypes[0],
            m_securityInterfaces[0], callback, OC_DISCOVERABLE | OC_OBSERVABLE);

    if (OC_STACK_OK != result)
    {
        std::cout << "Resource creation (security) was unsuccessful\n";
    }

    result = bindInterfaceToResource(m_securityHandle, m_securityInterfaces[1]);
    if (OC_STACK_OK != result)
    {
        std::cout << "Binding TypeName to Resource was unsuccessful\n";
    }

    result = bindInterfaceToResource(m_securityHandle, m_securityInterfaces[2]);
    if (OC_STACK_OK != result)
    {
        std::cout << "Binding TypeName to Resource was unsuccessful\n";
    }

    result = registerResource(m_modeHandle, m_modeUri, m_modeTypes[0], m_modeInterfaces[0],
            callback, OC_DISCOVERABLE | OC_OBSERVABLE);

    if (OC_STACK_OK != result)
    {
        std::cout << "Resource creation (mode) was unsuccessful\n";
    }

    result = bindResource(m_securityHandle, m_modeHandle);
    if (OC_STACK_OK != result)
    {
        std::cout << "Binding mode resource to room was unsuccessful\n";
    }

    std::cout << "Security Collection is Created!(URI: " << m_securityUri << ") \n";
}

void SecurityCollection::setSecurityRepresentation(OCRepresentation& rep)
{
    string value;

    if (rep.getValue("value", value))
    {
        m_securityValue = value;

        std::cout << "\t\t\t\t" << "m_securityValue: " << m_securityValue << std::endl;
    }
}

void SecurityCollection::setModeRepresentation(OCRepresentation& rep)
{
    string value;

    if (rep.getValue("mode", value))
    {
        m_modeValue = value;

        std::cout << "\t\t\t\t" << "value: " << m_modeValue << std::endl;
    }
}

OCRepresentation SecurityCollection::getModeRepresentation()
{
    m_modeRep.setValue("value", m_modeValue);

    return m_modeRep;
}

OCRepresentation SecurityCollection::getSecurityRepresentation()
{
    m_securityRep.clearChildren();

    m_securityRep.addChild(getModeRepresentation());

    m_securityRep.setValue("value", m_securityValue);

    return m_securityRep;
}

std::string SecurityCollection::getSecurityUri()
{
    return m_securityUri;
}

std::string SecurityCollection::getModeUri()
{
    return m_modeUri;
}

void SecurityCollection::factoryReset()
{
    m_securityValue = defaultSecurityValue;
    m_modeValue = defaultModeValue;
}

