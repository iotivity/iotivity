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

#ifndef _WORKQUEUE_H_
#define _WORKQUEUE_H_

#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <thread>
#include <unistd.h>

namespace OC
{
    namespace Bridging
    {
        template<class T>
        class WorkQueue
        {
        private:

            std::queue<T> m_workQueue;
            std::mutex m_workQueueMutex;
            std::condition_variable m_cv;
            bool m_signalToShutDown;

        public:

            inline WorkQueue()
            {
                m_signalToShutDown = false;
            }

            void put(T item)
            {
                std::unique_lock<std::mutex> lock(m_workQueueMutex);

                m_workQueue.push(std::move(item));
                m_cv.notify_all();
            }

            bool get(T *item)
            {
                std::unique_lock<std::mutex> lock(m_workQueueMutex);

                m_cv.wait(lock, [this]()
                {
                    return m_workQueue.size() > 0 || m_signalToShutDown;
                });

                if (m_signalToShutDown)
                {
                    return false;
                }

                *item = std::move(m_workQueue.front());
                m_workQueue.pop();
                return true;
            }

            void shutdown()
            {
                std::unique_lock<std::mutex> lock(m_workQueueMutex);
                m_signalToShutDown = true;
                m_cv.notify_all();
            }
        };
    }
}

#endif // _WORKQUEUE_H_