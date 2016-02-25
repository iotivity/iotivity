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

#ifndef SM_REMOTE_SCENECOLLECTION_H_
#define SM_REMOTE_SCENECOLLECTION_H_

#include <memory>
#include <functional>
#include <string>
#include <unordered_map>
#include <mutex>

#include "RemoteScene.h"
#include "RCSRemoteResourceObject.h"

namespace OIC
{
    namespace Service
    {

        class SceneCollectionResourceRequestor;

        class RemoteSceneCollection
        {
            public:
                typedef std::shared_ptr< RemoteSceneCollection > Ptr;

                typedef std::function< void(RemoteScene::Ptr, int eCode) >
                    AddNewSceneCallback;

                typedef std::function< void(int eCode) > RemoveSceneCallback;

                typedef std::function< void(int eCode) > SetNameCallback;

            public:
                ~RemoteSceneCollection() = default;

                void addNewScene(const std::string &name, AddNewSceneCallback);
                void removeScene(RemoteScene::Ptr, RemoveSceneCallback);

                std::unordered_map< std::string, RemoteScene::Ptr > getRemoteScenes() const;
                RemoteScene::Ptr getRemoteScene(const std::string &sceneName) const;

                void setName(const std::string &name, SetNameCallback);
                std::string getName() const;

                std::string getId() const;

            private:
                RemoteSceneCollection(
                    std::shared_ptr< SceneCollectionResourceRequestor >,
                    const std::string &id, const std::string &name);

                void initializeRemoteSceneCollection(const std::vector< RCSResourceAttributes > &,
                                                     const std::string &);

                RemoteScene::Ptr createRemoteSceneInstance(const std::string &);

                void onSceneAddedRemoved(int, const std::string &name, int,
                                         const AddNewSceneCallback &, const RemoveSceneCallback &);

                void onNameSet(int, const std::string &, const SetNameCallback &);

            private:
                std::string m_id;
                std::string m_name;
                mutable std::mutex m_sceneLock;
                std::unordered_map< std::string, RemoteScene::Ptr > m_remoteScenes;
                std::shared_ptr< SceneCollectionResourceRequestor > m_requestor;

                friend class RemoteSceneList;
        };

    }
}

#endif /* SM_REMOTE_SCENECOLLECTION_H_ */

