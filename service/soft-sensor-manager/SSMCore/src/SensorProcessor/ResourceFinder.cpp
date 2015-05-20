/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
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
#include "ResourceFinder.h"

SSMRESULT CResourceFinder::finalConstruct()
{
    SSMRESULT res = SSM_E_FAIL;

    OC::PlatformConfig cfg(OC::ServiceType::InProc, OC::ModeType::Both, "0.0.0.0", 0,
                           OC::QualityOfService::LowQos);

    SSM_CLEANUP_ASSERT(CreateGlobalInstance(OID_ITasker, (IBase **)&m_pTasker));

    OC::OCPlatform::Configure(cfg);

    m_pResourceFinderEvent = NULL;

    m_multicastPresenceHandle = nullptr;

CLEANUP:
    return res;
}

void CResourceFinder::finalRelease()
{
}

SSMRESULT CResourceFinder::registerResourceFinderEvent(IResourceFinderEvent *pEvent)
{
    m_pResourceFinderEvent = pEvent;
    return SSM_S_OK;
}

void CResourceFinder::onResourceFound(std::shared_ptr< OC::OCResource > resource)
{
    if (resource)
    {
        std::string path = resource->host() + resource->uri();

        if (m_mapResourceHandler.find(path) != m_mapResourceHandler.end())
            return;

        intptr_t      *pMessage = new intptr_t [2];
        pMessage[0] = RESOURCE_DISCOVER_REQUESTPROFILE;
        pMessage[1] = reinterpret_cast<intptr_t> (new  std::shared_ptr<OC::OCResource>(resource));

        m_pTasker->addTask(this, pMessage);
    }
}

void CResourceFinder::presenceHandler(OCStackResult result, const unsigned int nonce,
                                      const std::string &hostAddress)
{
    SSMRESULT res = SSM_E_FAIL;
    OCStackResult ret = OC_STACK_ERROR;
    intptr_t *pMessage = NULL;
    std::ostringstream requestURI;

    switch (result)
    {
        case OC_STACK_OK:
            requestURI << "coap://" << hostAddress << "/oc/core?rt=SSManager.Sensor";

            ret = OC::OCPlatform::findResource("", requestURI.str(), OC_ALL,
                                               std::bind(&CResourceFinder::onResourceFound, this, std::placeholders::_1));

            if (ret != OC_STACK_OK)
                SSM_CLEANUP_ASSERT(SSM_E_FAIL);

            break;

        case OC_STACK_PRESENCE_STOPPED:
            if (m_mapResources.find(hostAddress) != m_mapResources.end())
            {
                while (!m_mapResources[hostAddress].empty())
                {
                    pMessage = new intptr_t[2];
                    pMessage[0] = RESOURCE_DISCOVER_UNINSTALL_RESOURCE;
                    pMessage[1] = reinterpret_cast<intptr_t> (m_mapResourceHandler[m_mapResources[hostAddress].back()]);
                    m_mapResources[hostAddress].pop_back();
                    m_pTasker->addTask(this, pMessage);
                }

                m_mapResources.erase(hostAddress);
            }
            break;

        case OC_STACK_PRESENCE_TIMEOUT:
            break;

        case OC_STACK_VIRTUAL_DO_NOT_HANDLE:
            break;

        default:
            break;
    }

CLEANUP:
    ;
}

SSMRESULT CResourceFinder::startResourceFinder()
{
    SSMRESULT res = SSM_E_FAIL;
    OCStackResult ret = OC_STACK_ERROR;

    std::ostringstream requestURI;
    requestURI << OC_MULTICAST_DISCOVERY_URI << "?rt=SSManager.Sensor";

    std::ostringstream multicastPresenceURI;
    multicastPresenceURI << "coap://" << OC_MULTICAST_PREFIX;

    ret = OC::OCPlatform::findResource("", requestURI.str(), OC_ALL,
                                       std::bind(&CResourceFinder::onResourceFound, this, std::placeholders::_1));

    if (ret != OC_STACK_OK)
        SSM_CLEANUP_ASSERT(SSM_E_FAIL);

    ret = OC::OCPlatform::subscribePresence(m_multicastPresenceHandle, multicastPresenceURI.str(),
                                            "SSManager.Sensor", OC_ALL, std::bind(&CResourceFinder::presenceHandler, this,
                                                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    if (ret != OC_STACK_OK)
        SSM_CLEANUP_ASSERT(SSM_E_FAIL);

    res = SSM_S_OK;

CLEANUP:
    return res;
}

SSMRESULT CResourceFinder::stopResourceFinder()
{
    SSMRESULT res = SSM_E_FAIL;
    OCStackResult ret = OC_STACK_ERROR;

    ret = OC::OCPlatform::unsubscribePresence(m_multicastPresenceHandle);

    if (ret != OC_STACK_OK)
        SSM_CLEANUP_ASSERT(SSM_E_FAIL);

    m_multicastPresenceHandle = nullptr;

    res = SSM_S_OK;

CLEANUP:
    return res;
}

SSMRESULT CResourceFinder::startObserveResource(ISSMResource *pSensor, IEvent *pEvent)
{
    return m_mapResourceHandler[pSensor->name]->startObserve(pEvent);
}

SSMRESULT CResourceFinder::stopObserveResource(ISSMResource *pSensor)
{
    return m_mapResourceHandler[pSensor->name]->stopObserve();
}

void CResourceFinder::onExecute(void *pArg)
{
    SSMRESULT res = SSM_E_FAIL;
    OCStackResult ret = OC_STACK_ERROR;
    OC::QueryParamsMap queryParams;
    OICResourceHandler *pResourceHandler = NULL;
    intptr_t                 *pMessage = reinterpret_cast<intptr_t *>(pArg);
    std::shared_ptr< OC::OCResource > *pResource = NULL;
    OC::OCPlatform::OCPresenceHandle presenceHandle = NULL;

    std::string resourceHostAddress = "";
    std::string resourceFullPath = "";

    switch (pMessage[0])
    {
        case RESOURCE_DISCOVER_REQUESTPROFILE:
            pResource = (std::shared_ptr< OC::OCResource > *) pMessage[1];
            pResourceHandler = new OICResourceHandler();
            SSM_CLEANUP_ASSERT(pResourceHandler->initHandler(*pResource, this));

            resourceFullPath = pResource->get()->host() + pResource->get()->uri();

            resourceHostAddress = pResource->get()->host();
            resourceHostAddress.erase(0, 7);        // erase 'coap://'

            m_mapResourceHandler[resourceFullPath] = pResourceHandler;

            m_mapResources[resourceHostAddress].push_back(resourceFullPath);

            ret = pResource->get()->get(queryParams, std::bind(&OICResourceHandler::onGetResourceProfile,
                                        pResourceHandler, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

            if (ret != OC_STACK_OK)
                SSM_CLEANUP_ASSERT(SSM_E_FAIL);

            break;

        case RESOURCE_DISCOVER_INSTALL_RESOURCE:
            if (m_mapResourcePresenceHandles.find(((ISSMResource *)pMessage[1])->ip) ==
                m_mapResourcePresenceHandles.end())
            {
                ret = OC::OCPlatform::subscribePresence(presenceHandle, ((ISSMResource *)pMessage[1])->ip,
                                                        "SSManager.Sensor", OC_ALL, std::bind(&CResourceFinder::presenceHandler, this,
                                                                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

                if (ret != OC_STACK_OK)
                    SSM_CLEANUP_ASSERT(SSM_E_FAIL);

                m_mapResourcePresenceHandles[((ISSMResource *)pMessage[1])->ip] = presenceHandle;
            }

            m_pResourceFinderEvent->onResourceFound((ISSMResource *)pMessage[1]);

            if (ret != OC_STACK_OK)
                SSM_CLEANUP_ASSERT(SSM_E_FAIL);

            break;

        case RESOURCE_DISCOVER_UNINSTALL_RESOURCE:
            m_pResourceFinderEvent->onResourceLost(&((OICResourceHandler *) pMessage[1])->m_SSMResource);

            if (m_mapResourcePresenceHandles.find(((OICResourceHandler *)pMessage[1])->m_SSMResource.ip) !=
                m_mapResourcePresenceHandles.end())
            {
                ret = OC::OCPlatform::unsubscribePresence(m_mapResourcePresenceHandles[((
                            OICResourceHandler *)pMessage[1])->m_SSMResource.ip]);

                if (ret != OC_STACK_OK)
                    SSM_CLEANUP_ASSERT(SSM_E_FAIL);

                m_mapResourcePresenceHandles.erase(((OICResourceHandler *)pMessage[1])->m_SSMResource.ip);
            }

            delete m_mapResourceHandler[((OICResourceHandler *)pMessage[1])->m_SSMResource.name];
            m_mapResourceHandler.erase(((OICResourceHandler *) pMessage[1])->m_SSMResource.name);
            break;
    }

CLEANUP:
    ;
}

void CResourceFinder::onTerminate(void *pArg)
{
    std::shared_ptr< OC::OCResource > *pResource = NULL;
    intptr_t *pMessage = (intptr_t *)pArg;

    switch (pMessage[0])
    {
        case RESOURCE_DISCOVER_REQUESTPROFILE:
            pResource = (std::shared_ptr< OC::OCResource > *) pMessage[1];
            delete pResource;
            break;

        case RESOURCE_DISCOVER_INSTALL_RESOURCE:
            break;

        case RESOURCE_DISCOVER_UNINSTALL_RESOURCE:
            break;
    }

    delete[] pMessage;
}
