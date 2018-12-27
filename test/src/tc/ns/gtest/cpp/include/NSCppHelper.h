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

#ifndef INCLUDE_TESTCASE_NS_GTEST_NS_CPP_HELPER_H_
#define INCLUDE_TESTCASE_NS_GTEST_NS_CPP_HELPER_H_

#include "NSCppUtility.h"

using namespace OIC::Service;

#define PROVIDER_SIMULATOR "./iotivity_ns_provider_cpp_simulator"
#define PROVIDER_SIMULATOR_ALLOW "./iotivity_ns_provider_cpp_simulator 0"
#define PROVIDER_SIMULATOR_NOT_ALLOW "./iotivity_ns_provider_cpp_simulator 1"
#define CONSUMER_SIMULATOR "./iotivity_ns_consumer_cpp_simulator"
#define CONSUMER_SIMULATOR_OPTION "./iotivity_ns_consumer_cpp_simulator 0"

class NSCppHelper
{
private:
    static NSCppHelper* s_pNSHelperInstance;
    static std::mutex s_mutex;
    static string s_providerID;
    static string s_consumerID;
    static OIC::Service::NSMessage s_message;
    static OIC::Service::NSSyncInfo s_providerSyncInfo;
    static OIC::Service::NSSyncInfo s_consumerSyncInfo;
    static OIC::Service::NSProviderState s_providerState;
    static shared_ptr< OIC::Service::NSProvider > s_provider;
    static shared_ptr< OIC::Service::NSConsumer > s_consumer;
    NSProviderService* m_pProviderService;
    NSConsumerService* m_pConsumerService;
    bool m_isConsumerStarted;
    bool m_isProviderStarted;

public:
    NSCppHelper();
    static NSCppHelper* getInstance();
    static void onProviderDiscovered(shared_ptr< OIC::Service::NSProvider > provider);
    static void onConsumerDiscovered(shared_ptr< OIC::Service::NSConsumer > consumer);
    static void onMessageReceived(OIC::Service::NSMessage message);
    static void onProviderStateReceived(OIC::Service::NSProviderState state);
    static void syncCallback(OIC::Service::NSSyncInfo);
    static void syncSyncronizationCallback(OIC::Service::NSSyncInfo);
    static void subscribeRequestCallback(shared_ptr< OIC::Service::NSConsumer > );
    shared_ptr< OIC::Service::NSProvider > getProvider(bool subcontrollability);
    void waitForProvider(int timeout);
    void waitForConsumer(int timeout);
    shared_ptr< OIC::Service::NSConsumer > getConsumer(bool subcontrollability);
    shared_ptr< OIC::Service::NSProvider > getLocalProvider(bool);
    bool waitForService(int, bool);
    OIC::Service::NSMessage getNotification();
    bool sendNotification();
    shared_ptr< OIC::Service::NSConsumer > getLocalConsumer(bool, bool);
    OIC::Service::NSProviderService* getProviderService();
    OIC::Service::NSConsumerService* getConsumerService();
    void setProviderStartStatus(bool);
    void setConsumerStartStatus(bool);
    void stopServices();
    bool stopProviderService();
    OIC::Service::NSSyncInfo getSyncInfo(bool);
    OIC::Service::NSProviderState getProviderState(void);
    bool sendSyncInfo(bool isProvider, OIC::Service::NSSyncInfo::NSSyncType type);
    void preConfigure();
};

#endif /* INCLUDE_TESTCASE_NS_GTEST_NS_CPP_HELPER_H_ */
