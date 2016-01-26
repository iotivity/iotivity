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

#include "SceneList.h"

#include "RCSRequest.h"
#include "PrimitiveResource.h"
#include "OCPlatform.h"

namespace OIC
{
    namespace Service
    {
        namespace
        {
            const std::string SCENE_LIST_NAME = "n";
            const std::string SCENE_LIST_ID = "id";
            const std::string SCENE_LIST_LINK = "link";
            const std::string SCENE_LIST_RTS = "rts";
            const std::string SCENE_LIST_RTS_VALUE = "oic.wk.scenecollection";
            const std::string SCENE_LIST_RT_NAME = "oic.wk.scenelist";
            const std::string SCENE_LIST_URI = "/SceneListResURI";
            const std::string SCENE_LIST_DEFAULT_NAME = "list of scene Collections";
            const std::string COAP_TAG = "coap://";

            RCSResourceObject::Ptr sceneListResourcePtr;
            // < std::string uri, SceneCollection::Ptr collection_instance >
            std::map< std::string, SceneCollection::Ptr > sceneCollections;

            RCSSetResponse setRequestHandler(const RCSRequest & /*request*/,
                    RCSResourceAttributes & attributes)
            {
                auto newObject = SceneList::getInstance()->createSceneCollection();

                if (newObject == nullptr)
                {
                    return RCSSetResponse::create(attributes, (int) OC_STACK_ERROR).setAcceptanceMethod(
                            RCSSetResponse::AcceptanceMethod::IGNORE);
                }

                newObject->setName(attributes.at(SCENE_LIST_NAME).get< std::string >());

                attributes.at(SCENE_LIST_ID) = newObject->getId();
                attributes.at(SCENE_LIST_LINK) = COAP_TAG + newObject->getUri();

                return RCSSetResponse::create(attributes).setAcceptanceMethod(
                        RCSSetResponse::AcceptanceMethod::IGNORE);
            }
        }

        SceneList::SceneList()
        {
            sceneListResourcePtr = RCSResourceObject::Builder(SCENE_LIST_URI, SCENE_LIST_RT_NAME,
            OC_RSRVD_INTERFACE_DEFAULT).setDiscoverable(true).setObservable(false).build();

            sceneListResourcePtr->setAttribute(SCENE_LIST_NAME, SCENE_LIST_DEFAULT_NAME);
            sceneListResourcePtr->setAttribute(SCENE_LIST_RTS, SCENE_LIST_RTS_VALUE);
            sceneListResourcePtr->setSetRequestHandler(&setRequestHandler);
        }

        SceneList* SceneList::getInstance()
        {
            static SceneList instance;
            return &instance;
        }

        SceneCollection::Ptr SceneList::createSceneCollection()
        {
            auto newSceneCollectionObj = std::make_shared< SceneCollection >();

            if (newSceneCollectionObj != nullptr)
            {
                sceneCollections.insert(
                        std::make_pair(newSceneCollectionObj->getUri(), newSceneCollectionObj));
                // TODO : bind resource!!
                return newSceneCollectionObj;
            }
            return nullptr;
        }

        void SceneList::setName(const std::string& name)
        {
            sceneListResourcePtr->setAttribute(SCENE_LIST_NAME, name);
        }

        std::string SceneList::getName() const
        {
            return sceneListResourcePtr->getAttributeValue(SCENE_LIST_NAME).toString();
        }

    } /* namespace Service */
} /* namespace OIC */

