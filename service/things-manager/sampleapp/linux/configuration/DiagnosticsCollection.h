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

#pragma once

using namespace OC;

typedef std::function<
    OCEntityHandlerResult(std::shared_ptr< OCResourceRequest > request) > ResourceEntityHandler;

static std::string defaultFactoryReset = "false";
static std::string defaultReboot = "false";
static std::string defaultStartStatCollection = "false";

class DiagnosticsResource
{
public:

    // diagnostics members
    std::string m_diagnosticsUri;
    std::string m_factoryReset;
    std::string m_reboot;
    std::string m_startStatCollection;
    std::vector< std::string > m_diagnosticsTypes;
    std::vector< std::string > m_diagnosticsInterfaces;
    OCResourceHandle m_diagnosticsHandle;
    OCRepresentation m_diagnosticsRep;

public:
    /// Constructor
    DiagnosticsResource() :
           m_factoryReset(defaultFactoryReset), m_reboot(defaultReboot),
            m_startStatCollection(defaultStartStatCollection)
    {
        m_diagnosticsUri = "/oic/diag"; // URI of the resource
        m_diagnosticsTypes.push_back("oic.diag"); // resource type name.
        m_diagnosticsInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.
        m_diagnosticsRep.setValue("fr", m_factoryReset);
        m_diagnosticsRep.setValue("rb", m_reboot);
        m_diagnosticsRep.setValue("ssc", m_startStatCollection);
        m_diagnosticsRep.setUri(m_diagnosticsUri);
        m_diagnosticsRep.setResourceTypes(m_diagnosticsTypes);
        m_diagnosticsRep.setResourceInterfaces(m_diagnosticsInterfaces);
        m_diagnosticsHandle = NULL;
    }
    ;

    /// This function internally calls registerResource API.
    void createResources(ResourceEntityHandler callback);

    void setDiagnosticsRepresentation(OCRepresentation& rep);

    OCRepresentation getDiagnosticsRepresentation();

    std::string getUri();

    void diagnosticsMonitor(int second);

    std::function< void() > factoryReset;
};

