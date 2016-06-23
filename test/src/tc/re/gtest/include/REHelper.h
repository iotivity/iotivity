//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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

#ifndef INCLUDE_TESTCASE_RE_GTEST_REHELPER_H_
#define INCLUDE_TESTCASE_RE_GTEST_REHELPER_H_

#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <mutex>
#include <algorithm>
#include "ocstack.h"
#include "octypes.h"
#include "gtest_custom.h"
#include "OCPlatform.h"
#include "CommonUtil.h"
#include "IotivityTest_Logger.h"
#include "RCSAddress.h"
#include "RCSDiscoveryManager.h"
#include "RCSException.h"
#include "RCSRemoteResourceObject.h"
#include "RCSRequest.h"
#include "RCSResourceAttributes.h"
#include "RCSResourceObject.h"
#include "RCSResponse.h"
#include "RCSRepresentation.h"

using namespace OC;
using namespace OIC;
using namespace OIC::Service;
using namespace std;
namespace PH = std::placeholders;

#define LIGHT_1_URI "/device/light-1"
#define LIGHT_URI "/device/tube-light"
#define FAN_URI "/device/table-fan"
#define DEFAULT_LIGHT_INTERFACE "oic.if."
#define RESOURCE_TYPE_FAN "core.fan"
#define RESOURCE_TYPE_LIGHT "core.light"
#define DEFAULT_POWER_KEY "power"
#define DEFAULT_EMPTY_KEY ""
#define DEFAULT_POWER_VALUE_ON "on"
#define DEFAULT_POWER_VALUE "off"
#define DEFAULT_INTENSITY_KEY "intensity"
#define DEFAULT_INTENSITY_VALUE "10"
#define DEFAULT_TEMPETATURE_KEY "Temperature"
#define DEFAULT_TEMPETATURE_VALUE "30"

#define RE_SERVER_APP "./retestserver"

const int CALLBACK_WAIT_DEFAULT = 5;
const int CALLBACK_WAIT_MAX = 10;
const int CALLBACK_WAIT_MIN = 1;
const int CALLBACK_WAIT_NONE = 0;
const int SUCCESS_RESPONSE = 0;

class REHelper
{
private:
    static REHelper* s_reHelperInstance;
    static std::mutex s_mutex;
    vector< shared_ptr< RCSRemoteResourceObject > > m_primitiveResourceList;

    std::string m_errorMessage;
    std::unique_ptr< RCSDiscoveryManager::DiscoveryTask > discoveryTask;
    void OnPrimitiveResourceDiscovered(shared_ptr< RCSRemoteResourceObject > pResource);
public:
    RCSDiscoveryManager* m_pDiscoveryManager;
	REHelper();
    virtual ~REHelper();

    /**
     * @brief   Function is for getting singleton instance of REHelper
     * @return  singleton instance of REHelper
     */
    static REHelper* getInstance(void);

    /**
     * @brief   Function is for finding primitive Resource
     * @return  true if successfully found resource
     */
    bool findPrimitiveResources(string resourceType);

    /**
     * @brief   Function is for getting found Resource
     * @return  Found Resource List
     */
    vector< shared_ptr< RCSRemoteResourceObject > > getFoundResourceList(void);

    /**
     * @brief   Waiting method for Resource Found
     * @return  True if Resource found, False if Resource is not found.
     */
    bool waitForResourceFound(
            std::vector< shared_ptr< RCSRemoteResourceObject > > &foundResourceList, int timeOut);

    /**
     * @brief   Function is for getting Failure Message
     * @return  error message
     */
    std::string getFailureMessage();

    /**
     * @brief   Function is for getting Resource State
     * @return  Resource State
     */
    std::string getResourceState(ResourceState resourceState);

    /**
     * @brief   Function is for getting Cache State
     * @return  Cache State
     */
    std::string getCacheState(CacheState cacheState);

};

#endif /* INCLUDE_TESTCASE_RE_GTEST_REHELPER_H_ */
