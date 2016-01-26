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

#include "SceneAction.h"

namespace OIC
{
    namespace Service
    {
        SceneAction::SceneAction(const SceneMemberObject::Ptr& SceneMemberObjectPtr,
                const std::string& key, const RCSResourceAttributes::Value& value) :
                m_sceneMemberPtr(SceneMemberObjectPtr)
        {
            m_attr[key] = RCSResourceAttributes::Value(value);
        }

        void SceneAction::setCallback(ExecuteCallback cb)
        {
            if (cb == nullptr)
            {
                throw RCSInvalidParameterException { "Callback is empty!" };
            }

            m_callback = std::move(cb);
        }

        bool SceneAction::execute()
        {
            m_sceneMemberPtr->getRemoteResourceObject()->setRemoteAttributes(m_attr, m_callback);
            return true;
        }
    } /* namespace Service */
} /* namespace OIC */

