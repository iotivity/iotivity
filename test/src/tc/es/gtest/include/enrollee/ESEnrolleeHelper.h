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
#include "easysetup.h"
#include <escommon.h>
#include "ESEnrolleeCommon.h"
#include <iostream>
#include <pthread.h>
#include "IotivityTest_Logger.h"
using namespace std;

static bool s_secured = true;
static bool s_nonSecured = false;

class ESEnrolleeHelper
{
public:
    ESEnrolleeHelper();
    static bool isCallbackInvoked;
    static void wiFiProvCbInApp(ESWiFiProvData *eventData);
    static void devConfProvCbInApp(ESDevConfProvData *eventData);
    static void cloudDataProvCbInApp(ESCloudProvData *eventData);
    ESResult setDeviceProperty();
    static void *listeningFunc(void * data);
    void waitForResponse();
};
