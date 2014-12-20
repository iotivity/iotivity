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



#include "NotificationResourceHandler.h"
namespace OC
{
    namespace Cm
    {
        namespace Notification
        {



            NotificationResourceHandler::NotificationResourceHandler(OC::Cm::IContext *pContext)
                : OC::Cm::Server::ResourceHandler::SyncResourceHandler(pContext)
            {
            }

            NotificationResourceHandler::~NotificationResourceHandler()
            {
            }

            /******************************** NOTE: DO NOT modify below code ********************************/
            bool
            NotificationResourceHandler::handleSyncResource( OC::Cm::Connector::Server::ServerSession &session,
                    std::string methodId,
                    int &statusCode,
                    OC::Cm::Serialization::ISerializable  *request,
                    OC::Cm::Serialization::ISerializable  *response)
            {
                // NOTE: DO NOT modify this function

                std::string allowed_methods = "";

                if (std::string::npos != allowed_methods.find(methodId))
                {

                }
                else
                {
                    statusCode = 405 ;
                    session.getResponse().m_headers.setHeader( "Allow", allowed_methods ) ;
                }

                return true ;
            }

        }
    }
}
