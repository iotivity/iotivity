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



#include "NotificationsResourceHandler.h"
#include "CMNotification.h"
namespace OC
{
    namespace Cm
    {
        namespace Notification
        {


            NotificationsResourceHandler::NotificationsResourceHandler(OC::Cm::IContext *pContext)
                : OC::Cm::Server::ResourceHandler::SyncResourceHandler(pContext)
            {
            }

            NotificationsResourceHandler::~NotificationsResourceHandler()
            {
            }

            /******************************** NOTE: DO NOT modify below code ********************************/
            bool
            NotificationsResourceHandler::handleSyncResource( OC::Cm::Connector::Server::ServerSession &session,
                    std::string methodId,
                    int &statusCode,
                    OC::Cm::Serialization::ISerializable  *request,
                    OC::Cm::Serialization::ISerializable  *response)
            {
                // NOTE: DO NOT modify this function

                std::string allowed_methods = "POST";

                if (std::string::npos != allowed_methods.find(methodId))
                {
                    if ( 0 == methodId.compare("POST") )
                    {
                        if ( false == validateRepresentation(request, EN_NOTIFICATION )) { statusCode = 404 ; return true ; }
                        this->onPOST(session, statusCode, (::Notification *)request);

                    }
                }
                else
                {
                    statusCode = 405 ;
                    session.getResponse().m_headers.setHeader( "Allow", allowed_methods ) ;
                }

                return true ;
            }

            bool
            NotificationsResourceHandler::onPOST( OC::Cm::Connector::Server::ServerSession &session,
                                                  int &statusCode , ::Notification *reqData)

            {
                if ( NULL == p_context->getSubscriptionManager() )
                {
                    statusCode = 405 ;
                    return true ;
                }

                std::string clientAddress = session.getClientAddress() ;
                return p_context->getSubscriptionManager()->getSubscriptionHandler()->handleNotification(
                           clientAddress, reqData, statusCode);
            }

        }
    }
}
