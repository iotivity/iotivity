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

#define COAP_IP                 "134.134.161.33"
#define COAP_PORT               5683
#define COAP_MODE               ModeType::Client
#define COAP_SRVTYPE            ServiceType::InProc

#define COAP_SERVER_ADDR        "coap://224.0.1.187/oc/core?rt=core.SoftSensorManager"

SSMClient::SSMClient()
{
    m_sem.create(100000);
    m_pPlatform = NULL;
    m_appListener = NULL;
    m_retResponse = SSM_ERROR;
    _findResource();
    _createQueryEngine();
}

SSMClient::~SSMClient()
{
    _releaseQueryEngine(m_queryEngineId);
    if (m_pPlatform)
    {
        delete m_pPlatform;
        m_pPlatform = NULL;
    }
}

void SSMClient::_findResource()
{
    // Create PlatformConfig object
    PlatformConfig cfg(COAP_SRVTYPE, COAP_MODE, COAP_IP, COAP_PORT, QualityOfService::NonConfirmable);

    // Create a OCPlatform instance.
    // Note: Platform creation is synchronous call.
    try
    {
        m_pPlatform = new OCPlatform(cfg);

        // Find all resources
        OCStackResult result;
        if ((result = m_pPlatform->findResource("", COAP_SERVER_ADDR,
                                                std::bind(&SSMClient::onFoundResource, this, std::placeholders::_1)))
            != OC_STACK_OK)
        {
            delete m_pPlatform;
            m_pPlatform = NULL;
            return;
        }

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

    AttributeMap requestAttributeMap;
    AttributeValues commandValue;

    QueryParamsMap queryParamsMap;

    commandValue.push_back("CreateQueryEngine");

    requestAttributeMap["command"] = commandValue;

    rep.setAttributeMap(requestAttributeMap);

    if (m_SSMResource->put(rep, queryParamsMap,
                           std::bind(&SSMClient::onCreateQueryEngine, this, std::placeholders::_1,
                                     std::placeholders::_2)) != OC_STACK_OK)
    {
        return;
    }

    m_sem.wait();
}

void SSMClient::_releaseQueryEngine(std::string queryEngineId)
{
    OCRepresentation rep;

    AttributeMap requestAttributeMap;
    AttributeValues commandValue;
    AttributeValues queryEngineIdValue;

    QueryParamsMap queryParamsMap;

    commandValue.push_back("ReleaseQueryEngine");

    requestAttributeMap["command"] = commandValue;

    queryEngineIdValue.push_back(queryEngineId);

    requestAttributeMap["queryEngineId"] = queryEngineIdValue;

    rep.setAttributeMap(requestAttributeMap);

    m_SSMResource->put(rep, queryParamsMap,
                       std::bind(&SSMClient::onReleaseQueryEngine, this, std::placeholders::_1,
                                 std::placeholders::_2));

    m_sem.wait();
}

SSMReturn SSMClient::registerQuery(std::string queryString, ISSMClientListener *listener,
                                   std::string &cqid)
{
    OCRepresentation rep;

    AttributeMap requestAttributeMap;
    AttributeValues commandValue;
    AttributeValues queryEngineIdValue;
    AttributeValues contextQueryValue;

    QueryParamsMap queryParamsMap;

    commandValue.push_back("ExecuteContextQuery");

    requestAttributeMap["command"] = commandValue;

    queryEngineIdValue.push_back(m_queryEngineId);

    requestAttributeMap["queryEngineId"] = queryEngineIdValue;

    contextQueryValue.push_back(queryString);

    requestAttributeMap["contextQuery"] = contextQueryValue;

    m_appListener = listener;

    rep.setAttributeMap(requestAttributeMap);

    if (m_SSMResource->put(rep, queryParamsMap,
                           std::bind(&SSMClient::onRegisterQuery, this, std::placeholders::_1,
                                     std::placeholders::_2)) != OC_STACK_OK)
        return SSM_ERROR_NETWORK;

    m_sem.wait();

    if (m_retResponse == SSM_SUCCESS)
    {
        cqid = m_responseAttributeMap["CQID"].back();
    }

    return m_retResponse;
}

SSMReturn SSMClient::unregisterQuery(std::string cqid)
{
    OCRepresentation rep;

    AttributeMap requestAttributeMap;
    AttributeValues commandValue;
    AttributeValues queryEngineIdValue;
    AttributeValues cqidValue;

    QueryParamsMap queryParamsMap;

    commandValue.push_back("KillContextQuery");

    requestAttributeMap["command"] = commandValue;

    queryEngineIdValue.push_back(m_queryEngineId);

    requestAttributeMap["queryEngineId"] = queryEngineIdValue;

    cqidValue.push_back(cqid);

    requestAttributeMap["CQID"] = cqidValue;

    rep.setAttributeMap(requestAttributeMap);

    if (m_SSMResource->put(rep, queryParamsMap,
                           std::bind(&SSMClient::onUnregisterQuery, this, std::placeholders::_1,
                                     std::placeholders::_2)) != OC_STACK_OK)
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

void SSMClient::onCreateQueryEngine(const OCRepresentation &rep, const int eCode)
{
    if (eCode != 0)
    {
        m_retResponse = SSM_ERROR_NETWORK;
        goto CLEANUP;
    }

    m_responseAttributeMap = rep.getAttributeMap();
    m_queryEngineId = m_responseAttributeMap["queryEngineId"].back();
    m_responseAttributeMap["queryEngineId"].pop_back();
    m_retResponse = SSM_SUCCESS;

CLEANUP: m_sem.release();
}

void SSMClient::onRegisterQuery(const OCRepresentation &rep, const int eCode)
{
    QueryParamsMap queryParamsMap;

    if (eCode != 0)
    {
        m_retResponse = SSM_ERROR_NETWORK;
        goto CLEANUP;
    }

    m_responseAttributeMap = rep.getAttributeMap();

    if (m_responseAttributeMap.find("error") != m_responseAttributeMap.end())
    {
        m_retResponse = SSM_ERROR_QUERY_PARSING;
        goto CLEANUP;
    }

    m_SSMResource->observe(ObserveType::Observe, queryParamsMap,
                           std::bind(&SSMClient::onObserve, this, std::placeholders::_1, std::placeholders::_2));

    m_retResponse = SSM_SUCCESS;

CLEANUP: m_sem.release();
}

void SSMClient::onUnregisterQuery(const OCRepresentation &rep, const int eCode)
{
    if (eCode != 0)
    {
        m_retResponse = SSM_ERROR_NETWORK;
        goto CLEANUP;
    }

    m_responseAttributeMap = rep.getAttributeMap();

    if (m_responseAttributeMap.find("error") != m_responseAttributeMap.end())
    {
        m_retResponse = SSM_ERROR_NO_QUERY;
        goto CLEANUP;
    }

    m_retResponse = SSM_SUCCESS;

CLEANUP: m_sem.release();
}

void SSMClient::onReleaseQueryEngine(const OCRepresentation &rep, const int eCode)
{
    if (eCode != 0)
    {
        m_retResponse = SSM_ERROR_NETWORK;
        goto CLEANUP;
    }

    m_responseAttributeMap = rep.getAttributeMap();

    m_retResponse = SSM_SUCCESS;

CLEANUP: m_sem.release();
}

void SSMClient::onObserve(const OCRepresentation &rep, const int &eCode)
{
    SSMReturn ret = SSM_SUCCESS;

    if (eCode != 0)
    {
        ret = SSM_ERROR_NETWORK;
    }

    m_appListener->onRegisterQuery(rep.getAttributeMap(), ret);
}
