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

#ifndef SM_REMOTE_SCENEACTION_H_
#define SM_REMOTE_SCENEACTION_H_

#include <memory>
#include <string>
#include <mutex>

#include "RCSRemoteResourceObject.h"

namespace OIC
{
    namespace Service
    {

        class SceneMemberResourceRequestor;

        class RemoteSceneAction
        {
            public:
                typedef std::shared_ptr< RemoteSceneAction > Ptr;

                typedef std::function< void(int eCode) > UpdateCallback;

            public:
                ~RemoteSceneAction() = default;

                void setExecutionParameter(const std::string &key,
                    const RCSResourceAttributes::Value &value, UpdateCallback);
                void setExecutionParameter(const RCSResourceAttributes &attr, UpdateCallback);

                RCSResourceAttributes getExecutionParameter() const;

                RCSRemoteResourceObject::Ptr getRemoteResourceObject() const;

            private:
                RemoteSceneAction(std::shared_ptr< SceneMemberResourceRequestor >,
                                  const std::string &sceneName, const RCSResourceAttributes &);
                RemoteSceneAction(std::shared_ptr< SceneMemberResourceRequestor >,
                                  const std::string &sceneName,
                                  const std::string &key, const RCSResourceAttributes::Value &);

                void onUpdated(int, const RCSResourceAttributes &, const UpdateCallback &);

            private:
                std::string m_sceneName;
                std::mutex m_attributeLock;
                RCSResourceAttributes m_attributes;
                std::shared_ptr< SceneMemberResourceRequestor > m_requestor;

                friend class RemoteScene;
        };

    }
}

#endif /* SM_REMOTE_SCENEACTION_H_ */