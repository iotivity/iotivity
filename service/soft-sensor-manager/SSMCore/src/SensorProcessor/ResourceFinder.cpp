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

    SSM_CLEANUP_ASSERT(CreateGlobalInstance(OID_ITasker, (IBase **)&m_pTasker));
    SSM_CLEANUP_ASSERT(CreateGlobalInstance(OID_IResourceConnectivity,
                                            (IBase **)&m_pResourceConnectivity));

    m_pResourceFinderEvent = NULL;
    m_pPlatform = (OC::OCPlatform *)m_pResourceConnectivity->getPlatform();

CLEANUP:
    return res;
}

void CResourceFinder::finalRelease()
{
}

SSMRESULT CResourceFinder::registerResourceFinderEvent(IN IResourceFinderEvent *pEvent)
{
    m_pResourceFinderEvent = pEvent;
    return SSM_S_OK;
}

void CResourceFinder::onResourceFound(std::shared_ptr<OC::OCResource> resource)
{
    if (resource)
    {
        int     *pMessage = new int[2];
        pMessage[0] = RESOURCE_DISCOVER_REQUESTPROFILE;
        pMessage[1] = (int)new std::shared_ptr<OC::OCResource>(resource);

        m_pTasker->addTask(this, pMessage);
    }
}

SSMRESULT CResourceFinder::startResourceFinder()
{
    //m_pPlatform->findResource("", "oc/core/service/SoftSensorManager/SoftSensor",
    m_pPlatform->findResource("", "coap://224.0.1.187/oc/core?rt=SoftSensorManager.Sensor",
                              std::bind(&CResourceFinder::onResourceFound, this, std::placeholders::_1));

    return SSM_S_OK;
}

SSMRESULT CResourceFinder::startObserveResource(IN ISSMResource *pSensor, IN IEvent *pEvent)
{
    return m_mapResourceHandler[pSensor->name]->startObserve(pEvent);
}

SSMRESULT CResourceFinder::stopObserveResource(IN ISSMResource *pSensor)
{
    return m_mapResourceHandler[pSensor->name]->stopObserve();
}

void CResourceFinder::onExecute(IN void *pArg)
{
    SSMRESULT res = SSM_E_FAIL;
    OC::QueryParamsMap  queryParams;
    OICResourceHandler  *pResourceHandler = NULL;
    int                 *pMessage = (int *)pArg;
    std::shared_ptr<OC::OCResource> *pResource = NULL;

    switch (pMessage[0])
    {
        case RESOURCE_DISCOVER_REQUESTPROFILE:
            pResource = (std::shared_ptr<OC::OCResource> *)pMessage[1];
            pResourceHandler = new OICResourceHandler();
            SSM_CLEANUP_ASSERT(pResourceHandler->initHandler(*pResource, this));
            m_mapResourceHandler[pResource->get()->host() + pResource->get()->uri()] = pResourceHandler;
            pResource->get()->get(queryParams, std::bind(&OICResourceHandler::onGetResourceProfile,
                                  pResourceHandler, std::placeholders::_1, std::placeholders::_2));
            break;

        case RESOURCE_DISCOVER_SETUP_RESOURCE:
            m_pResourceFinderEvent->onResourceFound((ISSMResource *)pMessage[1]);
            break;
    }

CLEANUP:
    ;
}

void CResourceFinder::onTerminate(IN void *pArg)
{
    std::shared_ptr<OC::OCResource>     *pResource = NULL;
    int                 *pMessage = (int *)pArg;

    switch (pMessage[0])
    {
        case RESOURCE_DISCOVER_REQUESTPROFILE:
            pResource = (std::shared_ptr<OC::OCResource> *)pMessage[1];
            delete pResource;
            break;

        case RESOURCE_DISCOVER_SETUP_RESOURCE:
            break;
    }

    delete[] pMessage;
}
