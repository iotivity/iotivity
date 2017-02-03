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
#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <unistd.h>
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


            // Since there is no API in Iotivity to fetch the handle from the URI,
            // we stick it in this map.
            static std::map<std::string, OCResourceHandle> m_resourceUriToHandleMap;

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
                        ConcurrentIotivityUtils::processWorkItem(std::move(workItem));
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
                    // 200ms sleep. Hopefully it's enough for other threads to be scheduled
                    // insted of the spin here. OCProcess is very lightweight though.
                    usleep(200000);
                }
            }

        public:


            ConcurrentIotivityUtils(std::unique_ptr<WorkQueue<std::unique_ptr<IotivityWorkItem>>> queueToMonitor)
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
             * Processes a fetched workitem based on the type like CREATE, RESPOND etc.
             * @param workItem unique pointer to the workItem.
             */
            void processWorkItem(std::unique_ptr<IotivityWorkItem> workItem);

            /**
             * Gets the Iotivity resource handle OCResourceHandle from the uri of the resource.
             * Uses an internal map populated at the time of resource creation since Iotivity
             * does not have APIs to fetch the handle.
             * @param uri
             * @param handle
             * @return true if resource found and handle will be populated, else false.
             */
            bool static getHandleFromUri(std::string uri, OCResourceHandle *handle);

            /**
             * Gets the string uri associated with an Iotivity handle.
             * @param handle
             * @param uri
             * @return true if the resource is found and uri will be populated, else false.
             */
            bool static getUriFromHandle(OCResourceHandle handle, std::string &uri);

            /**
             * Sends out OBSERVE notifications for the resource with the given uri.
             * Notifications are sent out using OC_NA_QOS.
             * @param resourceUri
             * @return true if task was queued successfully.
             */
            bool static notifyObservers(const std::string &resourceUri);

            /**
             * Create an Iotivity resource with the given properties.
             * @param uri
             * @param resourceType
             * @param interface
             * @param entityHandler Callback function that will be called on requests to this resource.
             * @param callbackParam
             * @param resourceProperties
             */
            void static
            createResource(const std::string &uri, const std::string &resourceType, const std::string &interface,
                           OCEntityHandler entityHandler, void *callbackParam, uint8_t resourceProperties);

            /**
             * Delete the Iotivity resource given in the uri.
             * @param uri
             */
            void static deleteResource(const std::string &uri);

            /**
             *
             * Send a response to a request.
             * @param request OCEntityHandleRequest type that was handed in the entityhandler.
             * @param payload The response payload. This is cloned and callee still has ownership and the onus to free this.
             * @param responseCode The response code of type OCEntityHandlerResult in ocstack.h
             */
            void static
            respondToRequest(OCEntityHandlerRequest *request, OCRepPayload *payload,
                             OCEntityHandlerResult responseCode);

            /**
             * Respond with an error message. Internally calls ConcurrentIotivityUtils::respondToRequest() after creating
             * a payload containing the error message. The error message key will be x.com.intel.error and the value wil be
             * \p errorMessage.
             * @param request
             * @param errorMessage May be NULL.
             * @param errorCode
             */
            void static respondToRequestWithError(OCEntityHandlerRequest *request, const std::string &errorMessage,
                                                  OCEntityHandlerResult errorCode);

            bool static doesResponseNeedFullRepresentation(OCEntityHandlerRequest *request);

        };
    }
}

#endif //_CONCURRENTIOTIVITYUTILS_H_
