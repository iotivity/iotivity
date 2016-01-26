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

#include <functional>
#include <memory>

#include "RCSResourceObject.h"
#include "RCSRemoteResourceObject.h"
#include "SceneMemberObject.h"
#include "Scene.h"

namespace OIC
{
    namespace Service
    {
        class Scene;
        class SceneCollection
        {
        public:
            typedef std::shared_ptr< SceneCollection > Ptr;

        public:
            SceneCollection();

            void setName(const std::string&);
            std::string getName() const;

            std::string getUri() const;
            std::string getId() const;

            Scene::Ptr addScene(const std::string&);
            const std::vector< Scene::Ptr >& getSceneList();
            bool removeScene(const Scene::Ptr&);

            SceneMemberObject::Ptr addSceneMember(const RCSRemoteResourceObject::Ptr&);
            const  std::vector< SceneMemberObject::Ptr >& getSceneMemberList();
            bool removeSceneMember(SceneMemberObject::Ptr);

        private:
            RCSSetResponse setRequestHandler(const RCSRequest&, RCSResourceAttributes&);
            RCSSetResponse createSceneRequestHandler(const RCSRequest&, RCSResourceAttributes&);
            RCSSetResponse executeSceneRequestHandler(const RCSRequest&, RCSResourceAttributes&);
            RCSSetResponse createSceneMemberRequestHandler(const RCSRequest&,
                    RCSResourceAttributes&);

        private:
            std::string m_Id;
            std::string m_Uri;

            std::vector< Scene::Ptr > m_SceneList;
            std::vector< SceneMemberObject::Ptr > m_SceneMemberObjectList;

            RCSResourceObject::Ptr m_sceneCollectionResourcePtr;
        };
    } /* namespace Service */
} /* namespace OIC */

#endif /* SM_SCENECOLLECTION_H_ */

