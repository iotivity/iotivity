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
#include "ThreadImpl.h"

namespace OC
{
    namespace Cm
    {
        namespace Platform
        {
            namespace Thread
            {



                void thread_worker_function(OC::Cm::Platform::Thread::IRunnable *target)
                {
                    try
                    {
                        target->run();
                    }
                    catch (boost::thread_interrupted e)
                    {

                    }
                }

#if 0
                class ThreadWorker
                {
                    public:
                        OC::Cm::Platform::Thread::Thread *thread ;

                        ThreadWorker(OC::Cm::Platform::Thread::Thread *thread)
                        {
                            this->thread = thread;
                        }

                        void operator()()
                        {
                            this->thread->run();
                        }
                };
#endif


                ThreadImpl::ThreadImpl(OC::Cm::Platform::Thread::IRunnable *target)
                {
                    p_target = target;
                    p_thread = NULL ;
                }

                ThreadImpl::~ThreadImpl()
                {
                    if (p_thread != NULL)
                    {
                        delete p_thread;
                        p_thread = NULL ;
                    }

                    p_target = NULL;
                }


                bool
                ThreadImpl::start()
                {
#if 0
                    ThreadWorker *worker = new ThreadWorker(this);
                    p_platformData = new boost::thread(*worker);
#endif

                    p_thread = new boost::thread(thread_worker_function, p_target);

                    return true;
                }


                void
                ThreadImpl::join()
                {
                    if (p_thread != NULL)
                    {
                        p_thread->join();
                    }
                }

                void
                ThreadImpl::interrupt()
                {
                    if (p_thread != NULL)
                    {
                        p_thread->interrupt();
                    }
                }



            } /* namespace Thread */
        } /* namespace Platform */
    } /* namespace Cm */
} /* namespace OC */
