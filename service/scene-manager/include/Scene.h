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

#ifndef SM_SCENE_H_
#define SM_SCENE_H_

#include "SceneAction.h"

#include <vector>

namespace OIC
{
    namespace Service
    {
        class SceneCollectionResource;
        class Scene
        {
        public:
            class InvalidAddMemberRequestException: public RCSException
            {
            public:
                InvalidAddMemberRequestException(std::string&& what) :
                    RCSException{ std::move(what) } {}
            };

            typedef std::shared_ptr< Scene > Ptr;
            typedef std::function< void(int) >  ExecuteCallback;

        private:
            Scene(const Scene&) = default;
            Scene(const std::string&, std::shared_ptr<SceneCollectionResource>);
            friend class SceneCollection;

        public:
            SceneAction::Ptr addNewSceneAction(const RCSRemoteResourceObject::Ptr&,
                    const RCSResourceAttributes&);
            SceneAction::Ptr addNewSceneAction(const RCSRemoteResourceObject::Ptr&,
                    const std::string&, const RCSResourceAttributes::Value&);

            SceneAction::Ptr getSceneAction(
                    const RCSRemoteResourceObject::Ptr&) const;
            std::vector<SceneAction::Ptr> getSceneActions() const ;

            std::string getName() const;

            void removeSceneAction(const SceneAction::Ptr&);
            void removeSceneAction(const RCSRemoteResourceObject::Ptr&);

            void execute(ExecuteCallback);

        private:
            std::string m_name;
            std::shared_ptr< SceneCollectionResource > m_sceneCollectionResourceObj;

        };
    } /* namespace Service */
} /* namespace OIC */
#endif /* SM_SCENE_H_ */

