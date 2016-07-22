//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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

/**
 * @file
 *
 * This file contains Notification service Consumer representation.
 */

#ifndef _NS_CONSUMER_H_
#define _NS_CONSUMER_H_


#include <string>
#include <cstring>
#include "NSCommon.h"
#include "oic_string.h"
#include "NSConstants.h"

namespace OIC
{
    namespace Service
    {
        /**
         * @class   NSConsumer
         * @brief   This class provides a set of Notification service Consumer representation APIs.
         */
        class NSConsumer
        {
            public:
                /**
                      * Constructor of NSConsumer.
                      */
                NSConsumer() = default;

                /**
                      * Constructor of NSConsumer.
                      *
                      * @param consumerId - consumerId of the Notification service Consumer.
                      */
                NSConsumer(const std::string &consumerId)
                    : m_consumerId(consumerId) {}

                /**
                      * Constructor of NSConsumer.
                      *
                      * @param consumer - NSConsumer struct to initialize.
                      */
                NSConsumer(::NSConsumer *consumer);

                /**
                      * Destructor of NSConsumer.
                      */
                ~NSConsumer() = default;

                /**
                      * This method is for getting ConsumerId from the Notification service Consumer.
                      *
                      * @return ConsumerId as string.
                      */
                std::string getConsumerId() const;

                /**
                      * This method is for setting icon image for the Notification service media contents.
                      *
                      * @param consumer - NSConsumer.
                      * @param accepted - as bool.
                      */
                int acceptSubscription(NSConsumer *consumer, bool accepted);

            private:
                ::NSConsumer *getNSConsumer();

            private:
                std::string m_consumerId;
        };
    }
}
#endif /* _NS_CONSUMER_H_ */
