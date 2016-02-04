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

#ifndef SM_REMOTE_SCENELIST_H_
#define SM_REMOTE_SCENELIST_H_

#include <memory>
#include <functional>

#include "RemoteSceneCollection.h"
#include "RCSRemoteResourceObject.h"

namespace OIC
{
    namespace Service
    {
        class SceneListResourceRequestor;
        class RemoteSceneList
        {
            public:
                typedef std::shared_ptr< RemoteSceneList > Ptr;

                typedef std::function< void(RemoteSceneList::Ptr, int) > CreateInstanceCallback;

                typedef std::function< void(RemoteSceneCollection::Ptr, int) >
                AddNewSceneCollectionCallback;

                typedef std::function< void(int eCode) > SetNameCallback;

            public:
                ~RemoteSceneList();

                static void createInstance
                (RCSRemoteResourceObject::Ptr pSceneListResource, CreateInstanceCallback);

                void addNewSceneCollection(AddNewSceneCollectionCallback);
                void removeSceneCollection(RemoteSceneCollection::Ptr);

                std::vector< RemoteSceneCollection::Ptr > getRemoteSceneCollections() const;

                void setName(const std::string &name, SetNameCallback);
                std::string getName() const;

            private:
                RemoteSceneList(std::shared_ptr< SceneListResourceRequestor > pRequestor,
                                const std::string &name);

                void onSceneCollectionCreated
                (const std::string &link, const std::string &id, const std::string &name,
                 AddNewSceneCollectionCallback);

            private:
                std::string m_name;
                std::vector< RemoteSceneCollection::Ptr > m_remoteSceneCollections;

                std::shared_ptr< SceneListResourceRequestor > m_pRequestor;
        };
    }
}

#endif /* SM_REMOTE_SCENELIST_H_ */

