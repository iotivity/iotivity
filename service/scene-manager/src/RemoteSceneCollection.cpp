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

#include "RemoteSceneCollection.h"

#include "SceneCommons.h"
#include "RemoteSceneUtils.h"
#include "SceneCollectionResourceRequestor.h"
#include "OCPlatform.h"

namespace OIC
{
    namespace Service
    {

        RemoteSceneCollection::RemoteSceneCollection
        (std::shared_ptr< SceneCollectionResourceRequestor > pRequestor,
         const std::string &id, const std::string &name)
            : m_id{ id }, m_name{ name }, m_requestorPtr{ pRequestor }
        {
            // TODO: check pRequestor not null
        }

        void RemoteSceneCollection::addNewScene
        (const std::string &name, AddNewSceneCallback clientCB)
        {
            if (m_remoteScenes.find(name) == m_remoteScenes.end())
            {
                SceneCollectionResourceRequestor::InternalSceneRequestCallback internalCB
                    = std::bind(&RemoteSceneCollection::onSceneAddedRemoved, this,
                                std::placeholders::_1, std::placeholders::_2,
                                std::placeholders::_3, std::move(clientCB), nullptr);

                m_requestorPtr->requestSceneCreation(name, internalCB);
            }
            else
            {
                // TODO: throw duplication scene name exception
            }
        }

        void RemoteSceneCollection::removeScene(const std::string &/* name */,
                                                RemoveSceneCallback /* clientCB */)
        {

        }

        std::map< const std::string, RemoteScene::Ptr >
        RemoteSceneCollection::getRemoteScenes() const
        {
            return m_remoteScenes;
        }

        RemoteScene::Ptr RemoteSceneCollection::getRemoteScene(const std::string &sceneName) const
        {
            auto itr = m_remoteScenes.find(sceneName);

            // TODO: throw unadded scene exception
            return itr != m_remoteScenes.end() ? itr->second : nullptr;
        }

        void RemoteSceneCollection::setName(const std::string &name, SetNameCallback clientCB)
        {
            SceneCollectionResourceRequestor::InternalSetNameCallback internalCB
                = std::bind(&RemoteSceneCollection::onNameSet, this,
                            std::placeholders::_1, name, std::move(clientCB));

            m_requestorPtr->requestSetName(name, internalCB);
        }

        std::string RemoteSceneCollection::getName() const
        {
            return m_name;
        }

        std::string RemoteSceneCollection::getId() const
        {
            return m_id;
        }

        void RemoteSceneCollection::initializeRemoteSceneCollection(
            const std::vector< RCSRepresentation > &MemberReps, const std::string &host)
        {
            try
            {
                for (const auto &itr : MemberReps)
                {
                    RCSResourceAttributes attrs = itr.getAttributes();

                    for (const auto &mappingInfo :
                         attrs.at(SCENE_KEY_SCENEMAPPINGS).get
                         <std::vector< RCSResourceAttributes > >())
                    {
                        std::string sceneName
                            = mappingInfo.at(SCENE_KEY_SCENE).get< std::string >();
                        RemoteScene::Ptr pRemoteScene = nullptr;

                        auto remoteScene = m_remoteScenes.find(sceneName);
                        if (remoteScene == m_remoteScenes.end())
                        {
                            pRemoteScene = createRemoteSceneInstance(sceneName);
                        }
                        else
                        {
                            pRemoteScene = m_remoteScenes.at(sceneName);
                        }

                        pRemoteScene->addExistingRemoteSceneAction(
                            host + itr.getUri(),
                            attrs.at(SCENE_KEY_PAYLOAD_LINK).get< RCSResourceAttributes >().
                            at(SCENE_KEY_HREF).get< std::string >(),
                            attrs.at(SCENE_KEY_ID).get< std::string >(),
                            mappingInfo.at(SCENE_KEY_MEMBERPROPERTY).get< std::string >(),
                            RCSResourceAttributes::Value(mappingInfo.at(SCENE_KEY_MEMBERVALUE))
                        );
                    }
                }
            }
            catch (const std::exception &)
            {
                // error
            }
        }

        RemoteScene::Ptr RemoteSceneCollection::createRemoteSceneInstance
        (const std::string &name)
        {
            RemoteScene::Ptr pNewRemoteScene
            {
                new RemoteScene(name, m_requestorPtr)
            };

            m_remoteScenes[name] = pNewRemoteScene;

            return pNewRemoteScene;
        }

        void RemoteSceneCollection::onSceneAddedRemoved(const int &reqType,
                const std::string &name, int eCode,
                const AddNewSceneCallback &addCB, const RemoveSceneCallback &)
        {
            if (eCode == SCENE_RESPONSE_SUCCESS)
            {
                switch (reqType)
                {
                    case SceneCollectionResourceRequestor::REQUEST_TYPE::ADD_SCENE:
                        addCB(createRemoteSceneInstance(name), SCENE_RESPONSE_SUCCESS);
                        break;

                    case SceneCollectionResourceRequestor::REQUEST_TYPE::REMOVE_SCENE:
                        break;
                }
            }
            else
            {
                // error
            }
        }

        void RemoteSceneCollection::onNameSet(int eCode, const std::string &name,
                                              const SetNameCallback &clientCB)
        {
            if (eCode == SCENE_RESPONSE_SUCCESS)
                m_name = name;

            clientCB(eCode);
        }

    }
}