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

#include "SceneAction.h"
#include "SceneMemberResourceObject.h"

namespace OIC
{
    namespace Service
    {
        SceneAction::SceneAction(const SceneMemberResourceObject::Ptr SceneMemberResource,
                const std::string& sceneName, const RCSResourceAttributes& attr) :
                m_pRemoteResourceObject(SceneMemberResource->getRemoteResourceObject()),
                m_sceneName(sceneName),
                m_sceneMemberResourceObj(SceneMemberResource)
        {
            for (const auto& it : attr)
            {
                m_sceneMemberResourceObj->addMappingInfo(
                        SceneMemberResourceObject::MappingInfo(m_sceneName, it.key(), it.value()));
            }
        }

        SceneAction::SceneAction(const SceneMemberResourceObject::Ptr SceneMemberResource,
                const std::string& sceneName, const std::string& key,
                const RCSResourceAttributes::Value& value) :
                m_pRemoteResourceObject(SceneMemberResource->getRemoteResourceObject()),
                m_sceneName(sceneName), m_sceneMemberResourceObj(SceneMemberResource)
        {
            m_sceneMemberResourceObj->addMappingInfo(
                                SceneMemberResourceObject::MappingInfo(m_sceneName, key, value));
        }

        void SceneAction::update(const std::string& key,
                RCSResourceAttributes::Value value)
        {
            m_attr[key] = value;
            update(m_attr);
        }

        void SceneAction::update(const RCSResourceAttributes& attr)
        {
            for(const auto& it : attr)
            {
                m_sceneMemberResourceObj->addMappingInfo(
                        SceneMemberResourceObject::MappingInfo(m_sceneName, it.key(), it.value()));
            }
        }

        const RCSResourceAttributes SceneAction::getAction()
        {
            m_attr.clear();
            auto mappingInfo = m_sceneMemberResourceObj->getMappingInfo();
            for(const auto& it : mappingInfo)
            {
                if(it.sceneName == m_sceneName)
                {
                    m_attr[it.key] = RCSResourceAttributes::Value(it.value);
                }
            }
            return m_attr;
        }

        RCSRemoteResourceObject::Ptr SceneAction::getRemoteResourceObject() const
        {
            return m_pRemoteResourceObject;
        }

    } /* namespace Service */
} /* namespace OIC */

