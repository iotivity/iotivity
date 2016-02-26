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
#include "SceneCollectionResource.h"
#include <algorithm>

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


            auto sceneMemberResObjs = m_sceneCollectionResourceObj->getSceneMembers();
            bool isRegistered = false;

            SceneMemberResource::Ptr foundMember;
            std::function<void(const RCSRemoteResourceObject::Ptr&,
                    const SceneMemberResource::Ptr&)> buildActionParam = [&](
                            const RCSRemoteResourceObject::Ptr & RCSRemoteResourceObjectPtr,
                            const SceneMemberResource::Ptr & member)
                            {
                                if(RCSRemoteResourceObjectPtr == member->getRemoteResourceObject())
                                {
                                    foundMember = member;
                                    for(const auto &it: member->getMappingInfo())
                                    {
                                        isRegistered = (it.sceneName == m_name);
                                    }
                                }
                            };


            std::for_each(sceneMemberResObjs.begin(), sceneMemberResObjs.end(),
                    [&buildActionParam, &RCSRemoteResourceObjectPtr](
                            const SceneMemberResource::Ptr& member)
                        {
                            buildActionParam(RCSRemoteResourceObjectPtr, member);
                        });

            if(isRegistered)
            {
                throw InvalidAddMemberRequestException(
                        "It is already registered member. Please set Execution Parameter!");
            }

            if(foundMember)
            {
                return SceneAction::Ptr(new SceneAction(foundMember, m_name, attr));
            }

            SceneMemberResource::Ptr sceneMemberResObj;
            sceneMemberResObj = SceneMemberResource::createSceneMemberResource(
                    RCSRemoteResourceObjectPtr);
            try
            {
                m_sceneCollectionResourceObj->addSceneMember(sceneMemberResObj);
            }
            catch(std::exception e)
            {
                throw InvalidAddMemberRequestException(
                        "It is already registered member. Please set Execution Parameter!");
            }

            return SceneAction::Ptr(new SceneAction(sceneMemberResObj, m_name, attr));

        }

        SceneAction::Ptr Scene::getSceneAction(
                const RCSRemoteResourceObject::Ptr& RCSRemoteResourceObjectPtr) const
        {
            auto sceneMemberResObjs = m_sceneCollectionResourceObj->getSceneMembers();

            RCSResourceAttributes actionParam;
            SceneMemberResource::Ptr foundMember;

            std::function<void(const RCSRemoteResourceObject::Ptr&,
                    const SceneMemberResource::Ptr&)> buildActionParam = [&](
                            const RCSRemoteResourceObject::Ptr & RCSRemoteResourceObjectPtr,
                            const SceneMemberResource::Ptr & member)
                            {
                                if(RCSRemoteResourceObjectPtr == member->getRemoteResourceObject())
                                {
                                    foundMember = member;
                                for(const auto &it: member->getMappingInfo())
                                {
                                    if(it.sceneName == m_name)
                                    {
                                        actionParam[it.key] = it.value;
                                    }
                                }
                            }
                            };

            std::for_each (sceneMemberResObjs.begin(), sceneMemberResObjs.end(),
                    [&RCSRemoteResourceObjectPtr, &buildActionParam](
                            const SceneMemberResource::Ptr& member)
                    {
                        buildActionParam(RCSRemoteResourceObjectPtr, member);
                    });

            if(actionParam.empty())
            {
                throw RCSInvalidParameterException("Unknown Remote Resource!");
            }

            return SceneAction::Ptr(new SceneAction(foundMember, m_name, actionParam));
        }

        std::vector<SceneAction::Ptr> Scene::getSceneActions() const
        {
            std::vector<SceneAction::Ptr> actions;
            auto sceneMemberResObjs = m_sceneCollectionResourceObj->getSceneMembers();

            RCSResourceAttributes actionParam;

            std::function<void(const SceneMemberResource::Ptr&)> buildActionParams =
                    [&](const SceneMemberResource::Ptr & member)
                    {
                        for(const auto &it: member->getMappingInfo())
                        {
                            if(it.sceneName == m_name)
                            {
                                actionParam[it.key] = it.value;
                            }
                        }
                        if(!actionParam.empty())
                        {
                            actions.push_back(SceneAction::Ptr(
                                    new SceneAction(member, m_name, actionParam)));
                            actionParam.clear();
                        }
                    };

            std::for_each (sceneMemberResObjs.begin(), sceneMemberResObjs.end(),
                    [&buildActionParams](const SceneMemberResource::Ptr& member)
                    {
                        buildActionParams(member);
                    });

            return actions;
         }

        std::string Scene::getName() const
        {
            return m_name;
        }

        void Scene::removeSceneAction(const SceneAction::Ptr& /*sceneActionPtr*/)
        {
//            TODO : need to implement
        }

        void Scene::removeSceneAction(
                const RCSRemoteResourceObject::Ptr& /*RCSRemoteResourceObjectPtr*/)
        {
//            TODO : : need to implement
        }

        void Scene::execute(ExecuteCallback cb)
        {
            if(cb == nullptr)
            {
                throw RCSInvalidParameterException("Callback is empty!");
            }

            m_sceneCollectionResourceObj->execute(m_name, cb);
        }
    } /* namespace Service */
} /* namespace OIC */
