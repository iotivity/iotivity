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

#ifndef SM_SCENECOLLECTION_H_
#define SM_SCENECOLLECTION_H_

#include "Scene.h"
#include <functional>
#include <map>

namespace OIC
{
    namespace Service
    {
        class SceneCollectionResource;
        class SceneCollection
        {
        public:
            class InvalidSceneNameException: public RCSException
            {
                public:
                InvalidSceneNameException(std::string&& what) :
                    RCSException{ std::move(what) } {}
            };

            typedef std::shared_ptr< SceneCollection > Ptr;

        private:
            SceneCollection(const std::shared_ptr< SceneCollectionResource >&);
            friend class SceneList;

        public:
            Scene::Ptr addNewScene(const std::string&);
            std::map< const std::string, Scene::Ptr > getScenes() const;
            Scene::Ptr getScene(const std::string&) const;

            void removeScene(Scene::Ptr);

            void setName(const std::string&);
            std::string getName() const;
            std::string getId() const;

        private:
            std::map< const std::string, Scene::Ptr > m_scenes;
            std::shared_ptr< SceneCollectionResource > m_sceneCollectionResourceObj;

        };
    } /* namespace Service */
} /* namespace OIC */

#endif /* SM_SCENECOLLECTION_H_ */

