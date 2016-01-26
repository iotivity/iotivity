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

#ifndef SM_SCENELIST_H_
#define SM_SCENELIST_H_

#include <SceneCollection.h>

#include <string>
#include <memory>

namespace OIC
{
    namespace Service
    {
//        class RCSResourceObject;
        class RCSRemoteResourceObject;

        class SceneList
        {
        public:
            typedef std::shared_ptr< RCSRemoteResourceObject > RemoteObjectPtr;

        private:
            SceneList();
            ~SceneList() = default;

        public:
            static SceneList * getInstance();
            SceneCollection::Ptr createSceneCollection();

            void setName(const std::string&);
            std::string getName() const;
        };
    } /* namespace Service */
} /* namespace OIC */
#endif /* SM_SCENELIST_H_ */
