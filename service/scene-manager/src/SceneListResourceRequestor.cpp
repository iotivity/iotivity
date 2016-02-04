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

#include "SceneListResourceRequestor.h"
#include "RemoteSceneUtils.h"

namespace OIC
{
    namespace Service
    {

        SceneListResourceRequestor::SceneListResourceRequestor
        (RCSRemoteResourceObject::Ptr pSceneList)
            : m_SceneListResourcePtr{ pSceneList }
        {
            SCENE_CLIENT_ASSERT_NOT_NULL(pSceneList);
        }

        SceneListResourceRequestor::~SceneListResourceRequestor()
        {

        }

        void SceneListResourceRequestor::requestSceneCollectionCreation
        (const std::string &name, InternalCreateSceneCollectionCallback internalCB)
        {
            RCSResourceAttributes attrs;
            attrs[SCENE_KEY_NAME] = name;

            RCSRemoteResourceObject::SetCallback setRequestCB
                = std::bind(&SceneListResourceRequestor::onSceneCollectionCreated,
                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                            name, std::move(internalCB),
                            SceneListResourceRequestor::wPtr(shared_from_this()));

            RCSQueryParams queryParams;
            queryParams.setResourceInterface(SCENE_CLIENT_CREATE_REQ_IF);

            m_SceneListResourcePtr->set(queryParams, std::move(attrs), std::move(setRequestCB));
        }

        void SceneListResourceRequestor::requestSetName
        (const std::string &, InternalSetNameCallback)
        {

        }

        void SceneListResourceRequestor::onSceneCollectionCreated
        (const HeaderOpts &headOpts, const RCSRepresentation &rep, int eCode,
         const std::string &name, const InternalCreateSceneCollectionCallback &cb,
         SceneListResourceRequestor::wPtr ptr)
        {
            SceneListResourceRequestor::Ptr listPtr = ptr.lock();

            if (listPtr)
                listPtr->onSceneCollectionCreated_impl(headOpts, std::move(rep), eCode,
                                                       name, std::move(cb));
        }

        void SceneListResourceRequestor::onSceneCollectionCreated_impl
        (const HeaderOpts &, const RCSRepresentation &rep, int eCode,
         const std::string &name, const InternalCreateSceneCollectionCallback &internalCB)
        {
            // TODO: error code
            int result = SCENE_CLIENT_BADREQUEST;
            std::string link, id;

            if (eCode == SCENE_RESPONSE_SUCCESS)
            {
                try
                {
                    RCSResourceAttributes attrs = rep.getAttributes();

                    if (attrs.at(SCENE_KEY_NAME).get< std::string >().compare(name) == 0)
                    {
                        link = attrs.at(SCENE_KEY_PAYLOAD_LINK).get< std::string >();
                        id = attrs.at(SCENE_KEY_ID).get< std::string >();
                        result = SCENE_RESPONSE_SUCCESS;
                    }
                }
                catch (const std::exception &e)
                {
                    SCENE_CLIENT_PRINT_LOG(e.what());
                    result = SCENE_SERVER_INTERNALSERVERERROR;
                }
            }

            internalCB(link, id, name, result);
        }

    }
}