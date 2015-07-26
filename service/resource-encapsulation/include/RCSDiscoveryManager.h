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

#ifndef RCS_DISCOVERYMANAGER_H_
#define RCS_DISCOVERYMANAGER_H_

#include "RCSRemoteResourceObject.h"

namespace OIC
{
    namespace Service
    {

        /*
        * Forward Declaration of RCSAddress Class
        */
        class RCSAddress;

        /**
             * @class   RCSDiscoveryManager
             * @brief   This class contains the resource discovery method.
             *
             */
        class RCSDiscoveryManager
        {
            public:

                /**
                 *  Typedef for callback of discoverResource API
                 */
                typedef std::function< void( std::shared_ptr< RCSRemoteResourceObject>) >
                OnResourceDiscoveredCallback;

                /**
                * API for getting RCSDiscoveryManager instance.
                *
                * @return RCSDiscoveryManager - Instance of RCSDiscoveryManager class
                */
                static RCSDiscoveryManager *getInstance();

                /**
                * API for discovering the resource of Interest.
                *
                * @param address - RCSAddress object
                * @param resourceURI - uri of resource to be searched
                * @param cb - callback to obtain discovered resource
                *
                * @throw InvalidParameterException : This API throws the InvalidParameterException if any of
                *                                                         the parameter is invalid.
                * @RCSAddress
                */
                void discoverResource(const RCSAddress &address, const std::string &resourceURI,
                                      OnResourceDiscoveredCallback cb);
            private:

                /**
                 * Constructor for RCSDiscoveryManager.
                */
                RCSDiscoveryManager() = default;

                /**
                * Destructor for RCSDiscoveryManager.
                */
                ~RCSDiscoveryManager() = default;

        };
    }
}
#endif //RCS_DISCOVERYMANAGER_H_