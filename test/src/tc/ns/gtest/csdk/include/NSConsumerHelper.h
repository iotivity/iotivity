/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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

#ifndef INCLUDE_TESTCASE_NS_GTEST_NSCONSUMER_HELPER_H_
#define INCLUDE_TESTCASE_NS_GTEST_NSCONSUMER_HELPER_H_

#include "NSCommonHelper.h"

#include "NSConsumerInterface.h"
#include "NSCommon.h"

#define PROVIDER_SIMULATOR "./ns_provider_csdk_simulator"
#define PROVIDER_SIMULATOR_ALLOW "./ns_provider_csdk_simulator 0"

#define TOPIC_NAME_2 "TESTAPP_Topic_2"

#define PROVIDER_APP_MENU_1 "1"
#define PROVIDER_APP_MENU_2 "2"
#define PROVIDER_APP_MENU_3 "3"
#define PROVIDER_APP_MENU_4 "4"
#define PROVIDER_APP_MENU_5 "5"
#define PROVIDER_APP_MENU_6 "6"
#define PROVIDER_APP_MENU_7 "7"
#define PROVIDER_APP_MENU_8 "8"
#define PROVIDER_APP_MENU_9 "9"
#define PROVIDER_APP_MENU_10 "10"

class NSConsumerHelper
{
private:
    static NSConsumerHelper* s_nsHelperInstance;
    static std::mutex s_mutex;
    void waitForProvider(int time);
    static NSMessage* s_pNotification;
    NSConsumerHelper();
    ~NSConsumerHelper();

public:
    static NSProvider *s_pProvider;
    static bool s_isDiscovered;
    static bool s_isTopicChanged;
    static bool s_isConsumerAllowed;
    static bool s_isNotificationPosted;

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

    bool printTopicList(NSTopicLL *topics);

    NSMessage* getNotificationMessage();
};

#endif /* INCLUDE_TESTCASE_NS_GTEST_NSCONSUMER_HELPER_H_ */
