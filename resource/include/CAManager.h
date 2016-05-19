/* ****************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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

#ifndef CA_MANAGER_H_
#define CA_MANAGER_H_

#include <OCApi.h>

namespace OC
{
    /**
    * This namespace contains the main entrance/functionality to monitoring network changes.
    * It may be used with OC::CAManager::functionName. To set a custom callback function,
    * the implementer must make a call to CAManager::setNetworkMonitorHandler.
    */
    namespace CAManager
    {
        // typedef to get adapter status changes from CA.
        typedef std::function<void(OCTransportAdapter,
                                   const std::string&, bool)> ConnectionChangedCallback;

        // typedef to get connection status changes from CA.
        typedef std::function<void(OCTransportAdapter, bool)> AdapterChangedCallback;

        /**
        * Set network monitoring handler.
        * @param adapterHandler adapter state change handler to handle changes for
        *                       any transport types.
        * @param connectionHandler connection state change handler to handle changes for
        *                          connection with remote devices.
        * @return Returns ::OC_STACK_OK if success.
        */
        OCStackResult setNetworkMonitorHandler(AdapterChangedCallback adapterHandler,
                                               ConnectionChangedCallback connectionHandler);
    }
}

#endif // CA_MANAGER_H_



