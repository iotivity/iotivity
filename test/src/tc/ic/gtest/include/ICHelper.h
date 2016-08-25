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

#ifndef INCLUDE_TESTCASE_ICHELPER_H_
#define INCLUDE_TESTCASE_ICHELPER_H_

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <ctime>
#include <unistd.h>
#include <functional>
#include <bitset>
#include <map>
#include <set>
#include <exception>

#include "gtest/gtest.h"
#include "gtest_custom.h"
#include "OCResource.h"
#include "octypes.h"
#include "OCApi.h"
#include "OCPlatform.h"
#include <InitializeException.h>
#include "OCException.h"
#include <ocstack.h>

#include "CommonUtil.h"
#include "IotivityTest_Logger.h"

using namespace std;
using namespace OC;
using namespace OC::OCPlatform;

#define IC_HOST_ADDRESS                     "192.168.1.2:5000"
#define IC_HOST_ADDRESS_INVALID             "AAA.BBB:GGG"
#define IC_HOST_ADDRESS_EMPTY               ""
#define IC_CONNECTIVITY_TYPE                CT_DEFAULT
#define IC_OC_QUALITY_OF_SERVICE            QualityOfService::HighQos
#define IC_OC_QUALITY_OF_SERVICE_NULL       NULL
#define IC_CALLBACK_HANDLER_NULL            NULL

#define IC_HOST_ADDRESS_EMPTY               ""
#define IC_SLEEPTIME_MAX                    5
#define IC_SLEEPTIME_MIN                    2

class ICHelper
{

private:
    static ICHelper* s_ICHelper;
    static std::mutex s_mutex;

public:
    ICHelper(void);
    ~ICHelper(void);
    /**
     * @brief 	function for getting helper instance
     * @return	helper instance
     */
    static ICHelper* getInstance(void);
};

#endif
