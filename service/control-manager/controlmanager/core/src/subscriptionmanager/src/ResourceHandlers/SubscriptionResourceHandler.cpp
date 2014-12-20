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



#include "SubscriptionResourceHandler.h"
#include "CMNotification.h"

namespace OC
{
    namespace Cm
    {
        namespace Notification
        {

            SubscriptionResourceHandler::SubscriptionResourceHandler(OC::Cm::IContext *pContext)
                : OC::Cm::Server::ResourceHandler::SyncResourceHandler(pContext)
            {
            }

            SubscriptionResourceHandler::~SubscriptionResourceHandler()
            {
            }

            /******************************** NOTE: DO NOT modify below code ********************************/
            bool
            SubscriptionResourceHandler::handleSyncResource( OC::Cm::Connector::Server::ServerSession &session,
                    std::string methodId,
                    int &statusCode,
                    OC::Cm::Serialization::ISerializable  *request,
                    OC::Cm::Serialization::ISerializable  *response)
            {
                // NOTE: DO NOT modify this function

                std::string allowed_methods = "GET, PUT, DELETE";

                if (std::string::npos != allowed_methods.find(methodId))
                {
                    if ( 0 == methodId.compare("GET") )
                    {
                        if ( false == validateRepresentation(response, EN_SUBSCRIPTION )) { statusCode = 404 ; return true ; }
                        this->onGET(session, statusCode, (::Subscription *)response);
                    }
                    else if ( 0 == methodId.compare("PUT") )
                    {
                        if ( false == validateRepresentation(request, EN_SUBSCRIPTION )) { statusCode = 404 ; return true ; }
                        this->onPUT(session, statusCode, (::Subscription *)request);
                    }
                    else if ( 0 == methodId.compare("DELETE") )
                    {
                        this->onDELETE(session, statusCode);
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
            SubscriptionResourceHandler::onGET( OC::Cm::Connector::Server::ServerSession &session,
                                                int &statusCode , ::Subscription *respData)

            {
                std::string subscriptionId =  session.getRequest().m_template_parameters["subscriptionId"] ;
                std::string clientAddress = session.getClientAddress() ;

                if ( NULL == p_context->getSubscriptionManager() )
                {
                    statusCode = 405 ;
                    return true ;
                }

                return p_context->getSubscriptionManager()->getSubscriptionHandler()->handleGetSubscriptionUri (
                           clientAddress, subscriptionId, respData, statusCode) ;
            }

            bool
            SubscriptionResourceHandler::onPUT( OC::Cm::Connector::Server::ServerSession &session,
                                                int &statusCode , ::Subscription *reqData)

            {
                std::string subscriptionId =  session.getRequest().m_template_parameters["subscriptionId"] ;
                std::string clientAddress = session.getClientAddress() ;

                if ( NULL == p_context->getSubscriptionManager() )
                {
                    statusCode = 405 ;
                    return true ;
                }

                return p_context->getSubscriptionManager()->getSubscriptionHandler()->handleUpdateSubscriptionRequest(
                           clientAddress, subscriptionId, reqData, statusCode) ;
            }


            bool
            SubscriptionResourceHandler::onDELETE( OC::Cm::Connector::Server::ServerSession &session,
                                                   int &statusCode)

            {
                std::string subscriptionId =  session.getRequest().m_template_parameters["subscriptionId"] ;
                std::string clientAddress = session.getClientAddress() ;

                if ( NULL == p_context->getSubscriptionManager() )
                {
                    statusCode = 405 ;
                    return true ;
                }

                return p_context->getSubscriptionManager()->getSubscriptionHandler()->handleDeleteSubscriptionRequest(
                           clientAddress, subscriptionId, statusCode) ;
            }
        }
    }
}
