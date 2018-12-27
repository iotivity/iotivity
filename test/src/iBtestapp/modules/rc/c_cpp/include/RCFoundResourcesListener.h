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
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#ifndef INCLUDE_TESTAPP_FOUNDRESOURCELISTENERHELPER_H
#define INCLUDE_TESTAPP_FOUNDRESOURCELISTENERHELPER_H

#include <iostream>
#include <string>
#include <map>
#include <cstdlib>
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include "RCSDiscoveryManager.h"
#include "RCSRemoteResourceObject.h"
#include "RCSResourceAttributes.h"
#include "RCSResourceObject.h"
#include "RCSAddress.h"
#include "OCPlatform.h"
#include "OCApi.h"
#include "RCClient.h"
#include "RCServer.h"
#include "IBtestappCommonUtil.h"

using namespace OC;
using namespace OIC::Service;

class RCFoundResourcesListener
{
private:
    static RCFoundResourcesListener* s_rcFoundResourceListener;
    static std::mutex s_mutexFoundListener;
public:
    RCFoundResourcesListener(void);
    ~RCFoundResourcesListener(void);

    static bool g_foundResource;

    /**
     * @brief   function for getting class instance
     * @return  class instance
     */
    static RCFoundResourcesListener* getInstance(void);

    /**
     * @brief               foundResource callback function invoked when found resources is returned by the Resource Container
     */
    static void rcFoundResourceCB(std::shared_ptr< OC::OCResource > resource);
};

#endif
