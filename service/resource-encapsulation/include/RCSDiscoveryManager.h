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
 * This file contains the RCSDiscoveryManager class which provide APIs to discover the Resource in the network
 *
 */

#ifndef RCSDISCOVERYMANAGER_H
#define RCSDISCOVERYMANAGER_H

#include <memory>
#include <functional>

#include "octypes.h"

namespace OIC
{
    namespace Service
    {
        class RCSRemoteResourceObject;
        class RCSAddress;

        /**
         * This class contains the resource discovery methods.
         *
         * @see RCSRemoteResourceObject
         */
        class RCSDiscoveryManager
        {
            public:
                class DiscoveryTask
                {
                    public:

                        void cancel();
                        bool isCanceled();
                        ~DiscoveryTask();

                    public:

                        DiscoveryTask(const DiscoveryTask&) = delete;
                        DiscoveryTask(DiscoveryTask&&) = delete;
                        DiscoveryTask& operator = (const DiscoveryTask&) const = delete;
                        DiscoveryTask& operator = (DiscoveryTask&&) const = delete;

                    private:

                        explicit DiscoveryTask(unsigned int id) : m_id{ id } {};
                    private:

                        unsigned int m_id;
                        friend class RCSDiscoveryManagerImpl;
                };
            public:

                /**
                 * Typedef for callback of discoverResource APIs
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
                 * API for discovering the resource of Interest, regardless of URI and resource type
                 *
                 * @param address A RCSAddress object
                 * @param cb A callback to obtain discovered resource
                 *
                 * @throws InvalidParameterException If cb is empty.
                 * @throws PlatformException If the operation failed.
                 *
                 * @note The callback will be invoked in an internal thread.
                 *
                 * @see RCSAddress
                 *
                 */
                std::unique_ptr<DiscoveryTask> discoverResource(const RCSAddress& address,
                        ResourceDiscoveredCallback cb);

                /**
                 * API for discovering the resource of Interest, regardless of resource type
                 *
                 * @param address A RCSAddress object
                 * @param relativeURI The relative uri of resource to be searched
                 * @param cb A callback to obtain discovered resource
                 *
                 * @throws InvalidParameterException If cb is empty.
                 * @throws PlatformException If the operation failed.
                 *
                 * @note The callback will be invoked in an internal thread.
                 *
                 * @see RCSAddress
                 *
                 */
                std::unique_ptr<DiscoveryTask> discoverResource(const RCSAddress& address,
                        const std::string& relativeURI, ResourceDiscoveredCallback cb);

                /**
                 * API for discovering the resource of Interest by Resource type.
                 *
                 * @param address A RCSAddress object
                 * @param resourceType Ressource Type
                 * @param cb A callback to obtain discovered resource
                 *
                 * @throws InvalidParameterException If cb is empty.
                 * @throws PlatformException If the operation failed.
                 *
                 * @note The callback will be invoked in an internal thread.
                 *
                 * @see RCSAddress
                 *
                 */
                std::unique_ptr<DiscoveryTask> discoverResourceByType(const RCSAddress& address,
                        const std::string& resourceType, ResourceDiscoveredCallback cb);

                /**
                 * API for discovering the resource of Interest by Resource type with provided relativeURI
                 *
                 * @param address A RCSAddress object
                 * @param relativeURI The relative uri of resource to be searched
                 * @param resourceType Ressource Type
                 * @param cb A callback to obtain discovered resource
                 *
                 * @throws InvalidParameterException If cb is empty.
                 * @throws PlatformException If the operation failed.
                 *
                 * @note The callback will be invoked in an internal thread.
                 *
                 * @see RCSAddress
                 *
                 */
                std::unique_ptr<DiscoveryTask>  discoverResourceByType(const RCSAddress& address,
                        const std::string& relativeURI, const std::string& resourceType,
                        ResourceDiscoveredCallback cb);

            private:

                RCSDiscoveryManager() = default;
                ~RCSDiscoveryManager()= default;;

                friend class DiscoveryTask;
        };
    }
}
#endif // RCSDISCOVERYMANAGER_H
