//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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
 * This file contains the RCSDiscoveryManager class which provide API to discover the Resource in the network
 *
 */

#ifndef RCSDISCOVERYMANAGER_H
#define RCSDISCOVERYMANAGER_H

#include <memory>
#include <functional>

namespace OIC
{
    namespace Service
    {

        class RCSRemoteResourceObject;
        class RCSAddress;

        /**
         * This class contains the resource discovery method.
         *
         * @see RCSRemoteResourceObject
         */
        class RCSDiscoveryManager
        {
        public:

            /**
             * Typedef for callback of discoverResource API
             *
             * @see discoverResource
             */
            typedef std::function< void(std::shared_ptr< RCSRemoteResourceObject >) >
                ResourceDiscoveredCallback;

            /**
             * Returns RCSDiscoveryManager instance.
             *
             */
            static RCSDiscoveryManager* getInstance();

            /**
             * API for discovering the resource of Interest.
             *
             * @param address A RCSAddress object
             * @param resourceURI The uri of resource to be searched
             * @param cb A callback to obtain discovered resource
             *
             * @throws InvalidParameterException If cb is empty.
             *
             * @note The callback will be invoked in an internal thread.
             *
             * @see RCSAddress
             *
             */
            void discoverResource(const RCSAddress& address, const std::string& resourceURI,
                    ResourceDiscoveredCallback cb);

        private:
            RCSDiscoveryManager() = default;
            ~RCSDiscoveryManager() = default;

        };
    }
}
#endif // RCSDISCOVERYMANAGER_H
