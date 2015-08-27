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

#include "diagnosticsresource.h"

#include <functional>
#include <dlog.h>

#include "conserverapp.h"
#include "OCPlatform.h"
#include "OCApi.h"
#include "ThingsManager.h"

extern string logMessage;

extern void *updateLog(void *);

using namespace OC;

// Constructor
DiagnosticsResource::DiagnosticsResource() :
    m_factoryReset(defaultFactoryReset), m_reboot(defaultReboot),
    m_startStatCollection(defaultStartStatCollection)
{
    m_diagnosticsUri = "/oic/diag"; // URI of the resource
    m_diagnosticsTypes.push_back("oic.diag"); // resource type name
    m_diagnosticsInterfaces.push_back(DEFAULT_INTERFACE); // resource interface
    m_diagnosticsRep.setValue("fr", m_factoryReset);
    m_diagnosticsRep.setValue("rb", m_reboot);
    m_diagnosticsRep.setValue("ssc", m_startStatCollection);
    m_diagnosticsRep.setUri(m_diagnosticsUri);
    m_diagnosticsRep.setResourceTypes(m_diagnosticsTypes);
    m_diagnosticsRep.setResourceInterfaces(m_diagnosticsInterfaces);
    m_diagnosticsHandle = NULL;
}

// Creates a DiagnosticResource
void DiagnosticsResource::createResource(ResourceEntityHandler callback)
{
    using namespace OC::OCPlatform;

    if (NULL == callback)
    {
        dlog_print(DLOG_INFO, "DiagnosticsResource", "#### Callback should be binded");
        return;
    }

    // This will internally create and register the resource
    OCStackResult result = registerResource(m_diagnosticsHandle, m_diagnosticsUri,
                                            m_diagnosticsTypes[0], m_diagnosticsInterfaces[0],
                                            callback, OC_DISCOVERABLE | OC_OBSERVABLE);

    if (OC_STACK_OK != result)
    {
        dlog_print(DLOG_INFO, "DiagnosticsResource", "#### Resource creation"
                   "(configuration) was unsuccessful");
        return;
    }

    thread exec(
        std::function< void(int second) >(
            std::bind(&DiagnosticsResource::diagnosticsMonitor, this,
                      std::placeholders::_1)), 1);
    exec.detach();

    dlog_print(DLOG_INFO, "DiagnosticsResource", "#### Diagnostics Resource is Created");
}

void DiagnosticsResource::setDiagnosticsRepresentation(OCRepresentation &rep)
{
    string value;

    if (rep.getValue("fr", value))
    {
        m_factoryReset = value;
        dlog_print(DLOG_INFO, "DiagnosticsResource", "#### m_factoryReset: %s",
                   m_factoryReset.c_str());
    }

    if (rep.getValue("rb", value))
    {
        m_reboot = value;
        dlog_print(DLOG_INFO, "DiagnosticsResource", "#### m_reboot: %s", m_reboot.c_str());
    }

    if (rep.getValue("ssc", value))
    {
        m_startStatCollection = value;
        dlog_print(DLOG_INFO, "DiagnosticsResource", "#### m_startStatCollection: %s",
                   m_startStatCollection.c_str());
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

// Handles the Reboot and FactoryReset request
void DiagnosticsResource::diagnosticsMonitor(int second)
{
    while (1)
    {
        sleep(second);

        if (m_reboot == "true")
        {
            int res;
            dlog_print(DLOG_INFO, "DiagnosticsResource", "#### Reboot will be soon...");
            m_reboot = defaultReboot;
            res = system("sudo reboot"); // System reboot

            dlog_print(DLOG_INFO, "DiagnosticsResource", "#### return: %d", res);

            logMessage = "----------------------------<br>";
            logMessage += "*** System Reboot Success ***<br>";

            dlog_print(DLOG_INFO, LOG_TAG, "  %s", logMessage.c_str());
            //Show the log
            ecore_main_loop_thread_safe_call_sync(updateLog, &logMessage);

        }
        else if (m_factoryReset == "true")
        {
            dlog_print(DLOG_INFO, "DiagnosticsResource", "#### Factory Reset will be soon...");
            m_factoryReset = defaultFactoryReset;
            factoryReset();
        }
    }
}

// Deletes the diagnostic resource which has been created using createResource()
void DiagnosticsResource::deleteResource()
{
    // Unregister the resource
    if (NULL != m_diagnosticsHandle)
    {
        OCPlatform::unregisterResource(m_diagnosticsHandle);
    }
}
