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

#ifndef SM_SCENEACTION_H_
#define SM_SCENEACTION_H_

#include "SceneMemberObject.h"

namespace OIC
{
    namespace Service
    {
        class SceneAction
        {
        public:
            typedef std::shared_ptr< SceneAction > Ptr;
            typedef std::function< void(const RCSResourceAttributes&, int) > ExecuteCallback;

        public:
            SceneAction(const SceneMemberObject::Ptr&, const std::string&,
                    const RCSResourceAttributes::Value&);

            bool execute();

            void setCallback(ExecuteCallback);

        private:
            RCSResourceAttributes m_attr;
            SceneMemberObject::Ptr m_sceneMemberPtr;
            ExecuteCallback m_callback;
        };
    } /* namespace Service */
} /* namespace OIC */

#endif /* SM_SCENEACTION_H_ */

