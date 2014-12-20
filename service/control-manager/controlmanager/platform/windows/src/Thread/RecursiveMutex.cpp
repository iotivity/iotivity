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



#include <boost/thread/recursive_mutex.hpp>
#include "Thread/RecursiveMutex.h"
#include "Log.h"

namespace OC
{
    namespace Cm
    {
        namespace Platform
        {
            namespace Thread
            {

                /**
                * @fn          : RecursiveMutex
                * @brief       : Create recursive_mutex
                * @Assumptions :
                * @input       :
                * @output      : NA
                * @logic       : NA
                * @return      :
                * @since       : 01/08/2014
                *
                * @author      : Seetaram Rathod
                *
                * @version     : 1
                **/

                RecursiveMutex::RecursiveMutex()
                {
                    p_platformData = new boost::recursive_mutex();
                }

                /**
                * @fn          : ~RecursiveMutex
                * @brief       : Destroy recursive_mutex
                * @Assumptions :
                * @input       :
                * @output      : NA
                * @logic       : NA
                * @return      :
                * @since       : 01/08/2014
                *
                * @author      : Seetaram Rathod
                *
                * @version     : 1
                **/

                RecursiveMutex::~RecursiveMutex()
                {
                    boost::recursive_mutex *p_recursiveMutex = (boost::recursive_mutex *) p_platformData;
                    if (NULL != p_recursiveMutex)
                    {
                        delete p_recursiveMutex;
                    }
                }

                /**
                * @fn          : lock
                * @brief       : lock the recursive_mutex.
                * @Assumptions :
                * @input       :
                * @output      : NA
                * @logic       : NA
                * @return      :
                * @since       : 01/08/2014
                *
                * @author      : Seetaram Rathod
                *
                * @version     : 1
                **/

                void RecursiveMutex::lock()
                {
                    /* boost::recursive_mutex implements the Lockable concept to provide an exclusive-ownership recursive mutex.
                     * At most one thread can own the lock on a given instance of boost::recursive_mutex at any time.
                     * Multiple concurrent calls to lock()and unlock() shall be permitted.
                     * A thread that already has exclusive ownership of a given boost::recursive_mutex instance can
                     * call lock() or try_lock() to acquire an additional level of ownership of the mutex.
                    */

                    boost::recursive_mutex *p_recursiveMutex = (boost::recursive_mutex *) p_platformData;
                    p_recursiveMutex->lock();

                    CM_LOG_INFO_A(CM_PAL_THREAD, "%s", "Locked the RecursiveMutex.");
                }

                /**
                * @fn          : unlock
                * @brief       : unlock the recursive_mutex
                * @Assumptions :
                * @input       :
                * @output      : NA
                * @logic       : NA
                * @return      :
                * @since       : 01/08/2014
                *
                * @author      : Seetaram Rathod
                *
                * @version     : 1
                **/

                void
                RecursiveMutex::unlock()
                {
                    /*unlock() must be called once for each level of ownership acquired by a single thread before ownership can be acquired by another thread. */

                    boost::recursive_mutex *p_recursiveMutex = (boost::recursive_mutex *) p_platformData;
                    p_recursiveMutex->unlock();

                    CM_LOG_INFO_A(CM_PAL_THREAD, "%s", "unlocked the RecursiveMutex.");
                }

                /**
                * @fn          : getPlatformData
                * @brief       : Return the recursive_mutex object pointer
                * @Assumptions :
                * @input       :
                * @output      : NA
                * @logic       : NA
                * @return      :
                * @since       : 01/08/2014
                *
                * @author      : Seetaram Rathod
                *
                * @version     : 1
                **/

                void *
                RecursiveMutex::getPlatformData() const
                {
                    return p_platformData;
                }

            }
        }
    }
}
