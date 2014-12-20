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

#ifndef ISUBSCRIPTIONHANDLER_H_
#define ISUBSCRIPTIONHANDLER_H_

#include "ISerializable.h"

namespace OC
{
    namespace Cm
    {
        namespace Notification
        {
            /**
             * @interface   ISubscriptionHandler
             * @brief       Defines methods required to handle requests related to subscription and notification
             *
             * This interface defines methods required to handle requests related to subscription and notification
             *
             */
            class ISubscriptionHandler
            {
                public:

                    /**
                     * ~ISubscriptionHandler - Default destructor of this class
                     * @param -none
                     * @return -none
                     */
                    virtual ~ISubscriptionHandler()
                    {
                    }
                    ;

                    /**
                     *
                     * handleGetSubscriptionsRequest - handles GET request on Subscripions resource
                     *
                     * @param response  response data
                     * @param statusCode response status code
                     *
                     * @return  type:bool - returns true if request is sent successfully, false in case of any error
                     *
                     */
                    virtual bool handleGetSubscriptionsRequest(
                        OC::Cm::Serialization::ISerializable *response,
                        int &statusCode) = 0;

                    /**
                     *
                     * handleSubscriptionRequest - handles add subscription request
                     *
                     * @param deviceAddress address of the device from which this request received(input)
                     * @param request   request data(input)
                     * @param location location header value(output)
                     * @param statusCode response status code(output)
                     *
                     * @return  type:bool - returns true if request is sent successfully ,false in case of any error
                     *
                     */
                    virtual bool handleSubscriptionRequest(std::string &deviceAddress,
                                                           OC::Cm::Serialization::ISerializable *request,
                                                           std::string &location, int &statusCode) = 0;

                    /**
                     *
                     * handleUpdateSubscriptionRequest - handles update subscription request
                     *
                     * @param deviceAddress address of the device from which this request received(input)
                     * @param subscriptionId subscription id(input)
                     * @param request   request data(output)
                     * @param statusCode response status code(output)
                     *
                     * @return  type:bool returns true if request is sent successfully, false in case of any error
                     *
                     */
                    virtual bool handleUpdateSubscriptionRequest(std::string &deviceAddress,
                            std::string &subscriptionId,
                            OC::Cm::Serialization::ISerializable *request, int &statusCode) = 0;
                    /**
                     *
                     * handleDeleteSubscriptionRequest - handles delete subscription request
                     *
                     * @param deviceAddress address of the device from which this request received(input)
                     * @param subscriptionId subscription id(input)
                     * @param statusCode response status code(output)
                     *
                     * @return  type:bool returns true if request is sent successfully,false in case of any error
                     *
                     */
                    virtual bool handleDeleteSubscriptionRequest(std::string &deviceAddress,
                            std::string &subscriptionId, int &statusCode) = 0;

                    /**
                     *
                     * handleGetSubscriptionUri - handles get subscription request
                     *
                     * @param deviceAddress address of the device from which this request received(input)
                     * @param subscriptionId subscription id(input)
                     * @param response  response data(output)
                     * @param statusCode response status code(output)
                     *
                     * @return  returns true if request is sent successfully , false in case of any error
                     *
                     */
                    virtual bool handleGetSubscriptionUri(std::string &deviceAddress,
                                                          std::string &subscriptionId,
                                                          OC::Cm::Serialization::ISerializable *response,
                                                          int &statusCode) = 0;
            };
        }
    }
}

#endif /* ISUBSCRIPTIONHANDLER_H_ */
