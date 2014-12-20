//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

#include "ISerializable.h"
#include "ServerSession.h"
#include "CDContext.h"
#include "IContext.h"
#include "AsyncResourceHandler.h"

namespace OC
{
    namespace Cm
    {
        namespace Server
        {
            namespace ResourceHandler
            {

                AsyncResourceHandler::AsyncResourceHandler(IContext *context)
                {
                    p_context = context;
                }

                bool AsyncResourceHandler::sendResponse(int statusCode,
                                                        OC::Cm::Connector::Server::ServerSession &session,
                                                        OC::Cm::Serialization::ISerializable *response)
                {
                    OC::CDContext *pContext = (OC::CDContext *) p_context;
                    return false;
                }

                bool AsyncResourceHandler::handleResource(
                    OC::Cm::Connector::Server::ServerSession &session, std::string methodId,
                    OC::Cm::Serialization::ISerializable *request,
                    OC::Cm::Serialization::ISerializable *response)
                {
                    return this->handleAsyncResource(session, methodId, request, response);
                }

                bool AsyncResourceHandler::onInit()
                {
                    return true;
                }

                bool AsyncResourceHandler::onDeinit()
                {
                    return true;
                }

            }
        }
    }
}
