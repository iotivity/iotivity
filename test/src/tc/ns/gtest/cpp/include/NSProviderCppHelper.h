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

#ifndef INCLUDE_TESTCASE_NS_GTEST_NSCONSUMER_CPP_HELPER_H_
#define INCLUDE_TESTCASE_NS_GTEST_NSCONSUMER_CPP_HELPER_H_

#include "NSCommonCppHelper.h"
#include "NSProviderService.h"
#include "NSConsumer.h"

using namespace OIC::Service;

#define CONSUMER_SIMULATOR "./ns_consumer_cpp_simulator"
#define CONSUMER_SIMULATOR_OPTION "'./ns_consumer_cpp_simulator 0'"

class NSProviderCppHelper
{
private:
    static NSProviderCppHelper* s_nsHelperInstance;
    static std::mutex s_mutex;
    static string s_consumerID;

public:
    static NSProviderService* s_pNSProviderService;

public:
    NSProviderCppHelper();
    static NSProviderCppHelper* getInstance();
    string getResultString(OIC::Service::NSResult result);
    void waitForConsumer(int timeout);
    OIC::Service::NSConsumer* getConsumer(bool subcontrollability);
    static void syncCallback(OIC::Service::NSSyncInfo *sync);
    static void subscribeRequestCallback(OIC::Service::NSConsumer *consumer);
};

#endif /* INCLUDE_TESTCASE_NS_GTEST_NSCONSUMER_CPP_HELPER_H_ */
