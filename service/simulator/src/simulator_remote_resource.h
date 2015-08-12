/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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
 * @file   simulator_remote_resource.h
 *
 * @brief   This file provides a class for handling remote resources by the client.
 *
 */

#ifndef SIMULATOR_REMOTE_RESOURCE_H_
#define SIMULATOR_REMOTE_RESOURCE_H_

#include "simulator_resource_model.h"
#include "simulator_error_codes.h"

#include <string>
#include <vector>
#include "OCPlatform.h"
#include "OCApi.h"

/**
 * @class   SimulatorRemoteResource
 * @brief   This class represents the resource discovered in the network and provides APIs
 *          for sending requests to discovered resource.
 */
class SimulatorRemoteResource
{
    public:
        typedef enum
        {
            OBSERVE,
            OBSERVE_ALL
        } ObserveType;

        // Typedef for query parameter map
        typedef std::map<std::string, std::string> QueryParamsMap;

        typedef std::function<void (int, const SimulatorResourceModel &, int)>
        RepresentationChangeCallback;

        typedef std::function<void (int, const SimulatorResourceModel &)>
        ResponseCallback;

        SimulatorRemoteResource(std::shared_ptr<OC::OCResource> resource);

        std::string getURI() const;

        std::string getHost() const;

        std::string getSID() const;

        OCConnectivityType getConnectivityType() const;

        std::vector < std::string > getResourceTypes() const;

        std::vector < std::string > getResourceInterfaces() const;

        bool isObservable() const;

        SimulatorResult observe(ObserveType type,
                                const QueryParamsMap &queryParams, RepresentationChangeCallback callback);

        SimulatorResult cancelObserve();

        SimulatorResult get(const QueryParamsMap &queryParams,
                            ResponseCallback callback);

        SimulatorResult get(const std::string &resourceType, const std::string &interfaceType,
                            const QueryParamsMap &queryParams, ResponseCallback callback);

        SimulatorResult put(const SimulatorResourceModel &representation,
                            const QueryParamsMap &queryParams, ResponseCallback callback);

        SimulatorResult put(const std::string &resourceType, const std::string &interfaceType,
                            const SimulatorResourceModel &representation,
                            const QueryParamsMap &queryParams, ResponseCallback callback);

        SimulatorResult post(const SimulatorResourceModel &representation,
                             const QueryParamsMap &queryParams, ResponseCallback callback);

        SimulatorResult post(const std::string &resourceType, const std::string &interfaceType,
                             const SimulatorResourceModel &representation,
                             const QueryParamsMap &queryParams, ResponseCallback callback);

    private:

        std::shared_ptr<OC::OCResource> m_ocResource;
        std::mutex m_observeMutex;
};

typedef std::shared_ptr<SimulatorRemoteResource> SimulatorRemoteResourcePtr;
typedef std::function<void(SimulatorRemoteResourcePtr)> ResourceFindCallback;

#endif //SIMULATOR_REMOTE_RESOURCE_H_

