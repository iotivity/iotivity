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

#include "SceneMemberObject.h"

namespace OIC
{
    namespace Service
    {
        namespace
        {
            unsigned int m_numSceneMember = 0;

            const std::string SCENE_MEMBER_RT = "oic.r.scenemember";
        }

        SceneMemberObject::SceneMemberObject(RCSRemoteResourceObject::Ptr it) :
                link{ it->getUri() }
        {
            RemoteObjectPtr = it;

            std::string resourceUri = "/a/sceneMember" + std::to_string(m_numSceneMember++);
            sceneMemberResourcePtr = RCSResourceObject::Builder(resourceUri, SCENE_MEMBER_RT,
                    OC_RSRVD_INTERFACE_DEFAULT).setDiscoverable(true).setObservable(true).build();

            sceneMemberResourcePtr->setAutoNotifyPolicy(RCSResourceObject::AutoNotifyPolicy::NEVER);
            sceneMemberResourcePtr->setSetRequestHandlerPolicy(
                    RCSResourceObject::SetRequestHandlerPolicy::NEVER);

            sceneMemberResourcePtr->setAttribute("link", it->getUri());
        }

        void SceneMemberObject::setName(std::string sceneMemberName)
        {
            name = sceneMemberName;
            sceneMemberResourcePtr->setAttribute("name", sceneMemberName);
        }

        std::string SceneMemberObject::getName()
        {
            return name;
        }

        RCSResourceAttributes SceneMemberObject::getSceneMappingItem(std::string sceneName)
        {
            for (const auto& it : sceneMapping)
            {
                if (it.first == sceneName)
                    return it.second;
            }
        }

        std::map< std::string, RCSResourceAttributes > SceneMemberObject::getSceneMapping()
        {
            return sceneMapping;
        }

        bool SceneMemberObject::hasSceneMappingitem(std::string sceneName)
        {
            if (sceneMapping.find(sceneName) == sceneMapping.end())
                return false;
            return true;
        }

        RCSRemoteResourceObject::Ptr SceneMemberObject::getRemoteResourceObject()
        {
            return RemoteObjectPtr;
        }

    } /* namespace Service */
} /* namespace OIC */

