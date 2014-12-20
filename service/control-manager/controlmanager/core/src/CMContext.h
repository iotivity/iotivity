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



#if !defined(_CMContext_H)
#define _CMContext_H

#include "ControlManager.h"
#include "IContext.h"
#include "Engine/CMEngine.h"
#include "DeviceFinder/DeviceFinderImpl.h"

namespace OC
{
    namespace Cm
    {
        namespace Core
        {

            /**
             * @class   CMContext
             * @brief   This class defines the Context where Control Manager has started. This class holds all major class instances.
             */
            class CMContext : public OC::Cm::IContext
            {
                public:
                    /**
                     * Constructor
                     *
                     * @param[in] p_shp Represents the Control Manager Class instance for this context.
                     */
                    CMContext(OC::Cm::ControlManager *p_shp);

                    /**
                     * Destructor
                     */
                    ~CMContext();

                    /**
                     * Returns the Subscription Manager class instance which is in this context.
                     *
                     * @return  Returns the Subscription Manager Class instance which is responsible under this Control Manager Context.
                     */
                    virtual OC::Cm::Notification::ISubscriptionManager *getSubscriptionManager() ;

                    /**
                     * Represents the Subscription Manager Class instance which is responsible under this Control Manager Context.
                     */
                    OC::Cm::Notification::ISubscriptionManager *p_subscriptionManager ;

                    /**
                     * Represents the Device Finder Impl Class instance which is responsible under this Control Manager Context.
                     */
                    OC::Cm::Core::DeviceFinder::Impl::DeviceFinderImpl m_deviceFinderImpl ;

                    /**
                     * Represents the Control Manager Engine Class instance which is responsible under this Control Manager Context.
                     */
                    OC::Cm::Core::Engine::CMEngine  m_engine ;

                    /**
                     * Represents the Control Manager Class instance which is responsible under this Control Manager Context.
                     */
                    OC::Cm::ControlManager *p_shp ;

                    /**
                     * Represents the device address which is under this Context.
                     */
                    std::string m_deviceAddress ;

            };

        }
    }
}

#endif  //_CMContext_H
