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

/**
 * @file
 *
 */

#include <OCApi.h>
#include <OCPlatform.h>
#include <cstdlib>

#include "ThingsDiagnostics.h"

using namespace OC;

namespace OIC
{
    std::map< std::string, DiagnosticsRequestEntry > diagnosticsRequestTable;
    ThingsDiagnostics* ThingsDiagnostics::thingsDiagnosticsInstance = NULL;

    DiagnosticsRequestEntry::DiagnosticsRequestEntry(std::string ID, DiagnosticsCallback callback,
                std::shared_ptr< OCResource > resource, std::string updateVal)
    {
        m_ID = ID;
        m_callback = callback;
        m_resource = resource;
        m_updateVal = updateVal;
    }

    DiagnosticsUnitInfo::DiagnosticsUnitInfo(std::string name,
                                            std::string attribute,
                                            std::string uri)
    {
        m_name = name;
        m_attribute = attribute;
        m_uri = uri;
    }

    std::string DiagnosticsUnitInfo::getJSON()
    {
        std::string res;

        res = "{\"name\":\"" + m_name + "\",\"attribute\":\"" + m_attribute + "\"}";

        return res;
    }

    ThingsDiagnostics::ThingsDiagnostics()
    {
        DiagnosticsUnitInfo unit[] =
                {
                { "rb", "Reboot", "/oic/diag"},
                { "ssc", "StartStatCollection", "/oic/diag"},
                { "fr", "Factory Reset", "/oic/diag" } };

        for (int i = 0; i < NUMDIAGUNIT; i++)
            DiagnosticsUnitTable.push_back(unit[i]);
    }

    ThingsDiagnostics::~ThingsDiagnostics()
    {
    }

    void ThingsDiagnostics::setGroupManager(GroupManager *groupmanager)
    {
        g_groupmanager = groupmanager;
    }

    ThingsDiagnostics* ThingsDiagnostics::getInstance()
    {
        if (thingsDiagnosticsInstance == NULL)
        {
            thingsDiagnosticsInstance = new ThingsDiagnostics();
        }
        return thingsDiagnosticsInstance;
    }

    void ThingsDiagnostics::deleteInstance()
    {
        if (thingsDiagnosticsInstance)
        {
            delete thingsDiagnosticsInstance;
            thingsDiagnosticsInstance = NULL;
        }
    }

    std::string ThingsDiagnostics::getAttributeByDiagnosticsName(DiagnosticsName name)
    {
        for (auto it = DiagnosticsUnitTable.begin(); DiagnosticsUnitTable.end() != it; it++)
        {
            if ((*it).m_name == name)
                return (*it).m_attribute;
        }

        return "";
    }

    std::string ThingsDiagnostics::getUriByDiagnosticsName(DiagnosticsName name)
    {
        for (auto it = DiagnosticsUnitTable.begin(); DiagnosticsUnitTable.end() != it; it++)
        {
            if ((*it).m_name == name)
                return (*it).m_uri;
        }

        return "";
    }

    std::string ThingsDiagnostics::getUpdateVal(std::string diag)
    {
        std::map< std::string, DiagnosticsRequestEntry >::iterator it =
                diagnosticsRequestTable.find(diag);

        if (it == diagnosticsRequestTable.end())
            return NULL;
        else
            return it->second.m_updateVal;

    }
    std::shared_ptr< OCResource > ThingsDiagnostics::getResource(std::string diag)
    {
        std::map< std::string, DiagnosticsRequestEntry >::iterator it =
                diagnosticsRequestTable.find(diag);

        if (it == diagnosticsRequestTable.end())
            return NULL;
        else
            return it->second.m_resource;
    }

    DiagnosticsCallback ThingsDiagnostics::getCallback(std::string diag)
    {
        std::map< std::string, DiagnosticsRequestEntry >::iterator it =
                diagnosticsRequestTable.find(diag);

        if (it == diagnosticsRequestTable.end())
            return NULL;
        else
            return it->second.m_callback;
    }

    std::string ThingsDiagnostics::getHostFromURI(std::string oldUri)
    {
        size_t f;
        std::string newUri;

        if ((f = oldUri.find("/factoryset/oic/")) != string::npos)
            newUri = oldUri.replace(f, oldUri.size(), "");
        else if ((f = oldUri.find("/oic/")) != string::npos)
            newUri = oldUri.replace(f, oldUri.size(), "");

        return newUri;
    }

    std::string ThingsDiagnostics::getListOfSupportedDiagnosticsUnits()
    {
        std::string res;

        res = "{\"Diagnostics Units\":[";

        auto it = DiagnosticsUnitTable.begin();
        while (1)
        {
            res = res + (*it).getJSON();
            it++;

            if (it == DiagnosticsUnitTable.end())
                break;
            else
                res += ",";
        }

        res += "]}";

        return res;
    }

    void ThingsDiagnostics::onGetChildInfoForUpdate(const HeaderOptions& headerOptions,
            const OCRepresentation& rep, const int eCode, std::string diag)
    {
        if (eCode != OC_STACK_OK)
        {
            std::cout << "onGet Response error: " << eCode << std::endl;
            getCallback(diag)(headerOptions, rep, eCode);
            return ;
        }

        std::cout << "GET request was successful" << std::endl;

        std::cout << "\tResource URI: " << rep.getUri() << std::endl;

        std::vector < OCRepresentation > children = rep.getChildren();
        for (auto oit = children.begin(); oit != children.end(); ++oit)
        {
            std::cout << "\t\tChild Resource URI: " << oit->getUri() << std::endl;
        }

        // Get information by using diagnostics name(diag)
        std::shared_ptr < OCResource > resource = getResource(diag);
        std::string actionstring = diag;
        std::string uri = getUriByDiagnosticsName(diag);
        std::string attrKey = diag;

        if (uri == "")
            return;

        if (resource)
        {
            // In this nest, we create a new action set of which name is the dignostics name.
            // Required information consists of a host address, URI, attribute key, and
            // attribute value.
            ActionSet *newActionSet = new ActionSet();
            newActionSet->actionsetName = diag;

            for (auto oit = children.begin(); oit != children.end(); ++oit)
            {
                Action *newAction = new Action();

                // oit->getUri() includes a host address as well as URI.
                // We should split these to each other and only use the host address to create
                // a child resource's URI. Note that the collection resource and its child
                // resource are located in same host.
                newAction->target = getHostFromURI(oit->getUri()) + uri;

                Capability *newCapability = new Capability();
                newCapability->capability = attrKey;
                newCapability->status = getUpdateVal(diag);

                newAction->listOfCapability.push_back(newCapability);
                newActionSet->listOfAction.push_back(newAction);
            }

            // Request to create a new action set by using the above actionSet
            g_groupmanager->addActionSet(resource, newActionSet,
                    std::function<
                            void(const HeaderOptions& headerOptions,
                                    const OCRepresentation& rep, const int eCode) >(
                            std::bind(&ThingsDiagnostics::onCreateActionSet, this,
                                    std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3, diag)));

            free(newActionSet);

        }
    }

    void ThingsDiagnostics::onCreateActionSet(const HeaderOptions& headerOptions,
            const OCRepresentation& rep, const int eCode, std::string diag)
    {
        if (eCode != OC_STACK_OK)
        {
            std::cout << "onPut Response error: " << eCode << std::endl;
            getCallback(diag)(headerOptions, rep, eCode);
            return ;
        }

        std::cout << "PUT request was successful" << std::endl;

        std::shared_ptr < OCResource > resource = getResource(diag);
        if (resource)
        {
            // Now, it is time to execute the action set.
            g_groupmanager->executeActionSet(resource, diag,
                    std::function<
                            void(const HeaderOptions& headerOptions,
                                    const OCRepresentation& rep, const int eCode) >(
                            std::bind(&ThingsDiagnostics::onExecuteForGroupAction, this,
                                    std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3, diag)));
        }
    }

    void ThingsDiagnostics::onExecuteForGroupAction(const HeaderOptions& headerOptions,
            const OCRepresentation& rep, const int eCode, std::string diag)
    {
        if (eCode != OC_STACK_OK)
        {
            std::cout << "onPut Response error: " << eCode << std::endl;
            getCallback(diag)(headerOptions, rep, eCode);
            return ;
        }

        std::cout << "PUT request was successful" << std::endl;
        getCallback(diag)(headerOptions, rep, eCode);

        // Delete the created actionset
        std::shared_ptr < OCResource > resource = getResource(diag);
        if (resource)
        {
            g_groupmanager->deleteActionSet(resource, diag,
                    std::function<
                            void(const HeaderOptions& headerOptions,
                                    const OCRepresentation& rep, const int eCode) >(
                            std::bind(&ThingsDiagnostics::onDeleteGroupAction, this,
                                    std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3, diag)));
        }
    }

    void ThingsDiagnostics::onDeleteGroupAction(const HeaderOptions& headerOptions,
            const OCRepresentation& rep, const int eCode, std::string diag)
    {
        if (eCode != OC_STACK_OK)
        {
            std::cout << "Delete actionset returned with error: " << eCode << diag << std::endl;
            return;
        }

        std::cout << "Deleted the actionset created!" << diag<< std::endl;
    }

    void ThingsDiagnostics::onPut(const HeaderOptions& headerOptions, const OCRepresentation& rep,
            const int eCode, std::string diag)
    {
        if (eCode != OC_STACK_OK)
        {
            std::cout << "onPut Response error: " << eCode << std::endl;
            getCallback(diag)(headerOptions, rep, eCode);
            return ;
        }

        std::cout << "PUT request was successful" << std::endl;

        getCallback(diag)(headerOptions, rep, eCode);

    }

    bool ThingsDiagnostics::isSimpleResource(std::shared_ptr< OCResource > resource)
    {
        for (unsigned int i = 0; i < resource->getResourceTypes().size(); ++i)
        {
            if (resource->getResourceTypes().at(0).find(".resourceset", 0) != std::string::npos)
                return false;
        }

        return true;
    }

    OCStackResult ThingsDiagnostics::reboot(std::shared_ptr< OCResource > resource,
            DiagnosticsCallback callback)
    {
        if (!resource)
        {
            std::cout << "resource is NULL\n";
            return OC_STACK_ERROR;
        }

        std::string diag = "rb";

        // Check the request queue if a previous request is still left. If so, remove it.
        std::map< std::string, DiagnosticsRequestEntry >::iterator iter =
                diagnosticsRequestTable.find(diag);
        if (iter != diagnosticsRequestTable.end())
            diagnosticsRequestTable.erase(iter);

        // Create new request entry stored in the queue
        DiagnosticsRequestEntry newCallback(diag, callback, resource, "true");
        diagnosticsRequestTable.insert(std::make_pair(diag, newCallback));

        QueryParamsMap query;
        OCRepresentation rep;

        if (isSimpleResource(resource))
        {
            // This resource is a simple resource. Just send a PUT message
            OCRepresentation rep;
            rep.setValue < std::string > (diag, "true");

            return resource->put(resource->getResourceTypes().at(0), DEFAULT_INTERFACE, rep, query,
                    std::function<
                            void(const HeaderOptions& headerOptions, const OCRepresentation& rep,
                                    const int eCode) >(
                            std::bind(&ThingsDiagnostics::onPut, this, std::placeholders::_1,
                                    std::placeholders::_2, std::placeholders::_3, diag)));
        }
        else
        {
            // This resource is a collection resource. It just acquires child resource's URI and
            // send GET massages to the child resources in turn.
            // First, request the child resources's URI.
            // TODO: Add a deletion of actionset
            return resource->get(resource->getResourceTypes().at(0), DEFAULT_INTERFACE, query,
                    std::function<
                            void(const HeaderOptions& headerOptions, const OCRepresentation& rep,
                                    const int eCode) >(
                            std::bind(&ThingsDiagnostics::onGetChildInfoForUpdate, this,
                                    std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3, diag)));
        }
    }

    OCStackResult ThingsDiagnostics::factoryReset(std::shared_ptr< OCResource > resource,
            DiagnosticsCallback callback)
    {
        if (!resource)
        {
            std::cout << "resource is NULL\n";
            return OC_STACK_ERROR;
        }

        std::string diag = "fr";

        // Check the request queue if a previous request is still left. If so, remove it.
        std::map< std::string, DiagnosticsRequestEntry >::iterator iter =
                diagnosticsRequestTable.find(diag);
        if (iter != diagnosticsRequestTable.end())
            diagnosticsRequestTable.erase(iter);

        // Create new request entry stored in the queue
        DiagnosticsRequestEntry newCallback(diag, callback, resource, "true");
        diagnosticsRequestTable.insert(std::make_pair(diag, newCallback));

        QueryParamsMap query;
        OCRepresentation rep;

        if (isSimpleResource(resource))
        {
            // This resource is a simple resource. Just send a PUT message
            OCRepresentation rep;
            rep.setValue < std::string > ("value", "true");

            return resource->put(resource->getResourceTypes().at(0), DEFAULT_INTERFACE, rep, query,
                    std::function<
                            void(const HeaderOptions& headerOptions, const OCRepresentation& rep,
                                    const int eCode) >(
                            std::bind(&ThingsDiagnostics::onPut, this, std::placeholders::_1,
                                    std::placeholders::_2, std::placeholders::_3, diag)));
        }
        else
        {
            // This resource is a collection resource. It just acquires child resource's URI and
            // send GET massages to the child resources in turn.
            // First, request the child resources's URI.
            // TODO: Add a deletion of actionset
            return resource->get(resource->getResourceTypes().at(0), DEFAULT_INTERFACE, query,
                    std::function<
                            void(const HeaderOptions& headerOptions, const OCRepresentation& rep,
                                    const int eCode) >(
                            std::bind(&ThingsDiagnostics::onGetChildInfoForUpdate, this,
                                    std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3, diag)));
        }
    }
}

