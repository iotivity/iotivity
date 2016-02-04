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

#include "RCSRemoteResourceObject.h"
#include "RCSResourceAttributes.h"

namespace OIC
{
    namespace Service
    {
        class RemoteSceneAction
        {
            public:
                typedef std::shared_ptr< RemoteSceneAction > Ptr;

                typedef std::function< void(int eCode) > UpdateActionCallback;


            public:
                ~RemoteSceneAction();

                void updateAction(const RCSResourceAttributes &attr, UpdateActionCallback);
                void updateAction(const std::string &key,
                                  const RCSResourceAttributes::Value &value,
                                  UpdateActionCallback);

                RCSResourceAttributes getAction() const;

                RCSRemoteResourceObject::Ptr getRemoteResourceObject() const;

            private:
                RemoteSceneAction(RCSRemoteResourceObject::Ptr pResource,
                                  const RCSResourceAttributes &attr);
                RemoteSceneAction(RCSRemoteResourceObject::Ptr pResource,
                                  const std::string &key,
                                  const RCSResourceAttributes::Value &value);


            private:
                RCSResourceAttributes m_attributes;
                RCSRemoteResourceObject::Ptr m_pRemoteResourceObject;
        };
    }
}

#endif /* SM_REMOTE_SCENEACTION_H_ */
