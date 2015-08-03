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
#ifndef _ResourceFinder_H_
#define _ResourceFinder_H_

#include "SSMInterface/SSMCore.h"
#include "Common/PlatformLayer.h"
#include "Common/InternalInterface.h"
#include "OCPlatform.h"
#include "OCApi.h"

class CResourceFinder: public CObjectRoot< CObjectMultiThreadModel >,
    public IResourceFinder,
    public IThreadClient
{
    public:
        SSMRESULT queryInterface(const OID &objectID, IBase **ppObject)
        {
            if (ppObject == NULL)
                return SSM_E_POINTER;

            if (IsEqualOID(objectID, OID_IResourceFinder))
            {
                IBase *pBase = (IResourceFinder *) this;
                pBase->addRef();
                *ppObject = pBase;
                return SSM_S_OK;
            }

            return SSM_E_NOINTERFACE;
        }

        SSMRESULT finalConstruct();
        void finalRelease();

        SSMRESULT registerResourceFinderEvent(IResourceFinderEvent *pEvent);
        void onResourceFound(std::shared_ptr< OC::OCResource > resource);
        void presenceHandler(OCStackResult result, const unsigned int nonce,
                             const std::string &hostAddress);
        SSMRESULT startResourceFinder();
        SSMRESULT stopResourceFinder();

        SSMRESULT startObserveResource(ISSMResource *pSensor, IEvent *pEvent);
        SSMRESULT stopObserveResource(ISSMResource *pSensor);

        void onExecute(void *pArg);
        void onTerminate(void *pArg);

    private:
        class OICResourceHandler
        {
            public:
                ISSMResource m_SSMResource;

                OICResourceHandler()
                {
                    m_pEvent = NULL;
                    m_pResourceFinderClient = NULL;
                }

                SSMRESULT initHandler(std::shared_ptr< OC::OCResource > resource,
                                      IThreadClient *pThreadClient)
                {
                    SSMRESULT res = SSM_E_FAIL;

                    SSM_CLEANUP_ASSERT(CreateGlobalInstance(OID_ITasker, (IBase **)&m_pTasker));
                    m_pResource = resource;
                    m_pResourceFinderClient = pThreadClient;

CLEANUP: return res;
                }

                SSMRESULT startObserve(IEvent *pEvent)
                {
                    OC::QueryParamsMap queryParams;

                    m_pEvent = pEvent;

                    m_pResource.get()->observe(OC::ObserveType::Observe, queryParams,
                                               std::bind(&OICResourceHandler::onResourceDataReceived, this,
                                                         std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                                         std::placeholders::_4));

                    return SSM_S_OK;
                }

                SSMRESULT stopObserve()
                {
                    m_pResource.get()->cancelObserve();

                    return SSM_S_OK;
                }

                void onResourceDataReceived(const OC::HeaderOptions headerOptions,
                                            const OC::OCRepresentation &representation, const int &eCode,
                                            const int &sequenceNumber)
                {
                    if (eCode == 0)
                    {
                        std::vector< ContextData > lstCtxData;
                        ContextData ctxData;
                        std::map< std::string, std::string > outputProperty;

                        //Bind data
                        ctxData.rootName = m_pResource->uri().substr(1);

                        //TODO: Temporally used for json parsing limitation
                        ctxData.outputPropertyCount = representation.numberOfAttributes() / 3;

                        if (ctxData.outputPropertyCount > 0)
                        {
                            for (int i = 0; i < ctxData.outputPropertyCount; i++)
                            {
                                outputProperty["name"] = representation.getValue<std::string>(toString(i * 3));
                                outputProperty["type"] = representation.getValue<std::string>(toString(i * 3 + 1));
                                outputProperty["value"] = representation.getValue<std::string>(toString(i * 3 + 2));
                                ctxData.outputProperty.push_back(outputProperty);
                            }

                            /////////////////////////////////////////////////////
                            lstCtxData.push_back(ctxData);
                            m_pEvent->onEvent(m_pResource->uri().substr(1), SSM_REPEAT, lstCtxData);
                        }
                        else
                        {
                            ; //Payload is empty!!
                        }
                    }
                }

                void onGetResourceProfile(const OC::HeaderOptions &headerOptions,
                                          const OC::OCRepresentation &representation, const int eCode)
                {
                    //unpack attributeMap

                    //Create SSMResource using OCResource attributeMap
                    std::map< std::string, std::string > outputProperty;
                    m_SSMResource.location = SENSOR_LOCATION_REMOTE;
                    m_SSMResource.name = m_pResource->host() + m_pResource->uri();
                    m_SSMResource.type = m_pResource->uri().substr(1);
                    m_SSMResource.ip = m_pResource->host();
                    m_SSMResource.connectivityType = m_pResource->connectivityType();

                    //bind default properties
                    outputProperty["name"] = "lifetime";
                    outputProperty["type"] = "int";
                    outputProperty["value"] = "0";
                    m_SSMResource.outputProperty.push_back(outputProperty);

                    //bind default primitive sensor property, value to resource class for schema creating

                    //TODO: Temporally used for json parsing limitation
                    for (int i = 0; i < representation.numberOfAttributes() / 3; i++)
                    {
                        outputProperty["name"] = representation.getValue<std::string>(toString(i * 3));
                        outputProperty["type"] = representation.getValue<std::string>(toString(i * 3 + 1));
                        outputProperty["value"] = representation.getValue<std::string>(toString(i * 3 + 2));
                        m_SSMResource.outputProperty.push_back(outputProperty);
                    }
                    /////////////////////////////////////////////////////
                    //
                    intptr_t      *pMessage = new intptr_t [2];
                    pMessage[0] = RESOURCE_DISCOVER_INSTALL_RESOURCE;
                    pMessage[1] = reinterpret_cast<intptr_t>(&m_SSMResource);

                    m_pTasker->addTask(m_pResourceFinderClient, (void *) pMessage);
                }

            private:
                CObjectPtr< ITasker > m_pTasker;
                std::shared_ptr< OC::OCResource > m_pResource;
                IThreadClient *m_pResourceFinderClient;
                IEvent *m_pEvent;

                std::string toString(int t)
                {
                    std::ostringstream os;
                    os << t;
                    return os.str();
                }
        };

        enum RESOURCE_DISCOVER_STATE
        {
            RESOURCE_DISCOVER_REQUESTPROFILE,
            RESOURCE_DISCOVER_INSTALL_RESOURCE,
            RESOURCE_DISCOVER_UNINSTALL_RESOURCE
        };

        IResourceFinderEvent *m_pResourceFinderEvent;
        CObjectPtr< ITasker > m_pTasker;
        OC::OCPlatform::OCPresenceHandle m_multicastPresenceHandle;
        std::map< std::string, OICResourceHandler * > m_mapResourceHandler;
        std::map< std::string, std::vector<std::string> >
        m_mapResources;    // <hostaddress, std::vector<resources> >
        std::map< std::string, OC::OCPlatform::OCPresenceHandle >
        m_mapResourcePresenceHandles;     // <hostaddress, presence handler>
};

#endif
