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

#ifndef INCLUDE_TESTCASE_RC_GTEST_RCHELPER_H_
#define INCLUDE_TESTCASE_RC_GTEST_RCHELPER_H_

#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <mutex>
#include <algorithm>
#include "ocstack.h"
#include "octypes.h"
#include "gtest_custom.h"
#include "OCPlatform.h"
#include "CommonUtil.h"
#include "IotivityTest_Logger.h"
#include "RCSBundleInfo.h"
#include "RCSResourceContainer.h"

using namespace OC;
using namespace OIC;
using namespace OIC::Service;
using namespace std;
namespace PH = std::placeholders;

class RCHelper {
private:
    static RCHelper* s_rcHelperInstance;
    static std::mutex s_mutex;
    string m_errorMessage;

public:
    RCHelper();
    virtual ~RCHelper();

    /**
     * @brief   Function is for getting singleton instance of TMHelper
     * @return  singleton instance of TMHelper
     */
    static RCHelper* getInstance(void);

    string getFailureMessage();

    bool isBundleAdded(RCSResourceContainer *container, string bundleID);

    unique_ptr<RCSBundleInfo> getBundle(RCSResourceContainer *container,
            string bundleID);

    bool startBundle(RCSResourceContainer *container, string bundleID);

    bool stopBundle(RCSResourceContainer *container, string bundleID);

    bool removeBundle(RCSResourceContainer *container, string bundleID);

    bool addResourceConfigVersion(RCSResourceContainer *container,
            string bundleID, string value);

};

#endif
