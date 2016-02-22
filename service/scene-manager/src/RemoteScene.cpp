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

#include "RemoteScene.h"

#include "SceneCommons.h"
#include "RemoteSceneUtils.h"
#include "SceneCollectionResourceRequestor.h"
#include "SceneMemberResourceRequestor.h"
#include "OCPlatform.h"

namespace OIC
{
    namespace Service
    {

        RemoteScene::RemoteScene
        (const std::string &name, std::shared_ptr< SceneCollectionResourceRequestor > pRequestor)
            : m_name{ name }, m_requestorPtr{ pRequestor }
        {
            // TODO: check pRequestor not null
        }

        void RemoteScene::addNewSceneAction(
            RCSRemoteResourceObject::Ptr, const RCSResourceAttributes &, AddNewSceneActionCallback)
        {

        }

        void RemoteScene::addNewSceneAction
        (RCSRemoteResourceObject::Ptr pTargetResource,
         const std::string &key, const RCSResourceAttributes::Value &value,
         AddNewSceneActionCallback clientCB)
        {
            // TODO: check pTargetResource not null
            // TODO: check key not empty

            RCSResourceAttributes attr;
            attr[key] = RCSResourceAttributes::Value(value);

            std::string targetLink = pTargetResource->getAddress() + pTargetResource->getUri();

            auto sceneActions = m_remoteSceneActions.find(targetLink);

            // if the target is not a scene member
            if (sceneActions == m_remoteSceneActions.end())
            {
                SceneCollectionResourceRequestor::InternalAddMemberCallback internalCB
                    = std::bind(&RemoteScene::onSceneActionAdded, this,
                                std::placeholders::_1, std::placeholders::_2,
                                pTargetResource, attr, std::move(clientCB));

                m_requestorPtr->requestAddSceneMember
                (pTargetResource, m_name, attr, internalCB);
            }
            else
            {
                // check if sceneAction already exists
                for (auto itr : sceneActions->second)
                {
                    if (itr->getAction().contains(key))
                    {
                        // throw exception
                        break;
                    }
                }

                SceneMemberResourceRequestor::Ptr pMemberRequestor
                    = m_requestorPtr->getSceneMemberResourceRequestor(targetLink);

                if (pMemberRequestor)
                {
                    SceneMemberResourceRequestor::InternalAddSceneActionCallback internalCB
                        = std::bind(&RemoteScene::onSceneActionAdded, this,
                                    nullptr,
                                    std::placeholders::_1, pTargetResource,
                                    attr, std::move(clientCB));

                    pMemberRequestor->
                    requestSceneActionCreation(m_name, attr, internalCB);
                }
            }
        }

        void RemoteScene::removeSceneAction(RemoteSceneAction::Ptr/* pRemoteSceneAction */,
                                            RemoveSceneActionCallback /* clientCB */)
        {

        }

        void RemoteScene::removeSceneAction(RCSRemoteResourceObject::Ptr/* pTargetResource*/,
                                            RemoveSceneActionCallback /* clientCB */)
        {

        }

        std::vector< RemoteSceneAction::Ptr > RemoteScene::getRemoteSceneActions() const
        {
            std::vector< RemoteSceneAction::Ptr > vecSceneActions;

            for (auto itrMap : m_remoteSceneActions)
            {
                for (auto itrVec : itrMap.second)
                {
                    vecSceneActions.push_back(itrVec);
                }
            }

            return vecSceneActions;
        }

        std::vector< RemoteSceneAction::Ptr > RemoteScene::getRemoteSceneAction
        (const RCSRemoteResourceObject::Ptr pTargetResource) const
        {
            auto itr = m_remoteSceneActions.find(
                           pTargetResource->getAddress() + pTargetResource->getUri());

            if (itr == m_remoteSceneActions.end())
            {
                ;
                // TODO: throw unadded scene action exception
            }

            return itr->second;
        }

        std::string RemoteScene::getName() const
        {
            return m_name;
        }

        void RemoteScene::execute(RemoteSceneExecuteCallback clientCB)
        {
            SceneCollectionResourceRequestor::InternalSceneRequestCallback internalCB
                = std::bind(&RemoteScene::onSceneExecuted, this,
                            std::placeholders::_1, std::placeholders::_2,
                            std::placeholders::_3, std::move(clientCB));

            m_requestorPtr->requestSceneExecution(m_name, internalCB);
        }

        RemoteSceneAction::Ptr RemoteScene::createRemoteSceneActionInstance
        (const std::string &targetLink, const RCSResourceAttributes &attrs)
        {
            RemoteSceneAction::Ptr pNewSceneAction = nullptr;
            SceneMemberResourceRequestor::Ptr pMemRequestor
                = m_requestorPtr->getSceneMemberResourceRequestor(targetLink);
            if (pMemRequestor)
            {
                pNewSceneAction.reset(new RemoteSceneAction(pMemRequestor, m_name, attrs));
                m_remoteSceneActions[targetLink].push_back(pNewSceneAction);
            }

            return pNewSceneAction;
        }

        void RemoteScene::addExistingRemoteSceneAction
        (const std::string &address, const std::string &targetLink,
         const std::string &id, const std::string &key,  const RCSResourceAttributes::Value &value)
        {
            SceneMemberResourceRequestor::Ptr pfoundMember
                = m_requestorPtr->getSceneMemberResourceRequestor(targetLink);

            if (pfoundMember == nullptr)
                m_requestorPtr->createSceneMemberResourceRequestor(address, id, targetLink);

            RCSResourceAttributes attrs;
            attrs[key] = RCSResourceAttributes::Value(value);

            createRemoteSceneActionInstance(targetLink, attrs);
        }

        void RemoteScene::onSceneActionAdded
        (SceneMemberResourceRequestor::Ptr, int eCode, RCSRemoteResourceObject::Ptr target,
         const RCSResourceAttributes &attrs,
         AddNewSceneActionCallback clientCB)
        {
            int result = SCENE_CLIENT_BADREQUEST;
            RemoteSceneAction::Ptr pNewRemoteSceneAction = nullptr;

            if (eCode == SCENE_RESPONSE_SUCCESS)
            {
                std::string targetLink = target->getAddress() + target->getUri();

                pNewRemoteSceneAction = createRemoteSceneActionInstance(targetLink, attrs);

                if (pNewRemoteSceneAction)
                    result = SCENE_RESPONSE_SUCCESS;
            }
            else
            {
                // error
            }

            clientCB(pNewRemoteSceneAction, result);
        }

        void RemoteScene::onSceneExecuted(const int &, const std::string &sceneName,
                                          const int eCode, RemoteSceneExecuteCallback clientCB)
        {
            clientCB(sceneName, eCode);
        }

    }
}