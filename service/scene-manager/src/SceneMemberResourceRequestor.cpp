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

#include "SceneMemberResourceRequestor.h"
#include "RemoteSceneUtils.h"

namespace OIC
{
    namespace Service
    {

        SceneMemberResourceRequestor::SceneMemberResourceRequestor
        (RCSRemoteResourceObject::Ptr pSceneMember, const std::string &id)
            : m_id{ id }, m_SceneMemberResourcePtr{ pSceneMember }
        {
            SCENE_CLIENT_ASSERT_NOT_NULL(pSceneMember);
        }

        SceneMemberResourceRequestor::~SceneMemberResourceRequestor()
        {

        }

        void SceneMemberResourceRequestor::requestSceneActionCreation
        (const std::string &sceneName, RCSResourceAttributes &attr,
         InternalAddSceneActionCallback internalCB)
        {
            RCSResourceAttributes attributesToSet;
            std::vector< RCSResourceAttributes > vecSceneMappings;

            for (const auto &itr : attr)
            {
                RCSResourceAttributes sceneMappingAttrs;
                sceneMappingAttrs[SCENE_KEY_SCENE] = sceneName;
                sceneMappingAttrs[SCENE_KEY_MEMBERPROPERTY] = itr.key();
                sceneMappingAttrs[SCENE_KEY_MEMBERVALUE] = itr.value();

                vecSceneMappings.push_back(sceneMappingAttrs);
            }

            attributesToSet[SCENE_KEY_SCENEMAPPINGS] = vecSceneMappings;

            RCSRemoteResourceObject::RemoteAttributesSetCallback setRequestCB
                = std::bind(&SceneMemberResourceRequestor::onSceneActionCreated,
                            std::placeholders::_1, std::placeholders::_2,
                            sceneName, std::move(attr), std::move(internalCB),
                            SceneMemberResourceRequestor::wPtr(shared_from_this()));

            m_SceneMemberResourcePtr->setRemoteAttributes
            (std::move(attributesToSet), std::move(setRequestCB));
        }

        RCSRemoteResourceObject::Ptr SceneMemberResourceRequestor::getRemoteResourceObject()
        {
            return m_SceneMemberResourcePtr;
        }

        void SceneMemberResourceRequestor::onSceneActionCreated
        (const RCSResourceAttributes &attrs, int eCode, const std::string &sceneName,
         const RCSResourceAttributes &requestedAttrs, const InternalAddSceneActionCallback &cb,
         SceneMemberResourceRequestor::wPtr ptr)
        {
            SceneMemberResourceRequestor::Ptr memberPtr = ptr.lock();

            if (memberPtr)
                memberPtr->onSceneActionCreated_impl(
                    std::move(attrs), eCode, sceneName, std::move(requestedAttrs), std::move(cb));
        }

        void SceneMemberResourceRequestor::onSceneActionCreated_impl
        (const RCSResourceAttributes &attrs, int eCode, const std::string &sceneName,
         const RCSResourceAttributes &requestedAttrs,
         const InternalAddSceneActionCallback &internalCB)
        {
            // TODO: error code
            int result = SCENE_CLIENT_BADREQUEST;

            if (eCode == SCENE_RESPONSE_SUCCESS)
            {
                try
                {
                    auto mappings
                        = attrs.at(SCENE_KEY_SCENEMAPPINGS).get
                          < std::vector< RCSResourceAttributes > >();

                    // check if the SCENE_MAPPINGS contains requested scene action
                    int uncreatedActionNum = requestedAttrs.size();
                    for (const auto &itr : mappings)
                    {
                        if (itr.at(SCENE_KEY_SCENE).get< std::string >().compare(sceneName) == 0)
                        {
                            std::string key
                                = itr.at(SCENE_KEY_MEMBERPROPERTY).get< std::string >();

                            if (requestedAttrs.contains(key)
                                && requestedAttrs.at(key) == itr.at(SCENE_KEY_MEMBERVALUE))
                            {
                                uncreatedActionNum--;
                            }
                        }

                        if (uncreatedActionNum == 0)
                        {
                            result = SCENE_RESPONSE_SUCCESS;
                            break;
                        }
                    }
                }
                catch (const std::exception &e)
                {
                    SCENE_CLIENT_ASSERT_NOT_NULL(e.what());
                    result = SCENE_SERVER_INTERNALSERVERERROR;
                }
            }

            internalCB(result);
        }

    }
}