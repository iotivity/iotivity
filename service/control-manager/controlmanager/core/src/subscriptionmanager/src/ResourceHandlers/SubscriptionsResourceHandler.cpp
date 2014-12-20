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



#include "SubscriptionsResourceHandler.h"
#include "CMNotification.h"

namespace OC
{
    namespace Cm
    {
        namespace Notification
        {

            SubscriptionsResourceHandler::SubscriptionsResourceHandler(OC::Cm::IContext *pContext)
                : OC::Cm::Server::ResourceHandler::SyncResourceHandler(pContext)
            {
            }

            SubscriptionsResourceHandler::~SubscriptionsResourceHandler()
            {
            }

            /******************************** NOTE: DO NOT modify below code ********************************/
            bool
            SubscriptionsResourceHandler::handleSyncResource( OC::Cm::Connector::Server::ServerSession &session,
                    std::string methodId,
                    int &statusCode,
                    OC::Cm::Serialization::ISerializable  *request,
                    OC::Cm::Serialization::ISerializable  *response)
            {
                // NOTE: DO NOT modify this function

                std::string allowed_methods = "GET, POST";

                if (std::string::npos != allowed_methods.find(methodId))
                {
                    if ( 0 == methodId.compare("GET") )
                    {
                        if ( false == validateRepresentation(response, EN_SUBSCRIPTIONS )) { statusCode = 404 ; return true ; }
                        this->onGET(session, statusCode, (::Subscriptions *)response);

                    }
                    else if ( 0 == methodId.compare("POST") )
                    {
                        if ( false == validateRepresentation(request, EN_SUBSCRIPTION )) { statusCode = 404 ; return true ; }
                        std::string location ;
                        this->onPOST(session, statusCode, (::Subscription *)request, location);
                        session.getResponse().m_headers.setHeader("Location", location);
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
            SubscriptionsResourceHandler::onGET( OC::Cm::Connector::Server::ServerSession &session,
                                                 int &statusCode , ::Subscriptions *respData)

            {
                if ( NULL == p_context->getSubscriptionManager() )
                {
                    statusCode = 405 ;
                    return true ;
                }

                return p_context->getSubscriptionManager()->getSubscriptionHandler()->handleGetSubscriptionsRequest (
                           respData, statusCode )  ;
            }

            bool
            SubscriptionsResourceHandler::onPOST( OC::Cm::Connector::Server::ServerSession &session,
                                                  int &statusCode , ::Subscription *reqData , std::string &location)

            {
                std::string clientAddress =  session.getClientAddress();

                if ( NULL == p_context->getSubscriptionManager() )
                {
                    statusCode = 405 ;
                    return true ;
                }

                return p_context->getSubscriptionManager()->getSubscriptionHandler()->handleSubscriptionRequest (
                           clientAddress , reqData, location, statusCode )  ;
            }

        }
    }
}
