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



#ifndef ICONTEXT_H_
#define ICONTEXT_H_

#include "CMConfig.h"

namespace OC
{
    namespace Cm
    {
        namespace Notification
        {
            class ISubscriptionManager;
        }
        /**
        *  @addtogroup ControlManager
        *  @{
        */


        /**
         * @class       IContext
         * @brief       This class represents context of Control Manager framework. Control Manager framework supports multiple instances.
         *              Control Manager Framework stores the information related to particular instance of framework in IContext.
         * @since       1.0
         */
        class CMAPI IContext
        {
            public:
                /**
                 * Default destructor of this class
                 */
                virtual ~IContext() {}


                /**
                 * Returns an instance of subscription manager which is interface to handle subscription and notification related events
                 *
                 * @return Returns an object of subscription manager class
                 */
                virtual OC::Cm::Notification::ISubscriptionManager *getSubscriptionManager() = 0 ;
        };
        /** @} */
    }
}


#endif /* ICONTEXT_H_ */
