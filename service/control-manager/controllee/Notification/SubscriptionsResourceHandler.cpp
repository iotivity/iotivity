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
#include "ISubscriptionManager.h"
#include "ISubscriptionHandler.h"
#include "IContext.h"
#include "ControlResponse.h"
#include "log.h"

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

            bool
            SubscriptionsResourceHandler::handleSyncResource(
                std::string methodId,
                int &statusCode,
                OC::Cm::Serialization::ISerializable  *request,
                OC::Cm::Serialization::ISerializable  *response)
            {
                // NOTE: DO NOT modify this function

                CE(printf("######### SubscriptionsResourceHandler::handleSyncResource 1  ##############\n"));


                std::string allowed_methods = "GET, POST";

                if (std::string::npos != allowed_methods.find(methodId))
                {
                    if ( 0 == methodId.compare("GET") )
                    {
                        if ( false == validateRepresentation(response, EN_SUBSCRIPTION )) { statusCode = 404 ; return true ; }
                        this->onGET( statusCode, (::Subscriptions * )response);

                    }
                    else if ( 0 == methodId.compare("POST") )
                    {
                        CE(printf("######### SubscriptionsResourceHandler::handleSyncResource 3  ##############\n"));


                        if ( false == validateRepresentation(request, EN_SUBSCRIPTION ))
                        {
                            statusCode = 404 ;
                            return true ;
                        }
                        std::string location ;
                        this->onPOST( statusCode, (::Subscription * )request, location);
                        response->resp.m_headers.setHeader("2048", location); // TODO:#define if it works
                    }
                }
                else
                {
                    statusCode = 405 ;
                }

                return true ;
            }

            bool
            SubscriptionsResourceHandler::onGET(  int &statusCode , ::Subscriptions *respData)

            {

                CE(printf("######### SubscriptionsResourceHandler::onGET 1  ##############\n"));


                if ( NULL == p_context->getSubscriptionManager() )
                {
                    statusCode = 405 ;
                    return true ;
                }

                return p_context->getSubscriptionManager()->getSubscriptionHandler()->handleGetSubscriptionsRequest ( respData, statusCode )  ;
            }

            bool
            SubscriptionsResourceHandler::onPOST(  int &statusCode , ::Subscription *reqData , std::string &location)

            {

                CE(printf("######### SubscriptionsResourceHandler::onPOST 1  ##############\n"));


                std::string clientAddress; // =  session.getClientAddress();

                if ( NULL == p_context->getSubscriptionManager() )
                {
                    statusCode = 405 ;
                    return true ;
                }

                bool bRet =  p_context->getSubscriptionManager()->getSubscriptionHandler()->handleSubscriptionRequest ( clientAddress , reqData, location, statusCode )  ;

                CE(std::cout << "Subscription status code " << statusCode << std::endl);

                return bRet;
            }

        }
    }
}
