/* ****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

/**
 * This file contains the declaration of classes and its members related to
 * RemoteSecurityPanelDevice.
 */

#ifndef SMARTHOME_API_CLIENT_REMOTESECURITYPANEL_H_
#define SMARTHOME_API_CLIENT_REMOTESECURITYPANEL_H_

#include <SHBaseRemoteDevice.h>
#include <RemoteModeResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /**
             * This class contains a set of functions to control remote securitypanel device.
             */
            class RemoteSecurityPanelDevice: public SHBaseRemoteDevice
            {
            public:
                virtual ~RemoteSecurityPanelDevice();

                RemoteModeResource *m_remoteMode;

            protected:
                RemoteSecurityPanelDevice();
            };
        }
    }
}

#endif /* SMARTHOME_API_CLIENT_REMOTESECURITYPANEL_H_ */
