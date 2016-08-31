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

#include <gtest/gtest.h>
#include "CommonUtil.h"
#include <atomic>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include "escommon.h"
#include "easysetup.h"
#include "OCPlatform.h"
#include "RemoteEnrollee.h"
#include "ESRichCommon.h"
#include "CloudResource.h"
#include <escommon.h>
#include "ESEnrolleeCommon.h"
#include <iostream>
#include <pthread.h>
#include "EasySetup.h"
#include "IotivityTest_Logger.h"
using namespace std;
using namespace OC;
using namespace OIC::Service;

class ESMediatorHelper
{
public:
    ESMediatorHelper();
    static std::shared_ptr< RemoteEnrollee > s_remoteEnrollee;
    static std::shared_ptr< OC::OCResource > s_curResource;
    static std::condition_variable s_cond;
    static bool s_isCallbackInvoked;
    static bool s_isDevicePropProvCallbackCalled;
    static bool s_isProvisionDevicePropSuccess;
    static bool s_isDeviceConfigCallbackCalled;
    static bool s_isConfigDevicePropSuccess;
    static bool s_isGetStatusCalled;
    static bool s_isGetStatusCallbackCalled;
    static bool s_isCloudPropProvCallbackCalled;
    static bool s_isProvisionCloudPropSuccess;

    static void cloudProvisioningStatusCallback(
            std::shared_ptr< CloudPropProvisioningStatus > provStatus);

    static void getStatusCallback(std::shared_ptr< GetEnrolleeStatus > getEnrolleeStatus);

    static void getConfigurationCallback(
            std::shared_ptr< GetConfigurationStatus > getConfigurationStatus);
    static void deviceProvisioningStatusCallback(
            std::shared_ptr< DevicePropProvisioningStatus > provStatus);
    static void foundResource(std::shared_ptr< OC::OCResource > resource);
    static void *listeningFunc(void * data);
    void findEnrolleeResource();
    void provisionCloudProperty();
    void printStatus(EnrolleeStatus status);
    void getStatus();
    void printConfiguration(EnrolleeConf conf);
    void getConfiguration();
    void provisionDeviceProperty();
    void waitForResponse(int time);
};
