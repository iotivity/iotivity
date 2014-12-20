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

#ifndef SUBSCRIPTIONRESOURCEHANDLER_H_
#define SUBSCRIPTIONRESOURCEHANDLER_H_

#include "SyncResourceHandler.h"
#include "ISerializable.h"
#include "IContext.h"
#include "Elements.h"
#include <string>

namespace OC
{
    namespace Cm
    {
        namespace Notification
        {
            /**
             * @class       SubscriptionRequest
             * @brief       This is an class implements the resource handler for all subscriptions
             */

            class SubscriptionResourceHandler: public OC::Cm::Server::ResourceHandler::SyncResourceHandler
            {

                public:

                    /**
                     * SubscriptionResourceHandler - Parameterized constructor
                     *
                     * @param   context Represents the instance of context.
                     * @return -none
                     */
                    SubscriptionResourceHandler(OC::Cm::IContext *context);

                    /**
                     * ~SubscriptionResourceHandler - Default destructor
                     *
                     * @param-none
                     * @return -none
                     */
                    virtual ~SubscriptionResourceHandler();

                    /**
                     * handleSyncResource - Resource handler logic
                     *
                     * @param- methodID - The kind of HTTP call
                     * @param- statusCode - The status code of the output(output).
                     * @param- request - The request
                     * @param- response - The output. (output)
                     * @return -type:bool - True if the handling was successful, else false
                     */
                    bool handleSyncResource(std::string methodId, int &statusCode,
                                            OC::Cm::Serialization::ISerializable *request,
                                            OC::Cm::Serialization::ISerializable *response);

                    /**
                     * onGET - Handle get
                     *
                     * @param- statusCode - The status code of the output(output).
                     * @param- respData - The output (output)
                     * @return -type:bool - True if the handling was successful, else false
                     */
                    bool onGET(int &statusCode, ::Subscription *respData);

                    /**
                     * onPUT - Handle put
                     *
                     * @param- statusCode - The status code of the output(output).
                     * @param- reqData - The output (output)
                     * @return -type:bool - True if the handling was successful, else false
                     */
                    bool onPUT(int &statusCode, ::Subscription *reqData);

                    /**
                     * onDELETE - Handle Delete
                     *
                     * @param- statusCode - The status code of the output(output).
                     * @return -type:bool - True if the handling was successful, else false
                     */
                    bool onDELETE(int &statusCode);

            };
        }
    }
}

#endif /* SUBSCRIPTIONRESOURCEHANDLER_H_ */
