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

#include "RequestObject.h"

#include "RCSResourceObject.h"
#include "RCSSeparateResponse.h"

namespace OIC
{
    namespace Service
    {
        void RequestObject::invokeRequest(RCSRemoteResourceObject::Ptr remoteObject,
                const RCSRequest & request, RequestMethod method,
                const RCSResourceAttributes & resourceAttibutes, SetRequestCallback setCB)
        {
            RequestObject::Ptr createdRequestObject = std::make_shared<RequestObject>();

            RCSRequest req(request.getResourceObject().lock(), request.getOCRequest());
            switch (method)
            {
            case RequestMethod::Set:
            {
                if (!setCB)
                {
                    remoteObject->setRemoteAttributes(resourceAttibutes,
                            std::bind(&RequestObject::setRequestCB, createdRequestObject,
                                    std::placeholders::_1, std::placeholders::_2,
                                    req, createdRequestObject));
                }
                else
                {
                    remoteObject->setRemoteAttributes(resourceAttibutes,
                            std::bind(std::move(setCB),
                                    std::placeholders::_1, std::placeholders::_2,
                                    req, createdRequestObject));
                }
            }
                break;
            case RequestMethod::Get:
            case RequestMethod::Delete:
            default:
                // unknown type of method.
                break;
            }
        }

        void RequestObject::setRequestCB(
                const RCSResourceAttributes & returnedAttributes, int /*eCode*/,
                const RCSRequest & request, RequestObject::Ptr /*this_ptr*/)
        {
            auto server = request.getResourceObject().lock();
            if (!server) return;

            RCSResourceObject::LockGuard guard(server);
            server->getAttributes() = RCSResourceAttributes(returnedAttributes);

            // TODO need to set error code.
            RCSSeparateResponse(request).set();
        }

    } /* namespace Service */
} /* namespace OIC */
