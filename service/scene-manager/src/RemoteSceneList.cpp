//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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

#include "RemoteSceneList.h"

#include <map>

#include "SceneCommons.h"
#include "RemoteSceneUtils.h"
#include "SceneListResourceRequestor.h"
#include "SceneCollectionResourceRequestor.h"
#include "OCPlatform.h"

namespace OIC
{
    namespace Service
    {

        RemoteSceneList::RemoteSceneList(SceneListResourceRequestor::Ptr pRequestor)
            : m_requestorPtr{ pRequestor }, m_getResponseHandler{}
        {

        }

        void RemoteSceneList::createInstance(RCSRemoteResourceObject::Ptr pSceneListResource,
                                             CreateInstanceCallback clientCB)
        {
            // TODO: check pRequestor not null

            SceneListResourceRequestor::Ptr pRequestor =
                std::make_shared< SceneListResourceRequestor >(pSceneListResource);

            RemoteSceneList::Ptr newSceneList(new RemoteSceneList(pRequestor));

            clientCB(std::move(newSceneList), SCENE_RESPONSE_SUCCESS);
        }

        void RemoteSceneList::addNewSceneCollection(AddNewSceneCollectionCallback clientCB)
        {
            SceneListResourceRequestor::InternalCreateSceneCollectionCallback internalCB
                = std::bind(&RemoteSceneList::onSceneCollectionCreated, this,
                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                            std::placeholders::_4, std::move(clientCB));

            m_requestorPtr->requestSceneCollectionCreation("", internalCB);
        }

        void RemoteSceneList::removeSceneCollection(RemoteSceneCollection::Ptr)
        {

        }

        std::vector< RemoteSceneCollection::Ptr >
        RemoteSceneList::getRemoteSceneCollections() const
        {
            return m_remoteSceneCollections;
        }

        void RemoteSceneList::setName(const std::string &name, SetNameCallback clientCB)
        {
            SceneListResourceRequestor::InternalSetNameCallback internalCB
                = std::bind(&RemoteSceneList::onNameSet, this,
                            std::placeholders::_1, name, std::move(clientCB));

            m_requestorPtr->requestSetName(name, internalCB);
        }

        std::string RemoteSceneList::getName() const
        {
            return m_name;
        }

        RemoteSceneCollection::Ptr RemoteSceneList::createRemoteSceneCollectionInstance
        (const std::string &link, const std::string &id, const std::string &name)
        {
            RemoteSceneCollection::Ptr pNewSceneCollection = nullptr;

            std::vector< std::string > vecRT{ SCENE_COLLECTION_RT };
            std::vector< std::string > vecIF{ SCENE_CLIENT_REQ_IF };

            RCSRemoteResourceObject::Ptr pResource
                = createRCSResourceObject(link, SCENE_CONNECTIVITY, vecRT, vecIF);

            if (pResource)
            {
                SceneCollectionResourceRequestor::Ptr pRequestor
                { new SceneCollectionResourceRequestor(pResource) };

                pNewSceneCollection.reset(
                    new RemoteSceneCollection(pRequestor, id, name));

                m_remoteSceneCollections.push_back(pNewSceneCollection);
            }

            return pNewSceneCollection;
        }

        void RemoteSceneList::setGetResponseHandler
        (const std::string &host, GetResponseHandler::GetCallback cb)
        {
            m_getResponseHandler.reset(new GetResponseHandler(shared_from_this()));

            m_getResponseHandler->startGetResponseHandler(host, std::move(cb));
        }

        void RemoteSceneList::onSceneCollectionCreated
        (const std::string &link, const std::string &id, const std::string &name, int eCode,
         AddNewSceneCollectionCallback clientCB)
        {
            int result = SCENE_CLIENT_BADREQUEST;
            RemoteSceneCollection::Ptr pNewRemoteSceneCollection = nullptr;

            if (eCode == SCENE_RESPONSE_SUCCESS)
            {
                pNewRemoteSceneCollection =
                    createRemoteSceneCollectionInstance(link, id, name);

                // TODO: result type check
                if (pNewRemoteSceneCollection)
                    result = SCENE_RESPONSE_SUCCESS;
            }
            clientCB(pNewRemoteSceneCollection, result);
        }

        void RemoteSceneList::onNameSet(int eCode, const std::string &name,
                                        const SetNameCallback &clientCB)
        {
            if (eCode == SCENE_RESPONSE_SUCCESS)
                m_name = name;

            clientCB(eCode);
        }

        RemoteSceneList::GetResponseHandler::GetResponseHandler
        (std::shared_ptr< RemoteSceneList > ptr)
            : m_errorCode{ SCENE_RESPONSE_SUCCESS }, m_owner{ ptr }
        {

        }

        void RemoteSceneList::GetResponseHandler::startGetResponseHandler
        (const std::string &host, GetCallback cb)
        {
            RCSRemoteResourceObject::GetCallback ListGetCB
                = std::bind(&GetResponseHandler::onGetListAttrs, this,
                            std::placeholders::_1, std::placeholders::_2,
                            std::placeholders::_3,
                            host);

            m_cb = cb;

            std::shared_ptr< RemoteSceneList > pList = m_owner.lock();

            if (pList)
            {
                pList->m_requestorPtr->
                requestGet(SCENE_CLIENT_CREATE_REQ_IF, ListGetCB);
            }
        }

        void RemoteSceneList::GetResponseHandler::onGetListAttrs
        (const HeaderOpts &, const RCSRepresentation &rep,
         int eCode, const std::string &host)
        {
            if (eCode == SCENE_RESPONSE_SUCCESS)
            {
                std::shared_ptr< RemoteSceneList > pList = m_owner.lock();

                if (pList)
                {
                    pList->m_name
                        = rep.getAttributes().at(SCENE_KEY_NAME).get< std::string >();

                    std::vector< RCSRepresentation > childReps
                        = rep.getChildren();

                    if (childReps.size() > 0)
                    {
                        for (auto itr : childReps)
                        {
                            std::vector< std::string > childRTs = itr.getResourceTypes();
                            if (std::find(childRTs.begin(), childRTs.end(), SCENE_COLLECTION_RT)
                                != childRTs.end())
                            {
                                RCSResourceAttributes attr = itr.getAttributes();
                                RemoteSceneCollection::Ptr pNewSceneCollection
                                    = pList->createRemoteSceneCollectionInstance(
                                          host + rep.getUri(),
                                          attr.at(SCENE_KEY_ID).get< std::string >(),
                                          attr.at(SCENE_KEY_NAME).get< std::string >());

                                if (pNewSceneCollection == nullptr)
                                {
                                    // error
                                    continue;
                                }

                                RCSRemoteResourceObject::GetCallback handlerGetCB
                                    = std::bind(&GetResponseHandler::onGetCollectionAttrs,
                                                this, std::placeholders::_1,
                                                std::placeholders::_2, std::placeholders::_3,
                                                pNewSceneCollection, host);

                                pNewSceneCollection->m_requestorPtr->requestGet(
                                    SCENE_CLIENT_REQ_IF, handlerGetCB);
                            }
                        }
                    }
                }
            }
            else
            {
                // error
            }
        }

        void RemoteSceneList::GetResponseHandler::onGetCollectionAttrs
        (const HeaderOpts &, const RCSRepresentation &reps, int eCode,
         RemoteSceneCollection::Ptr pRemoteSceneCollection, const std::string &host)
        {
            m_respondedCollections++;
            if (eCode == SCENE_RESPONSE_SUCCESS)
            {
                pRemoteSceneCollection->
                initializeRemoteSceneCollection(reps.getChildren(), std::string(host));
            }
            else
            {
                // error
                m_errorCode = eCode;
            }

            if (m_respondedCollections == m_numOfCollections)
            {
                m_cb(eCode);
            }
        }

    }
}