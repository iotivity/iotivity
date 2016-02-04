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

#ifndef SM_REMOTE_SCENE_H_
#define SM_REMOTE_SCENE_H_

#include <memory>
#include <string>
#include <map>

#include "RemoteSceneAction.h"
#include "RCSRemoteResourceObject.h"
#include "RCSResourceAttributes.h"

namespace OIC
{
    namespace Service
    {
        class SceneCollectionResourceRequestor;
        class SceneMemberResourceRequestor;
        class RemoteScene
        {
            public:
                typedef std::shared_ptr< RemoteScene > Ptr;

                typedef std::function< void(const RemoteSceneAction::Ptr) >
                AddNewSceneActionCallback;

                typedef std::function< void(const int eCode) > RemoveSceneActionCallback;

                typedef std::function< void(const int eCode) > RemoteSceneExecuteCallback;

            public:
                ~RemoteScene();

                void addNewSceneAction(RCSRemoteResourceObject::Ptr pTargetResource,
                                       const RCSResourceAttributes &attr,
                                       AddNewSceneActionCallback);
                void addNewSceneAction(RCSRemoteResourceObject::Ptr pTargetResource,
                                       const std::string &key,
                                       const RCSResourceAttributes::Value &value,
                                       AddNewSceneActionCallback);

                void removeSceneAction(RemoteSceneAction::Ptr pRemoteSceneAction,
                                       RemoveSceneActionCallback);
                void removeSceneAction(RCSRemoteResourceObject::Ptr pTargetResource,
                                       RemoveSceneActionCallback);

                std::vector< RemoteSceneAction::Ptr > getRemoteSceneActions() const;
                std::vector< RemoteSceneAction::Ptr > getRemoteSceneAction
                (RCSRemoteResourceObject::Ptr pTargetResource) const;

                std::string getName() const;

                void execute(RemoteSceneExecuteCallback);

            private:
                RemoteScene(const std::string &name,
                            std::shared_ptr< SceneCollectionResourceRequestor >);

                void onSceneMemberAdded(const std::string &link, const std::string &id, int eCode);

            private:
                std::string m_name;
                std::map < std::string, std::vector< RemoteSceneAction::Ptr > >
                m_mapRemoteSceneActions;

                std::shared_ptr< SceneCollectionResourceRequestor > m_pRequestor;
        };
    }
}

#endif /* SM_REMOTE_SCENE_H_ */

