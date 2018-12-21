/******************************************************************
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

/**
 * @file request_automation_manager.h
 *
 * @brief This file provides class for managing auto request generation sessions.
 *
 */

#ifndef SIMULATOR_REQUEST_AUTOMATION_MNGR_H_
#define SIMULATOR_REQUEST_AUTOMATION_MNGR_H_

#include <mutex>
#include <unordered_map>

#include "request_generation.h"

/**
 * OC namespace
 */
namespace OC
{
    class OCResource;
}

/**
 * @class RequestModel
 */
class RequestModel;
/**
 * @class RequestAutomationMngr
 */
class RequestAutomationMngr
{
    public:
        /**
         * API to request automation manager
         * @param[in] ocResource     oc resource
         */
        RequestAutomationMngr(const std::shared_ptr<OC::OCResource> &ocResource);

        /**
         * API to start the get request
         * @param[in] requestSchema   request schema
         * @param[in] callback        callback function
         */
        int startOnGET(const std::shared_ptr<RequestModel> &requestSchema,
                       RequestGeneration::ProgressStateCallback callback);

        /**
         * API to start the put request
         * @param[in] requestSchema   request schema
         * @param[in] callback        callback function
         */
        int startOnPUT(const std::shared_ptr<RequestModel> &requestSchema,
                       RequestGeneration::ProgressStateCallback callback);

        /**
         * API to start the post request
         * @param[in] requestSchema   request schema
         * @param[in] callback        callback function
         */
        int startOnPOST(const std::shared_ptr<RequestModel> &requestSchema,
                        RequestGeneration::ProgressStateCallback callback);

        /**
         * API to stop the request for the given id
         * @param[in] id    request identity
         */
        void stop(int id);

    private:
        void onProgressChange(int sessionId, OperationState state,
                              RequestGeneration::ProgressStateCallback clientCallback);
        bool isValid(int id);
        bool isInProgress(RequestType type);
        void remove(int id);

        std::mutex m_lock;
        std::unordered_map<int, std::shared_ptr<RequestGeneration>> m_requestGenList;
        int m_id;
        std::shared_ptr<OC::OCResource> m_ocResource;
};

#endif
