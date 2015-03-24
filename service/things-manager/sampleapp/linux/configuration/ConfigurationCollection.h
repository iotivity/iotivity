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

static std::string defaultURIPrefix = "/oic/con";
static std::string defaultResourceTypePrefix = "oic.con";

extern std::string defaultTimeValue;
extern std::string defaultCurrentTimeValue;

class TimeCollection
{
public:

    // diagnostics members
    std::string m_timeUri;
    std::string m_timeValue;
    std::vector< std::string > m_timeTypes;
    std::vector< std::string > m_timeInterfaces;
    OCResourceHandle m_timeHandle;
    OCRepresentation m_timeRep;

    // factory reset members
    std::string m_currentTimeUri;
    std::string m_currentTimeValue;
    std::vector< std::string > m_currentTimeTypes;
    std::vector< std::string > m_currentTimeInterfaces;
    OCResourceHandle m_currentTimeHandle;
    OCRepresentation m_currentTimeRep;

public:
    /// Constructor
    TimeCollection() :
            m_timeValue(defaultTimeValue), m_currentTimeValue(defaultCurrentTimeValue)
    {
        m_currentTimeUri = defaultURIPrefix + "/time/0/currentTime"; // URI of the resource
        m_currentTimeTypes.push_back("oic.con.time.currentTime"); // resource type name.
        m_currentTimeInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.

        m_currentTimeRep.setUri(m_currentTimeUri);
        m_currentTimeRep.setResourceTypes(m_currentTimeTypes);
        m_currentTimeRep.setResourceInterfaces(m_currentTimeInterfaces);
        m_currentTimeRep.setValue("value", m_currentTimeValue);
        m_currentTimeHandle = NULL;

        m_timeUri = defaultURIPrefix + "/time"; // URI of the resource
        m_timeTypes.push_back("oic.con.time"); // resource type name.
        m_timeInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.
        m_timeInterfaces.push_back(BATCH_INTERFACE); // resource interface.
        m_timeInterfaces.push_back(LINK_INTERFACE); // resource interface.
        m_timeRep.setValue("value", m_timeValue);
        m_timeRep.setUri(m_timeUri);
        m_timeRep.setResourceTypes(m_timeTypes);
        m_timeRep.setResourceInterfaces(m_timeInterfaces);
        m_timeHandle = NULL;
    }
    ;

    /// Constructor
    TimeCollection(std::string URIPrefix, std::string ResourceTypePrefix) :
            m_timeValue(defaultTimeValue), m_currentTimeValue(defaultCurrentTimeValue)
    {
        m_currentTimeUri = URIPrefix + "/time/0/currentTime"; // URI of the resource
        m_currentTimeTypes.push_back(ResourceTypePrefix + ".time.currentTime"); // type name.
        m_currentTimeInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.

        m_currentTimeRep.setUri(m_currentTimeUri);
        m_currentTimeRep.setResourceTypes(m_currentTimeTypes);
        m_currentTimeRep.setResourceInterfaces(m_currentTimeInterfaces);
        m_currentTimeRep.setValue("value", m_currentTimeValue);
        m_currentTimeHandle = NULL;

        m_timeUri = URIPrefix + "/time"; // URI of the resource
        m_timeTypes.push_back(ResourceTypePrefix + ".time"); // resource type name.
        m_timeInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.
        m_timeInterfaces.push_back(BATCH_INTERFACE); // resource interface.
        m_timeInterfaces.push_back(LINK_INTERFACE); // resource interface.
        m_timeRep.setValue("value", m_timeValue);
        m_timeRep.setUri(m_timeUri);
        m_timeRep.setResourceTypes(m_timeTypes);
        m_timeRep.setResourceInterfaces(m_timeInterfaces);
        m_timeHandle = NULL;
    }
    ;

    /// This function internally calls registerResource API.
    void createResources(ResourceEntityHandler callback);

    void setTimeRepresentation(OCRepresentation& rep);
    void setCurrentTimeRepresentation(OCRepresentation& rep);

    OCRepresentation getTimeRepresentation();
    OCRepresentation getCurrentTimeRepresentation();

    std::string getTimeUri();
    std::string getCurrentTimeUri();

    void factoryReset();

};

extern std::string defaultNetworkValue;
extern std::string defaultIPAddressValue;

class NetworkCollection
{
public:

    // diagnostics members
    std::string m_networkUri;
    std::string m_networkValue;
    std::vector< std::string > m_networkTypes;
    std::vector< std::string > m_networkInterfaces;
    OCResourceHandle m_networkHandle;
    OCRepresentation m_networkRep;

    // factory reset members
    std::string m_IPAddressUri;
    std::string m_IPAddressValue;
    std::vector< std::string > m_IPAddressTypes;
    std::vector< std::string > m_IPAddressInterfaces;
    OCResourceHandle m_IPAddressHandle;
    OCRepresentation m_IPAddressRep;

public:

    /// Constructor
    NetworkCollection() :
            m_networkValue(defaultNetworkValue), m_IPAddressValue(defaultIPAddressValue)
    {
        m_IPAddressUri = defaultURIPrefix + "/network/0/IPAddress"; // URI of the resource
        m_IPAddressTypes.push_back("oic.con.network.IPAddress"); // resource type name.
        m_IPAddressInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.

        m_IPAddressRep.setUri(m_IPAddressUri);
        m_IPAddressRep.setResourceTypes(m_IPAddressTypes);
        m_IPAddressRep.setResourceInterfaces(m_IPAddressInterfaces);
        m_IPAddressRep.setValue("value", m_IPAddressValue);
        m_IPAddressHandle = NULL;

        m_networkUri = defaultURIPrefix + "/network"; // URI of the resource
        m_networkTypes.push_back("oic.con.network"); // resource type name.
        m_networkInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.
        m_networkInterfaces.push_back(BATCH_INTERFACE); // resource interface.
        m_networkInterfaces.push_back(LINK_INTERFACE); // resource interface.
        m_networkRep.setValue("value", m_networkValue);
        m_networkRep.setUri(m_networkUri);
        m_networkRep.setResourceTypes(m_networkTypes);
        m_networkRep.setResourceInterfaces(m_networkInterfaces);
        m_networkHandle = NULL;
    }
    ;

    /// Constructor
    NetworkCollection(std::string URIPrefix, std::string ResourceTypePrefix) :
            m_networkValue(defaultNetworkValue), m_IPAddressValue(defaultIPAddressValue)
    {
        m_IPAddressUri = URIPrefix + "/network/0/IPAddress"; // URI of the resource
        m_IPAddressTypes.push_back(ResourceTypePrefix + "network.IPAddress"); // resource type name.
        m_IPAddressInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.

        m_IPAddressRep.setUri(m_IPAddressUri);
        m_IPAddressRep.setResourceTypes(m_IPAddressTypes);
        m_IPAddressRep.setResourceInterfaces(m_IPAddressInterfaces);
        m_IPAddressRep.setValue("value", m_IPAddressValue);
        m_IPAddressHandle = NULL;

        m_networkUri = URIPrefix + "/network"; // URI of the resource
        m_networkTypes.push_back(ResourceTypePrefix + ".network"); // resource type name.
        m_networkInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.
        m_networkInterfaces.push_back(BATCH_INTERFACE); // resource interface.
        m_networkInterfaces.push_back(LINK_INTERFACE); // resource interface.
        m_networkRep.setValue("value", m_networkValue);
        m_networkRep.setUri(m_networkUri);
        m_networkRep.setResourceTypes(m_networkTypes);
        m_networkRep.setResourceInterfaces(m_networkInterfaces);
        m_networkHandle = NULL;
    }
    ;

    /// This function internally calls registerResource API.
    void createResources(ResourceEntityHandler callback);

    void setNetworkRepresentation(OCRepresentation& rep);
    void setIPAddressRepresentation(OCRepresentation& rep);

    OCRepresentation getNetworkRepresentation();
    OCRepresentation getIPAddressRepresentation();

    std::string getNetworkUri();
    std::string getIPAddressUri();

    void factoryReset();

};

extern std::string defaultSecurityValue;
extern std::string defaultModeValue;

class SecurityCollection
{
public:

    // diagnostics members
    std::string m_securityUri;
    std::string m_securityValue;
    std::vector< std::string > m_securityTypes;
    std::vector< std::string > m_securityInterfaces;
    OCResourceHandle m_securityHandle;
    OCRepresentation m_securityRep;

    // factory reset members
    std::string m_modeUri;
    std::string m_modeValue;
    std::vector< std::string > m_modeTypes;
    std::vector< std::string > m_modeInterfaces;
    OCResourceHandle m_modeHandle;
    OCRepresentation m_modeRep;

public:
    /// Constructor
    SecurityCollection() :
            m_securityValue(defaultSecurityValue), m_modeValue(defaultModeValue)
    {
        m_modeUri = defaultURIPrefix + "/security/0/mode"; // URI of the resource
        m_modeTypes.push_back("oic.con.security.mode"); // resource type name.
        m_modeInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.

        m_modeRep.setUri(m_modeUri);
        m_modeRep.setResourceTypes(m_modeTypes);
        m_modeRep.setResourceInterfaces(m_modeInterfaces);
        m_modeRep.setValue("value", m_modeValue);
        m_modeHandle = NULL;

        m_securityUri = defaultURIPrefix + "/security"; // URI of the resource
        m_securityTypes.push_back("oic.con.security"); // resource type name.
        m_securityInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.
        m_securityInterfaces.push_back(BATCH_INTERFACE); // resource interface.
        m_securityInterfaces.push_back(LINK_INTERFACE); // resource interface.
        m_securityRep.setValue("value", m_securityValue);
        m_securityRep.setUri(m_securityUri);
        m_securityRep.setResourceTypes(m_securityTypes);
        m_securityRep.setResourceInterfaces(m_securityInterfaces);
        m_securityHandle = NULL;
    }
    ;

    /// Constructor
    SecurityCollection(std::string URIPrefix, std::string ResourceTypePrefix) :
            m_securityValue(defaultSecurityValue), m_modeValue(defaultModeValue)
    {
        m_modeUri = URIPrefix + "/security/0/mode"; // URI of the resource
        m_modeTypes.push_back(ResourceTypePrefix + ".security.mode"); // resource type name.
        m_modeInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.

        m_modeRep.setUri(m_modeUri);
        m_modeRep.setResourceTypes(m_modeTypes);
        m_modeRep.setResourceInterfaces(m_modeInterfaces);
        m_modeRep.setValue("value", m_modeValue);
        m_modeHandle = NULL;

        m_securityUri = URIPrefix + "/security"; // URI of the resource
        m_securityTypes.push_back(ResourceTypePrefix + ".security"); // resource type name.
        m_securityInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.
        m_securityInterfaces.push_back(BATCH_INTERFACE); // resource interface.
        m_securityInterfaces.push_back(LINK_INTERFACE); // resource interface.
        m_securityRep.setValue("value", m_securityValue);
        m_securityRep.setUri(m_securityUri);
        m_securityRep.setResourceTypes(m_securityTypes);
        m_securityRep.setResourceInterfaces(m_securityInterfaces);
        m_securityHandle = NULL;
    }
    ;

    /// This function internally calls registerResource API.
    void createResources(ResourceEntityHandler callback);

    void setSecurityRepresentation(OCRepresentation& rep);
    void setModeRepresentation(OCRepresentation& rep);

    OCRepresentation getSecurityRepresentation();
    OCRepresentation getModeRepresentation();

    std::string getSecurityUri();
    std::string getModeUri();

    void factoryReset();

};

extern std::string defaultConfigurationValue;
extern std::string defaultRegionValue;
static std::string defaultTimeLink = "/con/con/0/time";
static std::string defaultNetworkLink = "/con/con/0/network";
static std::string defaultSecurityLink = "/con/con/0/security";

class ConfigurationCollection
{
public:
    TimeCollection *myTimeCollection;
    NetworkCollection *myNetworkCollection;
    SecurityCollection *mySecurityCollection;

public:
    // Configuration members
    std::string m_configurationUri;
    std::string m_configurationValue;
    std::vector< std::string > m_configurationTypes;
    std::vector< std::string > m_configurationInterfaces;
    OCResourceHandle m_configurationHandle;
    OCRepresentation m_configurationRep;

    // Security members
    std::string m_regionUri;
    std::string m_regionValue;
    std::vector< std::string > m_regionTypes;
    std::vector< std::string > m_regionInterfaces;
    OCResourceHandle m_regionHandle;
    OCRepresentation m_regionRep;

    // Time members
    std::string m_timeUri;
    std::string m_timeLink;
    std::vector< std::string > m_timeTypes;
    std::vector< std::string > m_timeInterfaces;
    OCResourceHandle m_timeHandle;
    OCRepresentation m_timeRep;

    // Network members
    std::string m_networkUri;
    std::string m_networkLink;
    std::vector< std::string > m_networkTypes;
    std::vector< std::string > m_networkInterfaces;
    OCResourceHandle m_networkHandle;
    OCRepresentation m_networkRep;

    // Security members
    std::string m_securityUri;
    std::string m_securityLink;
    std::vector< std::string > m_securityTypes;
    std::vector< std::string > m_securityInterfaces;
    OCResourceHandle m_securityHandle;
    OCRepresentation m_securityRep;

public:
    /// Constructor
    ConfigurationCollection() :
            m_configurationValue(defaultConfigurationValue), m_regionValue(defaultRegionValue), m_timeLink(
                    defaultTimeLink), m_networkLink(defaultNetworkLink), m_securityLink(
                    defaultSecurityLink)
    {
        m_regionUri = defaultURIPrefix + "/0/region"; // URI of the resource
        m_regionTypes.push_back(defaultResourceTypePrefix + ".region"); // resource type name.
        m_regionInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.

        m_regionRep.setUri(m_regionUri);
        m_regionRep.setResourceTypes(m_regionTypes);
        m_regionRep.setResourceInterfaces(m_regionInterfaces);
        m_regionRep.setValue("value", m_regionValue);
        m_regionHandle = NULL;

        m_timeUri = defaultURIPrefix + "/0/time"; // URI of the resource
        m_timeTypes.push_back(defaultResourceTypePrefix + ".time"); // resource type name.
        m_timeInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.

        m_timeRep.setUri(m_timeUri);
        m_timeRep.setResourceTypes(m_timeTypes);
        m_timeRep.setResourceInterfaces(m_timeInterfaces);
        m_timeRep.setValue("link", m_timeLink);
        m_timeHandle = NULL;

        m_networkUri = defaultURIPrefix + "/0/net"; // URI of the resource
        m_networkTypes.push_back(defaultResourceTypePrefix + ".net"); // resource type name.
        m_networkInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.

        m_networkRep.setUri(m_networkUri);
        m_networkRep.setResourceTypes(m_networkTypes);
        m_networkRep.setResourceInterfaces(m_networkInterfaces);
        m_networkRep.setValue("link", m_networkLink);
        m_networkHandle = NULL;

        m_securityUri = defaultURIPrefix + "/0/sec"; // URI of the resource
        m_securityTypes.push_back(defaultResourceTypePrefix + ".sec"); // resource type name.
        m_securityInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.

        m_securityRep.setUri(m_securityUri);
        m_securityRep.setResourceTypes(m_securityTypes);
        m_securityRep.setResourceInterfaces(m_securityInterfaces);
        m_securityRep.setValue("link", m_securityLink);
        m_securityHandle = NULL;

        m_configurationUri = defaultURIPrefix + ""; // URI of the resource
        m_configurationTypes.push_back(defaultResourceTypePrefix + ""); // resource type name.
        m_configurationInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.
        m_configurationInterfaces.push_back(BATCH_INTERFACE); // resource interface.
        m_configurationInterfaces.push_back(LINK_INTERFACE); // resource interface.
        m_configurationRep.setValue("value", m_configurationValue);
        m_configurationRep.setUri(m_configurationUri);
        m_configurationRep.setResourceTypes(m_configurationTypes);
        m_configurationRep.setResourceInterfaces(m_configurationInterfaces);
        m_configurationHandle = NULL;

        myTimeCollection = new TimeCollection();
        myNetworkCollection = new NetworkCollection();
        mySecurityCollection = new SecurityCollection();
    }
    ;

    /// Constructor
    ConfigurationCollection(std::string URIPrefix, std::string ResourceTypePrefix) :
            m_configurationValue(defaultConfigurationValue), m_regionValue(defaultRegionValue), m_timeLink(
                    defaultTimeLink), m_networkLink(defaultNetworkLink), m_securityLink(
                    defaultSecurityLink)
    {
        m_regionUri = URIPrefix + "/0/region"; // URI of the resource
        m_regionTypes.push_back(ResourceTypePrefix + ".region"); // type name.
        m_regionInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.

        m_regionRep.setUri(m_regionUri);
        m_regionRep.setResourceTypes(m_regionTypes);
        m_regionRep.setResourceInterfaces(m_regionInterfaces);
        m_regionRep.setValue("value", m_regionValue);
        m_regionHandle = NULL;

        m_timeUri = URIPrefix + "/0/time"; // URI of the resource
        m_timeTypes.push_back(ResourceTypePrefix + ".time"); // resource type name.
        m_timeInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.

        m_timeRep.setUri(m_timeUri);
        m_timeRep.setResourceTypes(m_timeTypes);
        m_timeRep.setResourceInterfaces(m_timeInterfaces);
        m_timeRep.setValue("link", m_timeLink);
        m_timeHandle = NULL;

        m_networkUri = URIPrefix + "/0/net"; // URI of the resource
        m_networkTypes.push_back(ResourceTypePrefix + ".net"); // resource type name.
        m_networkInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.

        m_networkRep.setUri(m_networkUri);
        m_networkRep.setResourceTypes(m_networkTypes);
        m_networkRep.setResourceInterfaces(m_networkInterfaces);
        m_networkRep.setValue("link", m_networkLink);
        m_networkHandle = NULL;

        m_securityUri = URIPrefix + "/0/sec"; // URI of the resource
        m_securityTypes.push_back(ResourceTypePrefix + ".sec"); // resource type name.
        m_securityInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.

        m_securityRep.setUri(m_securityUri);
        m_securityRep.setResourceTypes(m_securityTypes);
        m_securityRep.setResourceInterfaces(m_securityInterfaces);
        m_securityRep.setValue("link", m_securityLink);
        m_securityHandle = NULL;

        m_configurationUri = URIPrefix + ""; // URI of the resource
        m_configurationTypes.push_back(ResourceTypePrefix + ""); // resource type name.
        m_configurationInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.
        m_configurationInterfaces.push_back(BATCH_INTERFACE); // resource interface.
        m_configurationInterfaces.push_back(LINK_INTERFACE); // resource interface.
        m_configurationRep.setValue("value", m_configurationValue);
        m_configurationRep.setUri(m_configurationUri);
        m_configurationRep.setResourceTypes(m_configurationTypes);
        m_configurationRep.setResourceInterfaces(m_configurationInterfaces);
        m_configurationHandle = NULL;

        myTimeCollection = new TimeCollection(URIPrefix, ResourceTypePrefix);
        myNetworkCollection = new NetworkCollection(URIPrefix, ResourceTypePrefix);
        mySecurityCollection = new SecurityCollection(URIPrefix, ResourceTypePrefix);
    }
    ;

    ~ConfigurationCollection()
    {
        free(myTimeCollection);
        free(myNetworkCollection);
        free(mySecurityCollection);
    }

    /// This function internally calls registerResource API.
    void createResources(ResourceEntityHandler callback);

    void setConfigurationRepresentation(OCRepresentation& rep);
    void setTimeRepresentation(OCRepresentation& rep);
    void setNetworkRepresentation(OCRepresentation& rep);
    void setSecurityRepresentation(OCRepresentation& rep);
    void setRegionRepresentation(OCRepresentation& rep);

    OCRepresentation getTimeRepresentation();
    OCRepresentation getNetworkRepresentation();
    OCRepresentation getSecurityRepresentation();
    OCRepresentation getRegionRepresentation();
    OCRepresentation getConfigurationRepresentation();

    std::string getConfigurationUri();
    std::string getTimeUri();
    std::string getNetworkUri();
    std::string getSecurityUri();
    std::string getRegionUri();

    void factoryReset();

};
