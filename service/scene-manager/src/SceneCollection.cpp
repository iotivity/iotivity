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

#include "SceneCollection.h"

#include "uuid/uuid.h"
#include "octypes.h"
#include "ocrandom.h"

namespace OIC
{
    namespace Service
    {
        namespace
        {
            unsigned int numSceneCollection = 0;

            const std::string PREFIX_SCENE_COLLECTION_URI = "/a/sceneCollection/";
            const std::string LAST_SCENE = "lastScene";
            const std::string SCENE_VALUES = "sceneValues";
            const std::string SCENE_COLLECTION_NAME = "n";
            const std::string SCENE_COLLECTION_ID = "id";
            const std::string SCENE_RTS = "rts";
            const std::string SCENE_RTS_VALUE = "oic.r.scenemember";
            const std::string SCENE_PAYLOAD_LINK = "link";

            const std::string SCENE_COLLECTION_RT = "oic.wk.scenecollection";


            std::string OICGenerateUUIDStr()
            {
                uint8_t uuid[UUID_SIZE] = { 0, };
                char uuidStr[UUID_STRING_SIZE] = { 0, };
                if(RAND_UUID_OK == OCGenerateUuid(uuid))
                {
                    if(RAND_UUID_OK == OCConvertUuidToString(uuid, uuidStr))
                    {
                        return std::string(uuidStr);
                    }
                }
                return "";
            }
        }

        SceneCollection::SceneCollection()
        : m_Id(), m_Uri(PREFIX_SCENE_COLLECTION_URI + std::to_string(numSceneCollection++))
        {
            m_Id = OICGenerateUUIDStr();
            if(m_Id == "")
            {
                // TODO handle uuid creation fail.
            }

            m_sceneCollectionResourcePtr = RCSResourceObject::Builder(m_Uri, SCENE_COLLECTION_RT,
                    OC_RSRVD_INTERFACE_DEFAULT).setDiscoverable(true).setObservable(true).build();

            m_sceneCollectionResourcePtr->setAttribute(LAST_SCENE, "");
            m_sceneCollectionResourcePtr->setAttribute(SCENE_VALUES, std::vector<std::string>());
            m_sceneCollectionResourcePtr->setAttribute(SCENE_COLLECTION_NAME, "");
            m_sceneCollectionResourcePtr->setAttribute(SCENE_COLLECTION_ID, m_Id);
            m_sceneCollectionResourcePtr->setAttribute(SCENE_RTS, SCENE_RTS_VALUE);


            m_sceneCollectionResourcePtr->setSetRequestHandler(
                    std::bind(
                            &SceneCollection::setRequestHandler, this,
                            std::placeholders::_1, std::placeholders::_2));
        }

        const std::vector< Scene::Ptr >& SceneCollection::getSceneList()
        {
            return m_SceneList;
        }

        void SceneCollection::setName(const std::string& sceneCollectionName)
        {
            m_sceneCollectionResourcePtr->setAttribute(SCENE_COLLECTION_NAME, sceneCollectionName);
        }

        std::string SceneCollection::getName() const
        {
            return m_sceneCollectionResourcePtr->
                    getAttributeValue(SCENE_COLLECTION_NAME).get<std::string>();
        }

        std::string SceneCollection::getUri() const
        {
            return m_Uri;
        }

        std::string SceneCollection::getId() const
        {
            return m_Id;
        }

        Scene::Ptr SceneCollection::addScene(const std::string& sceneName)
        {
            Scene::Ptr m_scene = std::make_shared < Scene > (sceneName);
            m_SceneList.push_back(m_scene);
            return m_scene;
        }

        bool SceneCollection::removeScene(const Scene::Ptr& it)
        {
            // remove scene
            // remove scene Name list
        }

        SceneMemberObject::Ptr SceneCollection::addSceneMember(
                const RCSRemoteResourceObject::Ptr& it)
        {
            auto newSceneMemberObj = std::make_shared < SceneMemberObject > (it);
            m_SceneMemberObjectList.push_back(newSceneMemberObj);
            return newSceneMemberObj;
        }

        bool SceneCollection::removeSceneMember(SceneMemberObject::Ptr it)
        {
        }

        const std::vector< SceneMemberObject::Ptr >& SceneCollection::getSceneMemberList()
        {
            return m_SceneMemberObjectList;
        }

        RCSSetResponse
        SceneCollection::setRequestHandler(
                const RCSRequest & request, RCSResourceAttributes & attributes)
        {
            if(attributes.contains(SCENE_PAYLOAD_LINK))
            {
                return createSceneMemberRequestHandler(request, attributes);
            }

            if(attributes.contains(SCENE_VALUES))
            {
                return createSceneRequestHandler(request, attributes);
            }

            if(attributes.contains(LAST_SCENE))
            {
                return executeSceneRequestHandler(request, attributes);
            }


            return RCSSetResponse::create(attributes, (int)OC_STACK_ERROR)
            .setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
        }

        RCSSetResponse
        SceneCollection::createSceneMemberRequestHandler(
                const RCSRequest &, RCSResourceAttributes & attributes)
        {
            std::string request_key = attributes.at(SCENE_PAYLOAD_LINK).get<std::string>();
            // TODO create scene member

            attributes.at(SCENE_COLLECTION_ID) = m_Id;

            return RCSSetResponse::create(attributes)
            .setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
        }

        RCSSetResponse
        SceneCollection::createSceneRequestHandler(
                const RCSRequest &, RCSResourceAttributes & attributes)
        {
            std::vector<std::string> request_key
                = attributes.at(SCENE_VALUES).get<std::vector<std::string>>();

            std::vector<std::string> newScene;
            std::vector<std::string>::iterator iter = request_key.begin();
            std::vector<Scene::Ptr>::iterator foundScene;

            struct FindSceneName
            {
                bool operator()(Scene::Ptr scene) const { return scene->getName() == name; }
                std::string name;
            };

            while(iter != request_key.end())
            {
                FindSceneName fScene;
                fScene.name = *iter;
                foundScene = std::find_if(m_SceneList.begin(), m_SceneList.end(), fScene);
                if(foundScene == m_SceneList.end())
                {
                    newScene.push_back(*iter);
                }
                iter++;
            }

            for(unsigned int i = 0; i < newScene.size(); ++i)
            {
                // TODO create scene
                newScene[i]; //Scene Value
            }


            // success
            return RCSSetResponse::create(attributes)
            .setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
        }

        RCSSetResponse
        SceneCollection::executeSceneRequestHandler(
                const RCSRequest &, RCSResourceAttributes & attributes)
        {
            std::string request_key = attributes.at(LAST_SCENE).get<std::string>();
            m_sceneCollectionResourcePtr->setAttribute(LAST_SCENE, request_key);

            // TODO execute scene

            // success
            return RCSSetResponse::create(attributes)
            .setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
        }
    } /* namespace Service */
} /* namespace OIC */

