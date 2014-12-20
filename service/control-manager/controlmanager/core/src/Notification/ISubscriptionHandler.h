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

#include "CMConfig.h"

namespace OC
{
    namespace Cm
    {
        namespace Notification
        {
            /**
            *  @addtogroup Notification
            *  @{
            */
            /**
             * @interface   ISubscriptionHandler
             * @brief       Defines methods required to handle requests related to subscription and notification
             *
             * @since       1.0
             *
             * This interface defines methods required to handle requests related to subscription and notification
             *
             */
            class CMAPI ISubscriptionHandler
            {
                public:

                    /**
                     * Default destructor of this class
                     */
                    virtual ~ISubscriptionHandler() {};

                    /**
                     *
                     * handles GET request on Subscripions resource
                     *
                     * @param[out] response response data
                     * @param[out] statusCode response status code
                     *
                     * @return  returns @c true if request is sent successfully @n
                     *                  @c false in case of any error
                     *
                     */
                    virtual bool handleGetSubscriptionsRequest(OC::Cm::Serialization::ISerializable *response,
                            int &statusCode) = 0;

                    /**
                     *
                     * handles add subscription request
                     *
                     * @param[in] deviceAddress address of the device from which this request received
                     * @param[in] request   request data
                     * @param[out] location location header value
                     * @param[out] statusCode response status code
                     *
                     * @return  returns @c true if request is sent successfully @n
                     *                  @c false in case of any error
                     *
                     */
                    virtual bool handleSubscriptionRequest( std::string &deviceAddress,
                                                            OC::Cm::Serialization::ISerializable *request,
                                                            std::string &location, int &statusCode) = 0;

                    /**
                     *
                     * handles update subscription request
                     *
                     * @param[in] deviceAddress address of the device from which this request received
                     * @param[in] subscriptionId subscription id
                     * @param[in] request   request data
                     * @param[out] statusCode response status code
                     *
                     * @return  returns @c true if request is sent successfully @n
                     *                  @c false in case of any error
                     *
                     */
                    virtual bool handleUpdateSubscriptionRequest( std::string &deviceAddress,
                            std::string &subscriptionId,
                            OC::Cm::Serialization::ISerializable *request, int &statusCode) = 0;
                    /**
                     *
                     * handles delete subscription request
                     *
                     * @param[in] deviceAddress address of the device from which this request received
                     * @param[in] subscriptionId subscription id
                     * @param[out] statusCode response status code
                     *
                     * @return  returns @c true if request is sent successfully @n
                     *                  @c false in case of any error
                     *
                     */
                    virtual bool handleDeleteSubscriptionRequest ( std::string &deviceAddress,
                            std::string &subscriptionId, int &statusCode ) = 0;


                    /**
                     *
                     * handles get subscription request
                     *
                     * @param[in] deviceAddress address of the device from which this request received
                     * @param[in] subscriptionId subscription id
                     * @param[out] response response data
                     * @param[out] statusCode response status code
                     *
                     * @return  returns @c true if request is sent successfully @n
                     *                  @c false in case of any error
                     *
                     */
                    virtual bool handleGetSubscriptionUri( std::string &deviceAddress, std::string &subscriptionId,
                                                           OC::Cm::Serialization::ISerializable *response, int &statusCode) = 0;

                    /**
                     *
                     * handles notification request
                     *
                     * @param[in] deviceAddress address of the device from which this request received
                     * @param[in] request   request data
                     * @param[out] statusCode response status code
                     *
                     * @return  returns @c true if request is sent successfully @n
                     *                  @c false in case of any error
                     *
                     */
                    virtual bool handleNotification( std::string &deviceAddress,
                                                     OC::Cm::Serialization::ISerializable *request, int &statusCode) = 0;
            };
            /** @} */
        }
    }
}



#endif /* ISUBSCRIPTIONHANDLER_H_ */
