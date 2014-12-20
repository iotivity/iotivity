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

#if !defined(_CDContext_H)
#define _CDContext_H

#include "IContext.h"
#include "ControlleeManager.h"
#include "CDServer.h"
#include <string>

namespace OC
{

    /**
     * @class   CDContext
     * @brief   This class defines the Context where the Controlled Device has started. This class holds all major class instances.
     */
    class CDContext: public OC::Cm::IContext
    {
        public:
            /**
             * CDContext - Constructor
             *
             * @param p_cm  Represents the ControlleeManager Class instance for this context.
             */
            CDContext(OC::ControlleeManager *p_cm);

            /**
             * CDContext - Constructor
             *
             * @param p_cm  Represents the ControlleeManager Class instance for this context.
             */
            CDContext();
            /**
             *~CDContext - Destructor
             */
            ~CDContext();

            /**
             * init - Initializer
             * @param -none
             * @return -none
             *
             */
            void init();

            /**
             * getSubscriptionManager - Returns the Subscription Manager class instance which is in this context.
             * @param -none
             * @return  type:ISubscriptionManager* Returns the Subscription Manager Class instance which is responsible under this Context.
             */
            virtual OC::Cm::Notification::ISubscriptionManager *getSubscriptionManager();

            OC::Cm::Notification::ISubscriptionManager *p_subscriptionManager; /**< Represents the Subscription Manager Class instance which is responsible under this Context. */
            OC::ControlleeManager *p_cm;/**< Represents the ControlleeManager Class instance which is responsible under this Context. */
            OC::CDServer *p_server;/**<  Represents the Server that is handling resource interactions under this Context */
            std::string m_deviceAddress;/**< Represents the device address which is under this Context.*/
            OC::Cm::Device *m_pMyDevice;/**<Represents the Device Class instance which is under this Context.*/
    };

}

#endif  //_CDContext_H
