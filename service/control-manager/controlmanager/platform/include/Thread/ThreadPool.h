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



#if !defined(_THREADPOOL_H)
#define _THREADPOOL_H

#include "CMPALConfig.h"
#include "IRunnable.h"

namespace OC
{
    namespace Cm
    {
        namespace Platform
        {
            namespace Thread
            {


                class CMPAL_API ThreadPool
                {

                    public:

                        ThreadPool(int poolSize) ;

                        ~ThreadPool() ;

                        bool runTask(OC::Cm::Platform::Thread::IRunnable &task);



                        void *getPlatformData() const;

                    private:

//  void *p_platformData ;

                };



            }
        }
    }
}

#endif  //_THREADPOOL_H
