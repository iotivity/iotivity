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
 *      LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/

#ifndef INCLUDE_TESTCASE_NS_GTEST_NSCONSUMER_HELPER_H_
#define INCLUDE_TESTCASE_NS_GTEST_NSCONSUMER_HELPER_H_

#include <algorithm>
#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <mutex>
#include <string>

#include "ocstack.h"
#include "octypes.h"
#include "oic_string.h"
#include "gtest_custom.h"
#include "gtest/gtest.h"
#include "OCPlatform.h"
#include "CommonUtil.h"
#include "IotivityTest_Logger.h"

#include "NSCommon.h"
#include "NSConsumerInterface.h"

#define WAIT_TIME_MIN 1
#define WAIT_TIME_DEFAULT 2
#define WAIT_TIME_MAX 10

#define PROVIDER_APP_ALLOW "./ns_provider_simulator 0"
#define PROVIDER_APP_DENY "./ns_provider_simulator 1"

using namespace OC;
using namespace std;
namespace PH = std::placeholders;

class NSConsumerHelper
{
private:
    static NSConsumerHelper* s_nsHelperInstance;
    static std::mutex s_mutex;
    static NSProvider *s_pProvider;
    void waitForProvider(int time);

public:
    NSConsumerHelper();
    ~NSConsumerHelper();

    /**
     * @brief   Function is for getting singleton instance of NSConsumerHelper
     * @return  singleton instance of NSConsumerHelper
     */
    static NSConsumerHelper* getInstance(void);

    /**
     * @brief   Callback function for ProviderChanged
     */
    static void onProviderChanged(NSProvider * provider, NSProviderState response);

    /**
     * @brief   Callback function for Provider Notification Posted
     */
    static void onNotificationPosted(NSMessage * notification);

    /**
     * @brief   Callback function for Provider Sync
     */
    static void onNotificationSync(NSSyncInfo * sync);

    /**
     * @brief   Construct consumer config
     * @return  Consumer configuration
     */
    NSConsumerConfig getConsumerConfig();

    /**
     * @brief   Get provider from callback
     * @return  NSProvider
     */
    NSProvider* getProvider();

    /**
     * @brief   Get the result string from NSResult enum
     * @param   NSResult
     * @return  NSResult as string
     */
    string getResultString(NSResult result);
};

#endif /* INCLUDE_TESTCASE_NS_GTEST_NSCONSUMER_HELPER_H_ */
