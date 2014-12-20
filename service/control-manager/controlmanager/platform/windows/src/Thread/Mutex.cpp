//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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



#include <boost/thread/mutex.hpp>
#include "Thread/Mutex.h"

namespace OC
{
    namespace Cm
    {
        namespace Platform
        {
            namespace Thread
            {


                Mutex::Mutex()
                {
                    p_platformData = new boost::mutex();
                }

                Mutex::~Mutex()
                {
                    boost::mutex *p_mutex = (boost::mutex *) p_platformData;
                    if (NULL != p_mutex)
                    {
                        delete p_mutex;
                    }
                }


                bool Mutex::lock()
                {
                    boost::mutex *p_mutex = (boost::mutex *) p_platformData;
                    p_mutex->lock();

                    return true;
                }

                bool
                Mutex::unlock()
                {
                    boost::mutex *p_mutex = (boost::mutex *) p_platformData;
                    p_mutex->unlock();

                    return true;
                }

                void *
                Mutex::getPlatformData() const
                {
                    return p_platformData;
                }


            }
        }
    }
}
