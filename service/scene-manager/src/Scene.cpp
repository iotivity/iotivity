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

#include <iostream>

namespace OIC
{
    namespace Service
    {

        Scene::Scene(const std::string& sceneName, ExecuteCallback cb) :
                m_sceneName(sceneName), m_callback(std::move(cb))
        {
        }

        Scene::Scene(const std::string& sceneName) :
                m_sceneName(sceneName)
        {
        }

        Scene::~Scene()
        {
            m_sceneActionList.clear();
        }

        bool Scene::addSceneAction(const SceneAction::Ptr& newSceneAction)
        {
            if(newSceneAction == nullptr)
            {
                throw RCSInvalidParameterException { "SceneAction is empty!" };
            }

            newSceneAction->setCallback(
                    std::bind(&Scene::onSceneActionExecuteResult, this, std::placeholders::_1,
                            std::placeholders::_2));
            m_sceneActionList.push_back(newSceneAction);

            return true;
        }
        bool Scene::removeSceneAction(const SceneAction::Ptr& sceneAction)
        {
            // TODO
        }

        void Scene::setCallback(ExecuteCallback cb)
        {
            if(cb == nullptr)
            {
                throw RCSInvalidParameterException { "callback is empty!" };
            }

            m_callback = std::move(cb);
        }

        bool Scene::execute()
        {
            for (const auto& it : m_sceneActionList)
            {
                if (it->execute() == false)
                {
                    return false;
                }
            }
            return true;
        }

        std::string Scene::getName() const
        {
            return m_sceneName;
        }

        void Scene::setName(const std::string name)
        {
            m_sceneName = name;
        }

        void Scene::onSceneActionExecuteResult(const RCSResourceAttributes& attributes, int eCode)
        {
            if (attributes.empty())
            {
                std::cout << "\tattributes is empty" << std::endl;
            }

            for (const auto& attr : attributes)
            {
                std::cout << "\tkey : " << attr.key() << std::endl << "\tvalue : "
                        << attr.value().toString() << std::endl;

                if (m_callback != nullptr)
                {
                    m_callback(attributes, eCode);
                }
            }
        }

    } /* namespace Service */
} /* namespace OIC */

