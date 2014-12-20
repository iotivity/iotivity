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



#ifndef ISUBSCRIPTIONPROVIDER_H_
#define ISUBSCRIPTIONPROVIDER_H_

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
             * @interface   ISubscriptionProvider
             * @brief       Defines callback methods which specifies whether framework should allow a subscription request or not
             *
             * @since       1.0
             *
             * This interface defines methods which specifies whether framework should allow a subscription request or not
             *
             */
            class CMAPI ISubscriptionProvider
            {
                public:

                    /**
                     * Default destructor
                     */
                    virtual ~ISubscriptionProvider() {};

                    /**
                     * returns a value which specifies whether to allow a subscription request or not
                     *
                     * @param[in] device device from which this request is received
                     * @param[in] resourcePath resource path to which subscription request is received
                     *
                     * @return  returns @c true if request is sent successfully @n
                     *                  @c false in case of any error
                     *
                     */
                    virtual bool shouldAllowSubscriptionRequest( OC::Cm::Device &device,
                            std::string &resourcePath ) = 0 ;
            };
            /** @} */
        }
    }
}


#endif /* ISUBSCRIPTIONPROVIDER_H_ */
