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

#ifndef INCLUDE_TESTCASE_NS_GTEST_NSCOMMON_HELPER_H_
#define INCLUDE_TESTCASE_NS_GTEST_NSCOMMON_HELPER_H_

#include <algorithm>
#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <mutex>
#include <string>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "ocstack.h"
#include "octypes.h"
#include "oic_string.h"
#include "gtest_custom.h"
#include "gtest/gtest.h"
#include "OCPlatform.h"
#include "CommonUtil.h"
#include "IotivityTest_Logger.h"

#define MAX_BUF 51200
#define WAIT_TIME_MIN 1
#define WAIT_TIME_DEFAULT 2
#define WAIT_TIME_MAX 10

#define PROVIDER_APP_OPTION "./ns_provider_testapp 0 1"
#define PROVIDER_APP "./ns_provider_testapp"

#define CONSUMER_APP_OPTION "./ns_consumer_testapp 0 1"
#define CONSUMER_TESTAPP "./ns_consumer_testapp"

using namespace OC;
using namespace std;
namespace PH = std::placeholders;

class NSCommonHelper
{
private:
    static NSCommonHelper* s_NSCommonHelperInstance;
    static std::mutex s_mutex;
    static string log;

    bool m_isProvider;

    NSCommonHelper();
    ~NSCommonHelper();

public:
    /**
     * @brief   Function is for getting singleton instance of NSCommonHelper
     * @return  singleton instance of NSCommonHelper
     */
    static NSCommonHelper* getInstance(void);

    void initPipe(bool isProvider);

    void closePipe();

    void inputMenu(char input[]);

    string getAppLog();

    void loggerReader();
};

#endif /* INCLUDE_TESTCASE_NS_GTEST_NSCOMMON_HELPER_H_ */
