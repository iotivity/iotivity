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

#include "RCSRemoteResourceObject.h"
#include "RCSResourceAttributes.h"

#include <memory>

namespace OIC
{
    namespace Service
    {
        class SceneMemberResourceObject;
        class SceneAction
        {
        public:
            typedef std::shared_ptr< SceneAction > Ptr;

        private:
            SceneAction(const std::shared_ptr< SceneMemberResourceObject >,
                    const std::string&, const RCSResourceAttributes&);
            SceneAction(const std::shared_ptr< SceneMemberResourceObject >,
                    const std::string&, const std::string&,
                    const RCSResourceAttributes::Value&);
            friend class Scene;

        public:
            void update(const std::string&, RCSResourceAttributes::Value);
            void update(const RCSResourceAttributes&);

            const RCSResourceAttributes getAction();
            RCSRemoteResourceObject::Ptr getRemoteResourceObject() const;

        private:
            RCSRemoteResourceObject::Ptr m_pRemoteResourceObject;
            std::string m_sceneName;
            std::shared_ptr< SceneMemberResourceObject > m_sceneMemberResourceObj;
            RCSResourceAttributes m_attr;
        };
    } /* namespace Service */
} /* namespace OIC */
#endif /* SM_SCENEACTION_H_ */
