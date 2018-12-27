/******************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/

#ifndef INCLUDE_TESTCASE_NS_GTEST_NSPROVIDERHELPER_H_
#define INCLUDE_TESTCASE_NS_GTEST_NSPROVIDERHELPER_H_

#include "NSCommonHelper.h"

#include "NSCommon.h"
#include "NSProviderInterface.h"

#define CONSUMER_SIMULATOR "./iotivity_ns_consumer_csdk_simulator"
#define CONSUMER_SIMULATOR_OPTION "./iotivity_ns_consumer_csdk_simulator 0"

#define TOPIC_NAME_PROVIDER "Test_Topic_1"
#define TOPIC_BODY "TESTAPP_Body_1"
#define TOPIC_TITLE "TESTAPP_Title_1"

#define CONSUMER_APP_MENU_1 "201"
#define CONSUMER_APP_MENU_2 "202"
#define CONSUMER_APP_MENU_3 "203"
#define CONSUMER_APP_MENU_4 "204"
#define CONSUMER_APP_MENU_5 "205"
#define CONSUMER_APP_MENU_6 "206"
#define CONSUMER_APP_MENU_7 "207"
#define CONSUMER_APP_MENU_8 "208"
#define CONSUMER_APP_MENU_9 "209"

class NSProviderHelper
{
private:
    static NSProviderHelper* s_nsHelperInstance;
    static std::mutex s_mutex;
    static NSConsumer *s_pConsumer;

    NSProviderHelper();
    ~NSProviderHelper();
    void waitForConsumer(int time);

public:
    static bool s_isOnSubscribeRequest;
    static bool s_isOnProviderSyncInfo;
    static string s_consumerID;

    /**
     * @brief   Function is for getting singleton instance of NSProviderHelper
     * @return  singleton instance of NSProviderHelper
     */
    static NSProviderHelper* getInstance(void);

    /**
     * @brief   Callback function for Consumer Subscribe
     */
    static void onSubscribeRequest(NSConsumer* consumer);

    /**
     * @brief   Callback function for Consumer Sync
     */
    static void onProviderSyncInfo(NSSyncInfo* syncInfo);

    /**
     * @brief   Construct provider config
     * @return  Provider configuration
     */
    NSProviderConfig getProviderConfig(bool policy);

    /**
     * @brief   Get consumer from callback
     * @return  consumerID
     */
    string getConsumerID();

    /**
     * @brief   Get consumer from callback
     * @return  consumer
     */
    NSConsumer* getConsumer();

    /**
     * @brief   Show List of Provider Topics
     * @input  Topic List
     */
    bool printProviderTopicList(NSTopicLL *topics);

};

#endif /* INCLUDE_TESTCASE_NS_GTEST_NSPROVIDERHELPER_H_ */
