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

#include "RCSRemoteResourceObject.h"
#include "RCSResourceObject.h"
#include "SceneAction.h"

#include <vector>
#include <memory>

namespace OIC
{
    namespace Service
    {
        class Scene
        {
        public:
            typedef std::shared_ptr< Scene > Ptr;
            typedef std::function< void(const RCSResourceAttributes&, int) > ExecuteCallback;
        public:
            Scene (const std::string&);
            Scene(const std::string&, ExecuteCallback);
            ~Scene();

            bool addSceneAction(const std::shared_ptr< SceneAction >&);
            bool removeSceneAction(const std::shared_ptr< SceneAction >&);

            bool execute();

            void setCallback(ExecuteCallback);

            std::string getName() const;
            void setName(const std::string);

        private:
            void onSceneActionExecuteResult(const RCSResourceAttributes& attributes, int);

        private:
            std::string m_sceneName;
            std::vector< std::shared_ptr< SceneAction > > m_sceneActionList;
            ExecuteCallback m_callback;
        };

    } /* namespace Service */
} /* namespace OIC */

#endif /* RH_HOSTINGOBJECT_H_ */

