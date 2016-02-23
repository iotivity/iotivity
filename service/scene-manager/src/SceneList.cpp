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
#include "SceneCollectionResource.h"
#include "SceneListResource.h"

namespace OIC
{
    namespace Service
    {
        SceneList * SceneList::getInstance()
        {
            static SceneList instance;
            return &instance;
        }

        SceneCollection::Ptr SceneList::addNewSceneCollection()
        {
            auto sceneCollectionResObj =
                    SceneCollectionResource::createSceneCollectionObject();
            SceneListResource::getInstance()->addSceneCollectionResource(sceneCollectionResObj);

            SceneCollection::Ptr sceneCollectionPtr(new SceneCollection(sceneCollectionResObj));
            return sceneCollectionPtr;
        }

        std::vector< SceneCollection::Ptr > SceneList::getSceneCollections() const
        {
            std::vector<SceneCollection::Ptr> sceneCollections;
            auto sceneCollectionResObjs = SceneListResource::getInstance()->getSceneCollections();
            for(const auto& it : sceneCollectionResObjs)
            {
                SceneCollection::Ptr sceneCollectionPtr(new SceneCollection(it));
                sceneCollections.push_back(sceneCollectionPtr);
            }
            return sceneCollections;
        }

        void SceneList::removeSceneCollection(SceneCollection::Ptr sceneCollectionPtr)
        {
//            TODO : remove logic
//            if (sceneCollectionPtr == nullptr)
//            {
//                throw RCSInvalidParameterException
//                { "Scene Collection Ptr is empty!" };
//            }
        }

        void SceneList::setName(const std::string& sceneListName)
        {
            SceneListResource::getInstance()->setName(sceneListName);
        }

        std::string SceneList::getName() const
        {
            return SceneListResource::getInstance()->getName();
        }
    } /* namespace Service */
} /* namespace OIC */

