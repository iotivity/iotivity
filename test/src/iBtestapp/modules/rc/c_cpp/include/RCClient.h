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

#ifndef INCLUDE_TESTAPP_RCClientHELPER_H
#define INCLUDE_TESTAPP_RCClientHELPER_H

#include <iostream>
#include <signal.h>
#include <string>
#include <map>
#include <cstdlib>
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <vector>
#include "RCSDiscoveryManager.h"
#include "RCSRemoteResourceObject.h"
#include "RCSResourceAttributes.h"
#include "RCSResourceObject.h"
#include "RCSAddress.h"
#include "OCPlatform.h"
#include "OCApi.h"
#include "RCSResourceContainer.h"
#include "RCSBundleInfo.h"
#include "RCFoundResourcesListener.h"
#include "RCServer.h"
#include "IBtestappCommonUtil.h"

using namespace OC;
using namespace OIC;
using namespace OIC::Service;
namespace PH = std::placeholders;

class RCClient
{
private:
    static RCClient* s_RCClient;
    std::unique_ptr< RCSDiscoveryManager::DiscoveryTask > discoveryTask;
    const std::string relativetUri;
    const std::vector< std::string > resourceTypes;
    static std::mutex s_mutexRcClient;

public:
    RCClient(void);
    ~RCClient(void);

    /**
     * @brief   function for getting class instance
     * @return  class instance
     */
    static RCClient* getInstance(void);

    /**
     * @brief       This function will check that container is started or not
     */
    void rcObserveResourceContainer(RCSResourceContainer *container,
            std::vector< std::string > vecResourceTypes);
};

#endif
