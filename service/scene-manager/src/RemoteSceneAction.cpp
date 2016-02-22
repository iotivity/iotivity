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

#include "RemoteSceneAction.h"

#include "SceneCommons.h"
#include "RemoteSceneUtils.h"
#include "SceneMemberResourceRequestor.h"

namespace OIC
{
    namespace Service
    {

        RemoteSceneAction::RemoteSceneAction
        (std::shared_ptr< SceneMemberResourceRequestor > pRequestor,
         const std::string &sceneName, const RCSResourceAttributes &attrs)
            : m_sceneName{ sceneName }, m_attributes{ attrs }, m_requestorPtr{ pRequestor }
        {
            // TODO: check pRequestor not null
        }

        RemoteSceneAction::RemoteSceneAction
        (std::shared_ptr< SceneMemberResourceRequestor > pRequestor,
         const std::string &sceneName,
         const std::string &key, const RCSResourceAttributes::Value &value)
            : m_sceneName{ sceneName }, m_requestorPtr{ pRequestor }
        {
            // TODO: check pRequestor not null
            m_attributes[key] = value;
        }

        void RemoteSceneAction::update(const RCSResourceAttributes &attr,
                                       UpdateCallback clientCB)
        {
            SceneMemberResourceRequestor::InternalAddSceneActionCallback internalCB
                = std::bind(&RemoteSceneAction::onUpdated, this,
                            std::placeholders::_1, attr, std::move(clientCB));

            m_requestorPtr->requestSceneActionCreation(m_sceneName,
                    attr, internalCB);
        }

        void RemoteSceneAction::update(const std::string &key,
                                       const RCSResourceAttributes::Value &value,
                                       UpdateCallback clientCB)
        {
            RCSResourceAttributes attr;
            attr[key] = RCSResourceAttributes::Value(value);

            update(attr, std::move(clientCB));
        }

        RCSResourceAttributes RemoteSceneAction::getAction() const
        {
            return m_attributes;
        }

        RCSRemoteResourceObject::Ptr RemoteSceneAction::getRemoteResourceObject() const
        {
            return m_requestorPtr->getRemoteResourceObject();
        }

        void RemoteSceneAction::onUpdated(int eCode, const RCSResourceAttributes &attr,
                                          const UpdateCallback &clientCB)
        {
            if (eCode == SCENE_RESPONSE_SUCCESS)
            {
                m_attributes = attr;
            }

            clientCB(eCode);
        }

    }
}