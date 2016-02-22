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

#include "SceneCollectionResourceRequestor.h"
#include "RemoteSceneUtils.h"
#include "OCPlatform.h"

namespace OIC
{
    namespace Service
    {

        SceneCollectionResourceRequestor::SceneCollectionResourceRequestor
        (RCSRemoteResourceObject::Ptr pSceneCollection)
            : m_SceneCollectionResourcePtr{ pSceneCollection }
        {
            SCENE_CLIENT_ASSERT_NOT_NULL(pSceneCollection);
        }

        SceneCollectionResourceRequestor::~SceneCollectionResourceRequestor()
        {

        }

        void SceneCollectionResourceRequestor::requestSceneCreation
        (const std::string &name, InternalSceneRequestCallback createSceneCB)
        {
            RCSResourceAttributes attributesToSet;
            std::vector< std::string > vecScenes{ name };

            attributesToSet[SCENE_KEY_SCENEVALUES] = vecScenes;

            RCSRemoteResourceObject::RemoteAttributesSetCallback setRequestCB
                = std::bind(&SceneCollectionResourceRequestor::onSetResponseForScene,
                            std::placeholders::_1, std::placeholders::_2,
                            name, std::move(createSceneCB), ADD_SCENE,
                            SceneCollectionResourceRequestor::wPtr(shared_from_this()));

            m_SceneCollectionResourcePtr->setRemoteAttributes(
                std::move(attributesToSet), std::move(setRequestCB));
        }

        void SceneCollectionResourceRequestor::requestSceneRemoval
        (const std::string &/* name */, InternalSceneRequestCallback /* removeSceneCB */)
        {

        }

        void SceneCollectionResourceRequestor::requestSceneExecution
        (const std::string &name, InternalSceneRequestCallback executeSceneCB)
        {
            RCSResourceAttributes attributesToSet;
            attributesToSet[SCENE_KEY_LAST_SCENE] = name;

            RCSRemoteResourceObject::RemoteAttributesSetCallback setRequestCB
                = std::bind(&SceneCollectionResourceRequestor::onSetResponseForScene,
                            std::placeholders::_1, std::placeholders::_2,
                            name, std::move(executeSceneCB), EXECUTE_SCENE,
                            SceneCollectionResourceRequestor::wPtr(shared_from_this()));

            m_SceneCollectionResourcePtr->setRemoteAttributes
            (std::move(attributesToSet), std::move(setRequestCB));
        }

        void SceneCollectionResourceRequestor::requestAddSceneMember
        (RCSRemoteResourceObject::Ptr pMember, const std::string &sceneName,
         const RCSResourceAttributes &attr, InternalAddMemberCallback addMemberCB)
        {
            SCENE_CLIENT_ASSERT_NOT_NULL(pMember);

            RCSResourceAttributes attributesToSet, linkAttrs;

            linkAttrs[SCENE_KEY_HREF] = pMember->getAddress() + pMember->getUri();
            linkAttrs[SCENE_KEY_IF] = pMember->getInterfaces();
            linkAttrs[SCENE_KEY_RT] = pMember->getTypes();

            attributesToSet[SCENE_KEY_PAYLOAD_LINK] = linkAttrs;

            if (!attr.empty())
            {
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
            }

            RCSRemoteResourceObject::SetCallback setRequestCB
                = std::bind(&SceneCollectionResourceRequestor::onSceneMemberAdded,
                            std::placeholders::_1, std::placeholders::_2,
                            std::placeholders::_3,
                            pMember->getAddress() + pMember->getUri(),
                            std::move(addMemberCB),
                            SceneCollectionResourceRequestor::wPtr(shared_from_this()));

            RCSQueryParams queryParams;
            queryParams.setResourceInterface(SCENE_CLIENT_CREATE_REQ_IF);

            m_SceneCollectionResourcePtr->set(queryParams, std::move(attributesToSet),
                                              std::move(setRequestCB));
        }

        void SceneCollectionResourceRequestor::requestSetName
        (const std::string &, InternalSetNameCallback)
        {

        }

        void SceneCollectionResourceRequestor::requestGet(
            const std::string &ifType, RCSRemoteResourceObject::GetCallback cb)
        {
            RCSQueryParams params;
            params.setResourceInterface(ifType);

            m_SceneCollectionResourcePtr->get(params, cb);
        }

        RCSRemoteResourceObject::Ptr SceneCollectionResourceRequestor::getRemoteResourceObject()
        {
            return m_SceneCollectionResourcePtr;
        }

        SceneMemberResourceRequestor::Ptr
        SceneCollectionResourceRequestor::createSceneMemberResourceRequestor(
            const std::string &memLink, const std::string &id, const std::string &targetlink)
        {
            SceneMemberResourceRequestor::Ptr pMemRequestor = nullptr;

            std::vector< std::string > vecRT{ SCENE_MEMBER_RT };
            std::vector< std::string > vecIF{ SCENE_CLIENT_REQ_IF };

            RCSRemoteResourceObject::Ptr pResource
                = createRCSResourceObject(memLink, SCENE_CONNECTIVITY, vecRT, vecIF);

            if (pResource)
            {
                pMemRequestor =
                    std::make_shared< SceneMemberResourceRequestor >(pResource, id);

                m_mapMemberRequestors[targetlink] = pMemRequestor;
            }

            return pMemRequestor;
        }

        SceneMemberResourceRequestor::Ptr
        SceneCollectionResourceRequestor::getSceneMemberResourceRequestor
        (const std::string &memLink)
        {
            return m_mapMemberRequestors.find(memLink) != m_mapMemberRequestors.end() ?
                   m_mapMemberRequestors.at(memLink) : nullptr;
        }

        void SceneCollectionResourceRequestor::onSetResponseForScene
        (const RCSResourceAttributes &attrs, int eCode,
         const std::string &name, const InternalSceneRequestCallback &cb,
         REQUEST_TYPE reqType, SceneCollectionResourceRequestor::wPtr ptr)
        {
            SceneCollectionResourceRequestor::Ptr collectionPtr = ptr.lock();

            if (collectionPtr)
                collectionPtr->onSetResponseForScene_impl(
                    std::move(attrs), eCode, name, std::move(cb), reqType);
        }

        void SceneCollectionResourceRequestor::onSetResponseForScene_impl
        (const RCSResourceAttributes &attrs, int eCode, const std::string &name,
         const InternalSceneRequestCallback &internalCB, REQUEST_TYPE reqType)
        {
            // TODO: error code
            int resultCode = SCENE_CLIENT_BADREQUEST;

            if (eCode == OC_STACK_OK)
            {
                try
                {
                    switch (reqType)
                    {
                        case ADD_SCENE:
                            {
                                auto scenes
                                    = attrs.at(SCENE_KEY_SCENEVALUES).get
                                      < std::vector< std::string > >();

                                if ((std::find(scenes.begin(), scenes.end(), name))
                                    != scenes.end())
                                    resultCode = SCENE_RESPONSE_SUCCESS;
                            }
                            break;

                        case REMOVE_SCENE:
                            break;

                        case EXECUTE_SCENE:
                            {
                                auto lastScene
                                    = attrs.at(SCENE_KEY_LAST_SCENE).get< std::string >();

                                if (lastScene.compare(name) == 0)
                                    resultCode = SCENE_RESPONSE_SUCCESS;
                            }
                            break;
                    }
                }
                catch (const std::exception &e)
                {
                    SCENE_CLIENT_PRINT_LOG(e.what());
                    resultCode = SCENE_SERVER_INTERNALSERVERERROR;
                }
            }

            internalCB(reqType, name, resultCode);
        }

        void SceneCollectionResourceRequestor::onSceneMemberAdded
        (const HeaderOpts &headOpt, const RCSRepresentation &rep, int eCode,
         const std::string &targetLink, const InternalAddMemberCallback &internalCB,
         SceneCollectionResourceRequestor::wPtr ptr)
        {
            SceneCollectionResourceRequestor::Ptr pSceneCollectionRequestor
                = ptr.lock();

            if (pSceneCollectionRequestor)
                pSceneCollectionRequestor->onSceneMemberAdded_impl(
                    headOpt, rep, eCode, targetLink, std::move(internalCB));
        }

        void SceneCollectionResourceRequestor::onSceneMemberAdded_impl
        (const HeaderOpts &, const RCSRepresentation &rep, int eCode,
         const std::string &targetLink, const InternalAddMemberCallback &internalCB)
        {
            // TODO: error code
            int result = SCENE_CLIENT_BADREQUEST;
            SceneMemberResourceRequestor::Ptr pMemRequestor = nullptr;

            if (eCode == OC_STACK_OK)
            {
                try
                {
                    RCSResourceAttributes receivedAttrs = rep.getAttributes();

                    pMemRequestor
                        = createSceneMemberResourceRequestor(
                              receivedAttrs.at(SCENE_KEY_CREATEDLINK).get< std::string >(),
                              receivedAttrs.at(SCENE_KEY_ID).get< std::string >(),
                              targetLink);

                    if (pMemRequestor)
                        result = SCENE_RESPONSE_SUCCESS;
                }
                catch (const std::exception &e)
                {
                    SCENE_CLIENT_PRINT_LOG(e.what());
                    result = SCENE_SERVER_INTERNALSERVERERROR;
                }
            }

            internalCB(pMemRequestor, result);
        }

    }
}