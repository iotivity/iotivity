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
#include "OCPlatform.h"
#include "NSConsumerService.h"
#include "NSProvider.h"

using namespace OIC::Service;

#define PROVIDER_SIMULATOR "./ns_provider_cpp_simulator"
#define PROVIDER_SIMULATOR_ALLOW "'./ns_provider_cpp_simulator 0'"
#define PROVIDER_SIMULATOR_NOT_ALLOW "'./ns_provider_cpp_simulator 1'"

class NSConsumerCppHelper
{
private:
    static NSConsumerCppHelper* s_nsHelperInstance;
    static std::mutex s_mutex;
    static string s_providerID;

public:
    NSConsumerCppHelper();
    static NSConsumerCppHelper* getInstance();
    static void onProviderDiscovered(OIC::Service::NSProvider* provider);
    OIC::Service::NSProvider* getProvider(bool subcontrollability);
    string getResultString(OIC::Service::NSResult result);
    void waitForProvider(int timeout);
};

#endif /* INCLUDE_TESTCASE_NS_GTEST_NSCONSUMER_CPP_HELPER_H_ */
