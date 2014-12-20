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

#ifndef SUBSCRIPTIONRECORD_H_
#define SUBSCRIPTIONRECORD_H_

#include "string"

namespace OC
{
    namespace Cm
    {
        namespace Notification
        {

            /**
             * @class       SubscriptionRecord
             * @brief       This is an class holds data which is related to subscription
             */
            class SubscriptionRecord
            {

                public:

                    /**
                     * Default constructor
                     */
                    SubscriptionRecord();

                    /**
                     * Default constructor
                     */
                    virtual ~SubscriptionRecord();

                    std::string m_deviceId ;            /**< Represents device identifier */

                    std::string m_subscribedResource ;  /**< Represents name of the subscription resource */

                    std::string m_subscriptionUri ;     /**< Represents the subscription uri */

                    std::string m_noificationUri ;      /**< Represents the notification uri */

                    int64_t     m_rowId ;               /**< Represents the row identifier */
            };

        } /* namespace Notification */
    } /* namespace Cm */
} /* namespace OC */
#endif /* SUBSCRIPTIONRECORD_H_ */
