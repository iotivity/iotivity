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
#include "DiagnosticsCollection.h"

using namespace OC;

/// This function internally calls registerResource API.
void DiagnosticsCollection::createResources(ResourceEntityHandler callback)
{
    using namespace OC::OCPlatform;

    if (callback == NULL)
    {
        std::cout << "callback should be binded\t";
        return;
    }

    // This will internally create and register the resource.
    OCStackResult result = registerResource(m_diagnosticsHandle, m_diagnosticsUri,
            m_diagnosticsTypes[0], m_diagnosticsInterfaces[0], callback,
            OC_DISCOVERABLE | OC_OBSERVABLE);

    if (OC_STACK_OK != result)
    {
        std::cout << "Resource creation (diagnostics) was unsuccessful\n";
    }

    result = bindInterfaceToResource(m_diagnosticsHandle, m_diagnosticsInterfaces[1]);
    if (OC_STACK_OK != result)
    {
        std::cout << "Binding TypeName to Resource was unsuccessful\n";
    }

    result = bindInterfaceToResource(m_diagnosticsHandle, m_diagnosticsInterfaces[2]);
    if (OC_STACK_OK != result)
    {
        std::cout << "Binding TypeName to Resource was unsuccessful\n";
    }

    result = registerResource(m_factoryResetHandle, m_factoryResetUri, m_factoryResetTypes[0],
            m_factoryResetInterfaces[0], callback, OC_DISCOVERABLE | OC_OBSERVABLE);

    if (OC_STACK_OK != result)
    {
        std::cout << "Resource creation (factoryReset) was unsuccessful\n";
    }

    result = registerResource(m_rebootHandle, m_rebootUri, m_rebootTypes[0], m_rebootInterfaces[0],
            callback, OC_DISCOVERABLE | OC_OBSERVABLE);

    if (OC_STACK_OK != result)
    {
        std::cout << "Resource creation (reboot) was unsuccessful\n";
    }

    result = registerResource(m_startCollectionHandle, m_startCollectionUri,
            m_startCollectionTypes[0], m_startCollectionInterfaces[0], callback,
            OC_DISCOVERABLE | OC_OBSERVABLE);

    if (OC_STACK_OK != result)
    {
        std::cout << "Resource creation (startCollection) was unsuccessful\n";
    }

    result = bindResource(m_diagnosticsHandle, m_factoryResetHandle);
    if (OC_STACK_OK != result)
    {
        std::cout << "Binding installedLocation resource to room was unsuccessful\n";
    }

    result = bindResource(m_diagnosticsHandle, m_rebootHandle);
    if (OC_STACK_OK != result)
    {
        std::cout << "Binding time resource to room was unsuccessful\n";
    }

    result = bindResource(m_diagnosticsHandle, m_startCollectionHandle);
    if (OC_STACK_OK != result)
    {
        std::cout << "Binding network resource to room was unsuccessful\n";
    }

    thread exec(
            std::function< void(int second) >(
                    std::bind(&DiagnosticsCollection::diagnosticsMonitor, this,
                            std::placeholders::_1)), 10); // every 10 seconds
    exec.detach();

    std::cout << "Diagnostics Collection is Created!\n";
}

void DiagnosticsCollection::setDiagnosticsRepresentation(OCRepresentation& rep)
{
    string value;

    if (rep.getValue("value", value))
    {
        m_diagnosticsValue = value;

        std::cout << "\t\t\t\t" << "m_diagnosticsValue: " << m_diagnosticsValue << std::endl;
    }
}

void DiagnosticsCollection::setFactoryResetRepresentation(OCRepresentation& rep)
{
    string value;

    if (rep.getValue("value", value))
    {
        m_factoryResetValue = value;

        std::cout << "\t\t\t\t" << "value: " << m_factoryResetValue << std::endl;
    }
}

void DiagnosticsCollection::setRebootRepresentation(OCRepresentation& rep)
{
    string value;

    if (rep.getValue("value", value))
    {
        m_rebootValue = value;

        std::cout << "\t\t\t\t" << "value: " << m_rebootValue << std::endl;
    }
}

void DiagnosticsCollection::setStartCollectionRepresentation(OCRepresentation& rep)
{
    string value;

    if (rep.getValue("value", value))
    {
        m_startCollectionValue = value;

        std::cout << "\t\t\t\t" << "value: " << m_startCollectionValue << std::endl;
    }
}

OCRepresentation DiagnosticsCollection::getFactoryResetRepresentation()
{
    m_factoryResetRep.setValue("value", m_factoryResetValue);

    return m_factoryResetRep;
}

OCRepresentation DiagnosticsCollection::getRebootRepresentation()
{
    m_rebootRep.setValue("value", m_rebootValue);

    return m_rebootRep;
}

OCRepresentation DiagnosticsCollection::getStartCollectionRepresentation()
{
    m_startCollectionRep.setValue("value", m_startCollectionValue);

    return m_startCollectionRep;
}

OCRepresentation DiagnosticsCollection::getDiagnosticsRepresentation()
{
    m_diagnosticsRep.clearChildren();

    m_diagnosticsRep.addChild(getFactoryResetRepresentation());
    m_diagnosticsRep.addChild(getRebootRepresentation());
    m_diagnosticsRep.addChild(getStartCollectionRepresentation());

    m_diagnosticsRep.setValue("value", m_diagnosticsValue);

    return m_diagnosticsRep;
}

std::string DiagnosticsCollection::getDiagnosticsUri()
{
    return m_diagnosticsUri;
}

std::string DiagnosticsCollection::getFactoryResetUri()
{
    return m_factoryResetUri;
}

std::string DiagnosticsCollection::getRebootUri()
{
    return m_rebootUri;
}

std::string DiagnosticsCollection::getStartCollectionUri()
{
    return m_startCollectionUri;
}

void DiagnosticsCollection::diagnosticsMonitor(int second)
{
    while (1)
    {
        sleep(second);

        if (m_rebootValue == "true")
        {
            int res;
            std::cout << "Reboot will be soon..." << std::endl;
            m_rebootValue = defaultReboot;
            res = system("sudo reboot"); // System reboot

            std::cout << "return: " << res << std::endl;

        }
        else if (m_factoryResetValue == "true")
        {
            std::cout << "Factory Reset will be soon..." << std::endl;
            m_factoryResetValue = defaultFactoryReset;
            factoryReset();
        }
    }
}
