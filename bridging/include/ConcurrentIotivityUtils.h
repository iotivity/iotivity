//******************************************************************
//
// Copyright 2017 Intel Mobile Communications GmbH All Rights Reserved.
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
//


#ifndef _CONCURRENTIOTIVITYUTILS_H_
#define _CONCURRENTIOTIVITYUTILS_H_

#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <thread>
#include <unistd.h>
#include <iostream>
#include <string>
#include <memory>
#include <map>
#include "IotivityWorkItem.h"
#include "WorkQueue.h"
#include "ocstack.h"
#include "octypes.h"

namespace OC
{
    namespace Bridging
    {
        template<typename T, typename... Args>
        std::unique_ptr<T> make_unique(Args &&... args)
        {
            return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
        }

        /**
         * Provides a synchronized C++ wrapper over the Iotivity CSDK.
         * Accepts workItems from the plugins for common operations.
         * A consumer thread processes these worker items and makes calls into Iotivity.
         * Another thread calls OCProcess() to check for network requests.
         */
        class ConcurrentIotivityUtils
        {
            private:

                static std::unique_ptr<WorkQueue<std::unique_ptr<IotivityWorkItem>>> m_queue;
                std::mutex m_iotivityApiCallMutex;

                std::thread m_processWorkQueueThread, m_ocProcessThread;
                bool m_threadStarted;
                bool m_shutDownOCProcessThread;
                static const int OCPROCESS_SLEEP_MICROSECONDS = 200000;

                // Fetches work item from queue and processes it.
                void processWorkQueue()
                {
                    while (true)
                    {
                        std::unique_ptr<IotivityWorkItem> workItem;
                        bool fetchedWorkItem = m_queue->get(&workItem);

                        if (fetchedWorkItem)
                        {
                            std::lock_guard<std::mutex> lock(m_iotivityApiCallMutex);
                            workItem->process();
                        }
                        else
                        {
                            break;
                        }
                    }
                }

                void callOCProcess()
                {
                    while (!m_shutDownOCProcessThread)
                    {
                        {
                            std::lock_guard<std::mutex> lock(m_iotivityApiCallMutex);
                            OCProcess();
                        }
                        // Hopefully it's enough for other threads to be scheduled
                        // instead of the spin here. OCProcess is very lightweight though.
                        usleep(OCPROCESS_SLEEP_MICROSECONDS);
                    }
                }

            public:


                ConcurrentIotivityUtils(std::unique_ptr<WorkQueue<std::unique_ptr<IotivityWorkItem>>>
                                        queueToMonitor)
                {
                    m_queue = std::move(queueToMonitor);
                    m_threadStarted = false;
                    m_shutDownOCProcessThread = false;
                }

                /**
                 * Starts 2 worker threads. One to service the concurrent work queue to call
                 * into Iotivity. One to process network requests by calling OCProcess()
                 */
                void startWorkerThreads();

                /**
                 * Stops the 2 worker threads started by startWorkerThreads. @see startWorkerThreads
                 */
                void stopWorkerThreads();

                /**
                 * Gets the string uri associated with an Iotivity handle.
                 * @warning This function is not thread safe and should only be called from entityHandler
                 *          specified when creating the resource. The entityhandler is called with the
                 *          Iotivity access mutex locked and this function does not modify anything
                 *          in the stack.
                 *
                 * @param[in]  handle    handle for the resource
                 * @param[out] uri       uri associated with the handle
                 *
                 * @return true if the resource is found and uri will be populated, else false.
                 */
                bool static getUriFromHandle(OCResourceHandle handle, std::string &uri);

                /**
                 * Sends out OBSERVE notifications for the resource with the given uri.
                 * Notifications are sent out using OC_NA_QOS.
                 *
                 * @param[in] resourceUri      resource uri for fetching handle and notifying
                 *
                 * @return OCStackResult OC_STACK_OK on success, some other value upon failure.
                 */
                OCStackResult static queueNotifyObservers(const std::string &resourceUri);

                /**
                 * Create an Iotivity resource with the given properties.
                 *
                 * @param[in] uri
                 * @param[in] resourceType
                 * @param[in] interface
                 * @param[in] entityHandler  Callback function that will be called on requests to this resource.
                 * @param[in] callbackParam
                 * @param[in] resourceProperties
                 *
                 * @return OCStackResult OC_STACK_OK on success, some other value upon failure.
                 */
                OCStackResult static
                queueCreateResource(const std::string &uri, const std::string &resourceType,
                               const std::string &interface,
                               OCEntityHandler entityHandler, void *callbackParam,
                               uint8_t resourceProperties);

                /**
                 * Delete the Iotivity resource given in the uri.
                 *
                 * @param[in] uri
                 *
                 * @return OCStackResult OC_STACK_OK on success, some other value upon failure.
                 */
                OCStackResult static queueDeleteResource(const std::string &uri);

                /**
                 * Send a response to a request.
                 *
                 * @param[in] request OCEntityHandleRequest type that was handed in the entityhandler.
                 * @param[in] payload The response payload. This is cloned and callee still has ownership
                 *                and the onus to free this.
                 * @param[in] responseCode The response code of type OCEntityHandlerResult in ocstack.h
                 *
                 * @return OCStackResult OC_STACK_OK on success, some other value upon failure.
                 */
                OCStackResult static
                respondToRequest(OCEntityHandlerRequest *request, OCRepPayload *payload,
                                 OCEntityHandlerResult responseCode);

                /**
                 * Respond with an error message. Internally calls
                 * ConcurrentIotivityUtils::respondToRequest() after creating
                 * a payload containing the error message. The error message
                 * key will be x.org.iotivity.error and the value will be
                 * errorMessage.
                 *
                 * @param[in] request EntityHandler request
                 * @param[in] errorMessage May be NULL.
                 * @param[in] errorCode entity handler result
                 *
                 * @return OCStackResult OC_STACK_OK on success, some other value upon failure.
                 */
                OCStackResult static respondToRequestWithError(OCEntityHandlerRequest *request,
                        const std::string &errorMessage,
                        OCEntityHandlerResult errorCode);

                /**
                 * Parse the query parameter as a keyValueMap
                 *
                 * @param[in] query query to be parsed
                 * @param[in,out] keyValueMap key value map of the query
                 */
                void static getKeyValueParams(const std::string &query,
                                              std::map<std::string, std::string> &keyValueMap);

                /**
                 * Can be called from the entity handler to handle requests for default interface
                 *
                 * @param[in] query
                 *
                 * @return true if request for default interface (oic.if.baseline) else false.
                 */
                bool static isRequestForDefaultInterface(const std::string &query);

        };
    }
}

#endif //_CONCURRENTIOTIVITYUTILS_H_
