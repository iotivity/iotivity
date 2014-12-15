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

#include <iostream>
#include <string>

#include "SSMClient.h"
#include "OCPlatform.h"
#include "OCApi.h"

const char *SSM_RESOURCE_URI = "/service/SoftSensorManager";

#define SSM_RESOURCE_TYPE       = "core.SoftSensorManager"

#define COAP_SERVER_ADDR        "coap://224.0.1.187/oc/core?rt=core.SoftSensorManager"

SSMClient::SSMClient()
{
    m_sem.create(100000);
    m_appListener = NULL;
    m_retResponse = SSM_ERROR;
    _findResource();
    _createQueryEngine();
}

SSMClient::~SSMClient()
{
    _releaseQueryEngine(m_queryEngineId);
}

void SSMClient::_findResource()
{
    // Create PlatformConfig object
    PlatformConfig cfg(OC::ServiceType::InProc, OC::ModeType::Both,
                       "0.0.0.0", 0, OC::QualityOfService::LowQos);

    // Create a OCPlatform instance.
    // Note: Platform creation is synchronous call.
    try
    {
        OCPlatform::Configure(cfg);

        // Find all resources
        OCStackResult result;
        if ((result = OCPlatform::findResource("", COAP_SERVER_ADDR,
                                               std::bind(&SSMClient::onFoundResource, this, std::placeholders::_1)))
            != OC_STACK_OK)
        {
            std::cout << "Faile to Find Resource... " << std::endl;
            return;
        }
        std::cout << "Find Resource... " << std::endl;

        m_sem.wait();

    }
    catch (OCException e)
    {
        //log(e.what());
    }
}

void SSMClient::_createQueryEngine(void)
{
    OCRepresentation rep;

    QueryParamsMap queryParamsMap;

    rep.setValue("command", std::string("CreateQueryEngine"));

    if (m_SSMResource->put(rep, queryParamsMap,
                           std::bind(&SSMClient::onCreateQueryEngine, this, std::placeholders::_1,
                                     std::placeholders::_2, std::placeholders::_3)) != OC_STACK_OK)
    {
        return;
    }

    m_sem.wait();
}

void SSMClient::_releaseQueryEngine(std::string queryEngineId)
{
    OCRepresentation rep;

    QueryParamsMap queryParamsMap;

    rep.setValue("command", std::string("ReleaseQueryEngine"));
    rep.setValue("queryEngineId", queryEngineId);

    m_SSMResource->put(rep, queryParamsMap,
                       std::bind(&SSMClient::onReleaseQueryEngine, this, std::placeholders::_1,
                                 std::placeholders::_2, std::placeholders::_3));

    m_sem.wait();
}

SSMReturn SSMClient::registerQuery(std::string queryString, ISSMClientListener *listener,
                                   std::string &cqid)
{
    OCRepresentation rep;

    QueryParamsMap queryParamsMap;


    rep.setValue("command", std::string("ExecuteContextQuery"));
    rep.setValue("queryEngineId", m_queryEngineId);
    rep.setValue("contextQuery", queryString);

    m_appListener = listener;

    if (m_SSMResource->put(rep, queryParamsMap,
                           std::bind(&SSMClient::onRegisterQuery, this, std::placeholders::_1,
                                     std::placeholders::_2, std::placeholders::_3)) != OC_STACK_OK)
        return SSM_ERROR_NETWORK;

    m_sem.wait();

    if (m_retResponse == SSM_SUCCESS)
    {
        cqid = m_responseAttributeMap["CQID"];
    }

    return m_retResponse;
}

SSMReturn SSMClient::unregisterQuery(std::string cqid)
{
    OCRepresentation rep;

    QueryParamsMap queryParamsMap;

    rep.setValue("command", std::string("KillContextQuery"));
    rep.setValue("queryEngineId", m_queryEngineId);
    rep.setValue("CQID", cqid);

    if (m_SSMResource->put(rep, queryParamsMap,
                           std::bind(&SSMClient::onUnregisterQuery, this, std::placeholders::_1,
                                     std::placeholders::_2, std::placeholders::_3)) != OC_STACK_OK)
        return SSM_ERROR_NETWORK;

    m_sem.wait();

    return m_retResponse;
}

/**
 * Callback Function List.
 */
// Callback to found resources
void SSMClient::onFoundResource(std::shared_ptr< OCResource > resource)
{
    std::string resourceURI;
    try
    {
        // Do some operations with resource object.
        if (resource)
        {
            // Get the resource URI
            resourceURI = resource->uri();

            if (resourceURI.compare(SSM_RESOURCE_URI) == 0)
            {
                m_SSMResource = resource;
            }
        }
    }
    catch (std::exception &e)
    {
        //log(e.what());
    }

    m_sem.release();
}

void SSMClient::onCreateQueryEngine(const HeaderOptions &headerOptions, const OCRepresentation &rep,
                                    const int eCode)
{
    if (eCode != 0)
    {
        m_retResponse = SSM_ERROR_NETWORK;
        goto CLEANUP;
    }

    m_queryEngineId = rep.getValue<std::string>("queryEngineId");
    m_retResponse = SSM_SUCCESS;

CLEANUP: m_sem.release();
}

void SSMClient::onRegisterQuery(const HeaderOptions &headerOptions, const OCRepresentation &rep,
                                const int eCode)
{
    QueryParamsMap queryParamsMap;

    if (eCode != 0)
    {
        m_retResponse = SSM_ERROR_NETWORK;
        goto CLEANUP;
    }

    if (rep.hasAttribute("error"))
    {
        m_retResponse = SSM_ERROR_QUERY_PARSING;
        goto CLEANUP;
    }

    m_SSMResource->observe(ObserveType::Observe, queryParamsMap,
                           std::bind(&SSMClient::onObserve, this, std::placeholders::_1,
                                     std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

    m_responseAttributeMap.clear();

    m_responseAttributeMap["CQID"] = rep.getValue<std::string>("CQID");

    m_retResponse = SSM_SUCCESS;

CLEANUP: m_sem.release();
}

void SSMClient::onUnregisterQuery(const HeaderOptions &headerOptions, const OCRepresentation &rep,
                                  const int eCode)
{
    if (eCode != 0)
    {
        m_retResponse = SSM_ERROR_NETWORK;
        goto CLEANUP;
    }

    if (rep.hasAttribute("error"))
    {
        m_retResponse = SSM_ERROR_NO_QUERY;
        goto CLEANUP;
    }

    m_responseAttributeMap.clear();

    m_retResponse = SSM_SUCCESS;

CLEANUP: m_sem.release();
}

void SSMClient::onReleaseQueryEngine(const HeaderOptions &headerOptions,
                                     const OCRepresentation &rep, const int eCode)
{
    if (eCode != 0)
    {
        m_retResponse = SSM_ERROR_NETWORK;
        goto CLEANUP;
    }

    m_retResponse = SSM_SUCCESS;

CLEANUP: m_sem.release();
}

void SSMClient::onObserve(const HeaderOptions &headerOptions, const OCRepresentation &rep,
                          const int &eCode, const int &sequenceNumber)
{
    SSMReturn ret = SSM_SUCCESS;

    if (eCode != 0)
    {
        ret = SSM_ERROR_NETWORK;
    }

    m_appListener->onRegisterQuery(rep.getJSONRepresentation(), ret);
}
