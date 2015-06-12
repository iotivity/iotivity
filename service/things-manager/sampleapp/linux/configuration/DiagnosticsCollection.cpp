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
void DiagnosticsResource::createResources(ResourceEntityHandler callback)
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

    thread exec(
            std::function< void(int second) >(
                    std::bind(&DiagnosticsResource::diagnosticsMonitor, this,
                            std::placeholders::_1)), 10); // every 10 seconds
    exec.detach();

    std::cout << "Diagnostics Resource is Created!\n";
}

void DiagnosticsResource::setDiagnosticsRepresentation(OCRepresentation& rep)
{
    string value;

    if (rep.getValue("fr", value))
    {
        m_factoryReset = value;
        std::cout << "\t\t\t\t" << "m_factoryReset: " << m_factoryReset << std::endl;
    }

    if (rep.getValue("rb", value))
    {
        m_reboot = value;
        std::cout << "\t\t\t\t" << "m_reboot: " << m_reboot << std::endl;
    }

    if (rep.getValue("ssc", value))
    {
        m_startStatCollection = value;
        std::cout << "\t\t\t\t" << "m_startStatCollection: " << m_startStatCollection << std::endl;
    }
}

OCRepresentation DiagnosticsResource::getDiagnosticsRepresentation()
{
    m_diagnosticsRep.setValue("fr", m_factoryReset);
    m_diagnosticsRep.setValue("rb", m_reboot);
    m_diagnosticsRep.setValue("ssc", m_startStatCollection);

    return m_diagnosticsRep;
}

std::string DiagnosticsResource::getUri()
{
    return m_diagnosticsUri;
}

void DiagnosticsResource::diagnosticsMonitor(int second)
{
    while (1)
    {
        sleep(second);

        if (m_reboot == "true")
        {
            int res;
            std::cout << "Reboot will be soon..." << std::endl;
            m_reboot = defaultReboot;
            res = system("sudo reboot"); // System reboot

            std::cout << "return: " << res << std::endl;

        }
        else if (m_factoryReset == "true")
        {
            std::cout << "Factory Reset will be soon..." << std::endl;
            m_factoryReset = defaultFactoryReset;
            factoryReset();
        }
    }
}
