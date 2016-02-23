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

#include "Scene.h"
#include <algorithm>
#include "SceneCollectionResource.h"

namespace OIC
{
    namespace Service
    {
        Scene::Scene(const std::string& sceneName,
                SceneCollectionResource::Ptr sceneCollectionResource) :
                m_name(sceneName), m_sceneCollectionResourceObj(sceneCollectionResource) {}

        SceneAction::Ptr Scene::addNewSceneAction(
               const RCSRemoteResourceObject::Ptr& RCSRemoteResourceObjectPtr,
               const std::string& key, const RCSResourceAttributes::Value& value)
        {
            RCSResourceAttributes resAttr;
            resAttr[key] = RCSResourceAttributes::Value(value);

            return addNewSceneAction(RCSRemoteResourceObjectPtr, resAttr);
        }

        SceneAction::Ptr Scene::addNewSceneAction(
                const RCSRemoteResourceObject::Ptr& RCSRemoteResourceObjectPtr,
                const RCSResourceAttributes& attr)
        {
            if(RCSRemoteResourceObjectPtr == nullptr)
            {
                throw RCSInvalidParameterException("RCSRemoteResoureObjectPtr value is null");
            }

            SceneMemberResource::Ptr sceneMemberResObj;
            auto members = m_sceneCollectionResourceObj->getSceneMembers();
            auto check = std::find_if(members.begin(), members.end(), [&RCSRemoteResourceObjectPtr] (
                    const SceneMemberResource::Ptr it){
                return it->getRemoteResourceObject() == RCSRemoteResourceObjectPtr;});

            if(check != members.end())
            {
                sceneMemberResObj = *check;
            }

            else
            {
                sceneMemberResObj = SceneMemberResource::createSceneMemberResource(
                        RCSRemoteResourceObjectPtr);
                m_sceneCollectionResourceObj->addSceneMember(sceneMemberResObj);
            }

            SceneAction::Ptr sceneActionPtr(new SceneAction(sceneMemberResObj, m_name, attr));
            return sceneActionPtr;
        }

        std::vector< SceneAction::Ptr > Scene::getSceneAction( // need to confirm return type vector or Ptr
                const RCSRemoteResourceObject::Ptr& RCSRemoteResourceObjectPtr) const
        {
            std::vector<SceneAction::Ptr> actions;
            auto sceneMemberResObjs = m_sceneCollectionResourceObj->getSceneMembers();
            for(const auto& member: sceneMemberResObjs)
            {
                if(member->getRemoteResourceObject() == RCSRemoteResourceObjectPtr)
                {
                    auto mappingInfo = member->getMappingInfo();
                    for(const auto& it: mappingInfo)
                    {
                        if(it.sceneName == m_name)
                        {
                            SceneAction::Ptr sceneActionPtr(new SceneAction(
                                    member, m_name, it.key, it.value));
                            actions.push_back(sceneActionPtr);
                        }
                    }
                }
            }
            return actions;
        }

        std::vector< SceneAction::Ptr > Scene::getSceneActions() const
        {
            std::vector<SceneAction::Ptr> actions;
            auto sceneMemberResObjs = m_sceneCollectionResourceObj->getSceneMembers();
            for(const auto& member: sceneMemberResObjs)
            {
                auto mappingInfo = member->getMappingInfo();
                for(const auto& it : mappingInfo)
                {
                    if(it.sceneName == m_name)
                    {
                        SceneAction::Ptr sceneActionPtr(new SceneAction(
                                member, m_name, it.key, it.value));
//                        SceneAction::WeakPtr sceneActionWeakPtr(sceneActionPtr);
                        actions.push_back(sceneActionPtr);
                    }
                }
            }
            return actions;
        }

        std::string Scene::getName() const
        {
            return m_name;
        }

        void Scene::removeSceneAction(const SceneAction::Ptr& sceneActionPtr)
        {
//            TODO
        }

        void Scene::removeSceneAction(
                const RCSRemoteResourceObject::Ptr& RCSRemoteResourceObjectPtr)
        {
//            TODO
        }

        void Scene::execute(ExecuteCallback cb)
        {
            m_sceneCollectionResourceObj->execute(m_name, cb);
        }
    } /* namespace Service */
} /* namespace OIC */
