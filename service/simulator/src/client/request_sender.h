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

#ifndef SIMULATOR_REQUEST_SENDER_H_
#define SIMULATOR_REQUEST_SENDER_H_

#include "simulator_client_types.h"
#include "simulator_resource_model.h"
#include "simulator_error_codes.h"

struct RequestInfo
{
    RequestType type;   /**< type of request */
    std::map<std::string, std::string> queryParams; /**< query parameters */
    SimulatorResourceModel payLoad;                 /**< resource payload */
};

/**
 * @class GETRequestSender
 */
class GETRequestSender
{
    public:
        typedef std::function<void (SimulatorResult result, const SimulatorResourceModel &resModel,
                                    const RequestInfo &reqInfo)> ResponseCallback;

        /**
         * send the get request
         * @param[in] ocResource   oc resource
         */
        GETRequestSender(const std::shared_ptr<OC::OCResource> &ocResource);

        SimulatorResult send(const ResponseCallback &callback);

        SimulatorResult send(const std::map<std::string, std::string> &queryParams,
                             const ResponseCallback &callback);

        /**
         * response of get request
         * @param[in] headerOptions    header options
         * @param[in] rep              oc representation
         * @param[in] errorCode        error code
         * @param[in] requestInfo      request information
         * @param[in] callback         callback function
         */
        void onResponseReceived(const OC::HeaderOptions &headerOptions,
                                const OC::OCRepresentation &rep, const int errorCode, RequestInfo &requestInfo,
                                ResponseCallback callback);

    private:
        std::shared_ptr<OC::OCResource> m_ocResource;
};

/**
 * @class PUTRequestSender
 */
class PUTRequestSender
{
    public:
        typedef std::function<void (SimulatorResult result, const SimulatorResourceModel &resModel,
                                    const RequestInfo &reqInfo)> ResponseCallback;

        /**
         * send the put request
         * @param[in] ocResource  oc resource
         */
        PUTRequestSender(const std::shared_ptr<OC::OCResource> &ocResource);

        SimulatorResult send(const SimulatorResourceModel &representation,
                             const ResponseCallback &callback);

        SimulatorResult send(const std::map<std::string, std::string> &queryParams,
                             const SimulatorResourceModel &representation,
                             const ResponseCallback &callback);

        /**
         * response of put request
         * @param[in] headerOptions    header options
         * @param[in] rep              oc representation
         * @param[in] errorCode        error code
         * @param[in] requestInfo      request information
         * @param[in] callback         callback function
         */
        void onResponseReceived(const OC::HeaderOptions &headerOptions,
                                const OC::OCRepresentation &rep, const int errorCode, RequestInfo &requestInfo,
                                ResponseCallback callback);

    private:
        std::shared_ptr<OC::OCResource> m_ocResource;
};

/**
 * @class POSTRequestSender
 */
class POSTRequestSender
{
    public:
        typedef std::function<void (SimulatorResult result, const SimulatorResourceModel &resModel,
                                    const RequestInfo &reqInfo)> ResponseCallback;

        /**
         * send the post request
         * @param[in] ocResource  oc resource
         */
        POSTRequestSender(const std::shared_ptr<OC::OCResource> &ocResource);

        SimulatorResult send(const SimulatorResourceModel &representation,
                             const ResponseCallback &callback);

        SimulatorResult send(const std::map<std::string, std::string> &queryParams,
                             const SimulatorResourceModel &representation,
                             const ResponseCallback &callback);

        /**
         * response of post request
         * @param[in] headerOptions    header options
         * @param[in] rep              oc representation
         * @param[in] errorCode        error code
         * @param[in] requestInfo      request information
         * @param[in] callback         callback function
         */
        void onResponseReceived(const OC::HeaderOptions &headerOptions,
                                const OC::OCRepresentation &rep, const int errorCode, RequestInfo &requestInfo,
                                ResponseCallback callback);

    private:
        std::shared_ptr<OC::OCResource> m_ocResource;
};

#endif
