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

typedef std::function< OCEntityHandlerResult(std::shared_ptr< OCResourceRequest > request) > ResourceEntityHandler;

static std::string defaultDiagnosticsValue = "false";
static std::string defaultFactoryReset = "false";
static std::string defaultReboot = "false";
static std::string defaultStartCollection = "false";

class DiagnosticsCollection
{
public:

    // diagnostics members
    std::string m_diagnosticsUri;
    std::string m_diagnosticsValue;
    std::vector< std::string > m_diagnosticsTypes;
    std::vector< std::string > m_diagnosticsInterfaces;
    OCResourceHandle m_diagnosticsHandle;
    OCRepresentation m_diagnosticsRep;

    // factory reset members
    std::string m_factoryResetUri;
    std::string m_factoryResetValue;
    std::vector< std::string > m_factoryResetTypes;
    std::vector< std::string > m_factoryResetInterfaces;
    OCResourceHandle m_factoryResetHandle;
    OCRepresentation m_factoryResetRep;

    // reboot members
    std::string m_rebootUri;
    std::string m_rebootValue;
    std::vector< std::string > m_rebootTypes;
    std::vector< std::string > m_rebootInterfaces;
    OCResourceHandle m_rebootHandle;
    OCRepresentation m_rebootRep;

    // startcollection members
    std::string m_startCollectionUri;
    std::string m_startCollectionValue;
    std::vector< std::string > m_startCollectionTypes;
    std::vector< std::string > m_startCollectionInterfaces;
    OCResourceHandle m_startCollectionHandle;
    OCRepresentation m_startCollectionRep;

public:
    /// Constructor
    DiagnosticsCollection() :
            m_diagnosticsValue(defaultDiagnosticsValue), m_factoryResetValue(defaultFactoryReset), m_rebootValue(
                    defaultReboot), m_startCollectionValue(defaultStartCollection)
    {
        m_factoryResetUri = "/oic/diag/0/factoryReset"; // URI of the resource
        m_factoryResetTypes.push_back("oic.diag.factoryReset"); // resource type name.
        m_factoryResetInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.

        m_factoryResetRep.setUri(m_factoryResetUri);
        m_factoryResetRep.setResourceTypes(m_factoryResetTypes);
        m_factoryResetRep.setResourceInterfaces(m_factoryResetInterfaces);
        m_factoryResetRep.setValue("value", m_factoryResetValue);
        m_factoryResetHandle = NULL;

        m_rebootUri = "/oic/diag/0/reboot"; // URI of the resource
        m_rebootTypes.push_back("oic.diag.reboot"); // resource type name.
        m_rebootInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.

        m_rebootRep.setUri(m_rebootUri);
        m_rebootRep.setResourceTypes(m_rebootTypes);
        m_rebootRep.setResourceInterfaces(m_rebootInterfaces);
        m_rebootRep.setValue("value", m_rebootValue);
        m_rebootHandle = NULL;

        m_startCollectionUri = "/oic/diag/0/startCollection"; // URI of the resource
        m_startCollectionTypes.push_back("oic.diag.startCollection"); // resource type name.
        m_startCollectionInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.

        m_startCollectionRep.setUri(m_startCollectionUri);
        m_startCollectionRep.setResourceTypes(m_startCollectionTypes);
        m_startCollectionRep.setResourceInterfaces(m_startCollectionInterfaces);
        m_startCollectionRep.setValue("value", m_startCollectionValue);
        m_startCollectionHandle = NULL;

        m_diagnosticsUri = "/oic/diag"; // URI of the resource
        m_diagnosticsTypes.push_back("oic.diag"); // resource type name.
        m_diagnosticsInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.
        m_diagnosticsInterfaces.push_back(BATCH_INTERFACE); // resource interface.
        m_diagnosticsInterfaces.push_back(LINK_INTERFACE); // resource interface.
        m_diagnosticsRep.setValue("value", m_diagnosticsValue);
        m_diagnosticsRep.setUri(m_diagnosticsUri);
        m_diagnosticsRep.setResourceTypes(m_diagnosticsTypes);
        m_diagnosticsRep.setResourceInterfaces(m_diagnosticsInterfaces);
        m_diagnosticsHandle = NULL;
    }
    ;

    /// This function internally calls registerResource API.
    void createResources(ResourceEntityHandler callback);

    void setDiagnosticsRepresentation(OCRepresentation& rep);
    void setFactoryResetRepresentation(OCRepresentation& rep);
    void setRebootRepresentation(OCRepresentation& rep);
    void setStartCollectionRepresentation(OCRepresentation& rep);

    OCRepresentation getDiagnosticsRepresentation();
    OCRepresentation getFactoryResetRepresentation();
    OCRepresentation getRebootRepresentation();
    OCRepresentation getStartCollectionRepresentation();

    std::string getDiagnosticsUri();
    std::string getFactoryResetUri();
    std::string getRebootUri();
    std::string getStartCollectionUri();

    void diagnosticsMonitor(int second);

    std::function< void() > factoryReset;
};

