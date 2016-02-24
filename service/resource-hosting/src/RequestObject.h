//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

#ifndef RH_REQUESTOBJECT_H_
#define RH_REQUESTOBJECT_H_

#include "RCSRemoteResourceObject.h"
#include "RCSRequest.h"

namespace OIC
{
    namespace Service
    {
        class RequestObject
        {
        public:
            typedef std::shared_ptr<RequestObject> Ptr;
            typedef std::function<void(const RCSResourceAttributes &, int,
                    const RCSRequest &, RequestObject::Ptr)> SetRequestCallback;

            enum class RequestMethod
            {
                Get = 0,
                Set,
                Post,
                Delete
            };

        private:
            typedef RCSRemoteResourceObject::Ptr RemoteObjectPtr;

        public:
            RequestObject() = default;
            ~RequestObject() = default;

            static void invokeRequest(RCSRemoteResourceObject::Ptr remoteObject,
                    const RCSRequest & request, RequestMethod method,
                    const RCSResourceAttributes & resourceAttibutes,
                    SetRequestCallback setCB = nullptr);

        private:
            void setRequestCB(const RCSResourceAttributes & returnedAttributes, int eCode,
                    const RCSRequest & request, RequestObject::Ptr this_ptr);
        };

    } /* namespace Service */
} /* namespace OIC */

#endif /* RH_REQUESTOBJECT_H_ */
