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

#include "SceneCollection.h"
#include "SceneCollectionResourceObject.h"

#include "octypes.h"
#include "ocrandom.h"

namespace OIC
{
    namespace Service
    {
        SceneCollection::SceneCollection(
                const SceneCollectionResourceObject::Ptr& sceneCollectionResource) :
                m_sceneCollectionResourceObj(sceneCollectionResource) {}

        Scene::Ptr SceneCollection::addNewScene(const std::string& sceneName)
        {
            if(sceneName.empty())
            {
                throw RCSInvalidParameterException("Scene name is an empty string");
            }

            m_sceneCollectionResourceObj->addScene(sceneName);

            for (const auto& it : m_scenes)
            {
                if (it.first.compare(sceneName) == 0)
                {
                    return it.second;
                }
            }

            Scene::Ptr scenePtr(new Scene(sceneName, m_sceneCollectionResourceObj));
            m_scenes.insert(std::pair<const std::string, Scene::Ptr>(sceneName, scenePtr));

            return scenePtr;
        }

        std::map< const std::string, Scene::Ptr > SceneCollection::getScenes() const
        {
            return m_scenes;
        }

        Scene::Ptr SceneCollection::getScene(const std::string& sceneName) const
        {
            auto it = m_scenes.find(sceneName);
            if (it != m_scenes.end())
            {
                return it->second;
            }
        }

        void SceneCollection::removeScene(Scene::Ptr scenePtr)
        {
            if (scenePtr == nullptr)
            {
                throw RCSInvalidParameterException("Scene Ptr is empty!");
            }
        }

        void SceneCollection::setName(const std::string& name)
        {
            m_sceneCollectionResourceObj->setName(name);
        }

        std::string SceneCollection::getName() const
        {
            return m_sceneCollectionResourceObj->getName();
        }

        std::string SceneCollection::getId() const
        {
            return m_sceneCollectionResourceObj->getId();
        }
    } /* namespace Service */
} /* namespace OIC */

