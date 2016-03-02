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
#include <unordered_map>
#include <mutex>

#include "RemoteSceneAction.h"
#include "RCSRemoteResourceObject.h"

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

                typedef std::function< void(RemoteSceneAction::Ptr, int eCode) >
                    AddNewSceneActionCallback;

                typedef std::function< void(const int eCode) >
                    RemoveSceneActionCallback;

                typedef std::function< void(const std::string &sceneName, int eCode) >
                    RemoteSceneExecuteCallback;

            public:
                ~RemoteScene() = default;

                void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
                                       const RCSResourceAttributes &,
                                       AddNewSceneActionCallback);
                void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
                                       const std::string &key,
                                       const RCSResourceAttributes::Value &,
                                       AddNewSceneActionCallback);

                void removeSceneAction(RemoteSceneAction::Ptr remoteSceneAction,
                                       RemoveSceneActionCallback);
                void removeSceneAction(RCSRemoteResourceObject::Ptr targetResource,
                                       RemoveSceneActionCallback);

                std::vector< RemoteSceneAction::Ptr > getRemoteSceneActions() const;
                RemoteSceneAction::Ptr getRemoteSceneAction(
                    const RCSRemoteResourceObject::Ptr targetResource) const;

                std::string getName() const;

                void execute(RemoteSceneExecuteCallback);

            private:
                RemoteScene(
                    const std::string &name, std::shared_ptr< SceneCollectionResourceRequestor >);

                RemoteSceneAction::Ptr createRemoteSceneAction(
                    const std::string &,  const RCSResourceAttributes &);

                void addExistingRemoteSceneAction(const std::string &, const std::string &,
                    RCSRemoteResourceObject::Ptr, const std::string &key,
                    const RCSResourceAttributes::Value &);

                void onSceneActionAdded(
                    int, RCSRemoteResourceObject::Ptr,
                    const RCSResourceAttributes &, const AddNewSceneActionCallback &);

                void onSceneExecuted(const std::string &name, int,
                                    const RemoteSceneExecuteCallback &);

            private:
                std::string m_name;
                mutable std::mutex m_sceneActionLock;
                std::unordered_map < std::string, RemoteSceneAction::Ptr >
                    m_remoteSceneActions;

                std::shared_ptr< SceneCollectionResourceRequestor > m_requestor;

                friend class RemoteSceneCollection;
        };

    }
}

#endif /* SM_REMOTE_SCENE_H_ */

