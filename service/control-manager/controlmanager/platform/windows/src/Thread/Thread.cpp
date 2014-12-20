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



#include <boost/date_time.hpp>

#include "Thread/Thread.h"
#include "ThreadImpl.h"

namespace OC
{
    namespace Cm
    {
        namespace Platform
        {
            namespace Thread
            {

                Thread *
                Thread::create(OC::Cm::Platform::Thread::IRunnable *target)
                {
                    return new OC::Cm::Platform::Thread::ThreadImpl(target);
                }


                void
                Thread::sleep(uint64_t microSeconds)
                {
                    boost::this_thread::sleep(boost::posix_time::microseconds(microSeconds));
                }
                void
                Thread::AddThreadExitCallBack( PALThradExitCallBack exitCallBack)
                {
                    boost::this_thread::at_thread_exit< PALThradExitCallBack >(exitCallBack) ;
                }

                Thread::~Thread()
                {

                }


            }
        }
    }
}
