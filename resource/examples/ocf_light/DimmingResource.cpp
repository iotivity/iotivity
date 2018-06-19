/*
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Copyright 2018 Intel Corporation All Rights Reserved.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
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
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

#include <iostream>

#include "DimmingResource.h"
#include "OCPlatform.h"

#define INTERFACE_KEY "if"

using namespace OC;
namespace PH = std::placeholders;

DimmingResource::DimmingResource(std::string resourceUri):
        m_interestedObservers{},
        m_var_value_dimmingSetting{100},
        m_var_value_n{},
        m_var_value_if{},
        m_var_value_rt{}
{
    std::cout << "Running: DimmingResource constructor" << std::endl;
    std::string resourceURI = "/dimming";

    // VS2013 will fail list initialization so use array initialization
    m_resourceUri = resourceUri;
    m_RESOURCE_TYPE[0] = "oic.r.light.dimming";
    m_RESOURCE_INTERFACE[0] = "oic.if.baseline";
    m_RESOURCE_INTERFACE[1] = "oic.if.a";
    m_IF_UPDATE[0] = "oic.if.a";
    m_IF_UPDATE[1] = "oic.if.rw";
    m_IF_UPDATE[2] = "oic.if.baseline";
    m_var_name_dimmingSetting = "dimmingSetting";
    m_var_name_n = "n";
    m_var_name_if = "if";
    m_var_name_rt = "rt";

    // initialize member variables /dimming
    m_var_value_dimmingSetting = 0; // current value of property "dimmingSetting"
    m_var_value_n = "";  // current value of property "n"
    // initialize vector if
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    // initialize vector rt
    m_var_value_rt.push_back("oic.r.light.dimming");
}

DimmingResource::~DimmingResource(void) { }

OCStackResult DimmingResource::registerResource(uint8_t resourceProperty)
{
    EntityHandler cb = std::bind(&DimmingResource::entityHandler, this, PH::_1);
    OCStackResult result = OC_STACK_ERROR;
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if (OC_STACK_OK != result)
    {
        std::cerr << "Failed to register BinarySwitchResoruce." << std::endl;
        return result;
    }

    /// add the additional resource types
    for( size_t a = 1; a < (sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0])); a++ )
    {
        result = OCPlatform::bindTypeToResource(m_resourceHandle, m_RESOURCE_TYPE[a].c_str());
        if (OC_STACK_OK != result)
        {
            std::cerr << "Could not bind resource type:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }

    // add the additional interfaces
    for( size_t a = 1; a < (sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0])); a++)
    {
        result = OCPlatform::bindInterfaceToResource(m_resourceHandle, m_RESOURCE_INTERFACE[a].c_str());
        if (OC_STACK_OK != result)
        {
            std::cerr << "Could not bind interface:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }

    std::cout << "DimmingResource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << (sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0])) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << (sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0])) << std::endl;

    return result;
}

int DimmingResource::getDimmingSetting(void)
{
    return m_var_value_dimmingSetting;
}

void DimmingResource::setDimmingSetting(int dimmingSetting)
{
    if (dimmingSetting >= 0 && dimmingSetting <= 100)
    {
        m_var_value_dimmingSetting = dimmingSetting;
        std::cout << "\t\t" << "property 'dimmingSetting': " << m_var_value_dimmingSetting << std::endl;
    } else {
        std::cerr << "the dimmingSetting must between 0 and 100" << std::endl;
    }
}

OCStackResult DimmingResource::sendNotification(void)
{
    OCStackResult sResult = OC_STACK_OK;
    if ( m_interestedObservers.size() > 0) {
        std::cout << "Notifying list"  << m_interestedObservers.size() << " of observers\n";
        auto pResponse = std::make_shared<OC::OCResourceResponse>();
        sResult = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                                    m_interestedObservers,
                                                    pResponse);
    }
    return sResult;
}

OC::OCRepresentation DimmingResource::get(OC::QueryParamsMap queries)
{
    OC_UNUSED(queries);

    m_rep.setValue(m_var_name_dimmingSetting, m_var_value_dimmingSetting );
    m_rep.setValue(m_var_name_n, m_var_value_n );
    m_rep.setValue(m_var_name_if,  m_var_value_if );
    m_rep.setValue(m_var_name_rt,  m_var_value_rt );

    return m_rep;
}

OCEntityHandlerResult DimmingResource::post(OC::QueryParamsMap queries, const OC::OCRepresentation& rep)
{
    OCEntityHandlerResult ehResult = OC_EH_OK;
    OC_UNUSED(queries);

    try {
        if (rep.hasAttribute(m_var_name_dimmingSetting))
        {
            // allocate the variable
            int value;
            // get the actual value from the payload
            rep.getValue(m_var_name_dimmingSetting, value);

            // value exist in payload
        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    if (ehResult == OC_EH_OK)
    {
        // no error: assign the variables
        try {
            // value exist in payload
            if (rep.getValue(m_var_name_dimmingSetting, m_var_value_dimmingSetting ))
            {
                std::cout << "\t\t" << "property 'dimmingSetting': " << m_var_value_dimmingSetting << std::endl;
            }
            else
            {
                std::cout << "\t\t" << "property 'dimmingSetting' not found in the representation" << std::endl;
            }
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }

        try {
            if (rep.getValue(m_var_name_n, m_var_value_n ))
            {
                std::cout << "\t\t" << "property 'n' : " << m_var_value_n << std::endl;
            }
            else
            {
                std::cout << "\t\t" << "property 'n' not found in the representation" << std::endl;
            }
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }

        try {
            if (rep.hasAttribute(m_var_name_if))
            {
                rep.getValue(m_var_name_if, m_var_value_if);
                int first = 1;
                std::cout << "\t\t" << "property 'if' : " ;
                for(auto myvar: m_var_value_if)
                {
                    if (first)
                    {
                        std::cout << myvar;
                        first = 0;
                    }
                    else
                    {
                        std::cout << "," << myvar;
                    }
                }
                std::cout <<  std::endl;
            }
            else
            {
                std::cout << "\t\t" << "property 'if' not found in the representation" << std::endl;
            }
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }

        try {
            if (rep.hasAttribute(m_var_name_rt))
            {
                rep.getValue(m_var_name_rt, m_var_value_rt);
                int first = 1;
                std::cout << "\t\t" << "property 'rt' : " ;
                for(auto myvar: m_var_value_rt)
                {
                    if (first)
                    {
                        std::cout << myvar;
                        first = 0;
                    }
                    else
                    {
                        std::cout << "," << myvar;
                    }
                }
                std::cout <<  std::endl;
            }
            else
            {
                std::cout << "\t\t" << "property 'rt' not found in the representation" << std::endl;
            }
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
    }
    return ehResult;
}

OCEntityHandlerResult DimmingResource::entityHandler(std::shared_ptr<OC::OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;

    if (request)
    {
        std::cout << "In entity handler for DimmingResource, URI is : "
                << request->getResourceUri() << std::endl;

        // Check for query params (if any)
        QueryParamsMap queries = request->getQueryParameters();
        if (!queries.empty())
        {
            std::cout << "\nQuery processing up to entityHandler" << std::endl;
        }
        for (auto it : queries)
        {
            std::cout << "Query key: " << it.first << " value : " << it.second
                    << std::endl;
        }
        // get the value, so that we can AND it to check which flags are set
        int requestFlag = request->getRequestHandlerFlag();

        if (requestFlag & RequestHandlerFlag::RequestFlag)
        {
            // request flag is set
            auto pResponse = std::make_shared<OC::OCResourceResponse>();
            pResponse->setRequestHandle(request->getRequestHandle());
            pResponse->setResourceHandle(request->getResourceHandle());

            if (request->getRequestType() == "GET")
            {
                std::cout<<"DimmingResource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if (OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }

            else if (request->getRequestType() == "POST")
            {
                std::cout <<"DimmingResource Post Request"<<std::endl;
                bool  handle_post = true;

                if (queries.size() > 0)
                {
                    for (const auto &eachQuery : queries)
                    {
                        std::string key = eachQuery.first;
                        if (key.compare(INTERFACE_KEY) == 0)
                        {
                            std::string value = eachQuery.second;
                            if (in_updatable_interfaces(value) == false)
                            {
                                std::cout << "Update request received via interface: " << value
                                        << " . This interface is not authorized to update resource!!" << std::endl;
                                pResponse->setResponseResult(OCEntityHandlerResult::OC_EH_FORBIDDEN);
                                handle_post = false;
                                ehResult = OC_EH_ERROR;
                                break;
                            }
                        }
                    }
                }
                if (handle_post)
                {
                    ehResult = post(queries, request->getResourceRepresentation());
                    if (ehResult == OC_EH_OK)
                    {
                        pResponse->setResourceRepresentation(get(queries), "");
                    }
                    else
                    {
                        pResponse->setResponseResult(OCEntityHandlerResult::OC_EH_ERROR);
                    }
                    if (OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                    {
                        if (OC_STACK_OK != sendNotification() )
                        {
                            std::cerr << "NOTIFY failed." << std::endl;
                        }
                    }
                }
            }
            else
            {
                std::cout << "DimmingResource unsupported request type (delete,put,..)"
                        << request->getRequestType() << std::endl;
                pResponse->setResponseResult(OC_EH_ERROR);
                OCPlatform::sendResponse(pResponse);
                ehResult = OC_EH_ERROR;
            }
        }

        if (requestFlag & RequestHandlerFlag::ObserverFlag)
        {
            // observe flag is set
            std::cout << "\t\trequestFlag : Observer ";
            ObservationInfo observationInfo = request->getObservationInfo();
            ((ObserveAction::ObserveRegister == observationInfo.action) ? std::cout << "Register\n" : std::cout << "Unregister\n");
            if (ObserveAction::ObserveRegister == observationInfo.action)
            {
                // add observer
                m_interestedObservers.push_back(observationInfo.obsId);
            }
            else if (ObserveAction::ObserveUnregister == observationInfo.action)
            {
                // delete observer
                m_interestedObservers.erase(std::remove(
                                            m_interestedObservers.begin(),
                                            m_interestedObservers.end(),
                                            observationInfo.obsId),
                                            m_interestedObservers.end());
            }
            ehResult = OC_EH_OK;
        }
    }
    return ehResult;
}
