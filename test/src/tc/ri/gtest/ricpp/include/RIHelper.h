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

#ifndef RI_TC_HELPER_H_
#define RI_TC_HELPER_H_

#include <string.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <mutex>
#include <algorithm>
#include <pthread.h>
#include <cstdlib>
#include <condition_variable>

#include <OCPlatform_impl.h>
#include "OCPlatform.h"
#include <OCApi.h>
#include <oic_malloc.h>
#include <gtest/gtest.h>

#include "CommonUtil.h"
#include "IotivityTest_Logger.h"

using namespace OC;
using namespace std;
namespace PH = std::placeholders;

#define EMPTY_STRING ""
#define TEMPERATURE_URI "/a/temperature"
#define TEMPERATURE_TYPE "oic.r.temperature"
#define KEY_TEMPERATURE "temperature"
#define KEY_HOUR "hour"

//Set of strings for each of deviceInfo fields
#define PLATFORM_ID "436f6e66-6f72-6d61-6e63-6553696d756c"
#define MANUFACTURER_NAME "Vendor"
#define MANUFACTURER_URL "www.default-vendor.com"
#define MODEL_NO "ABCDE00004"
#define MANUFACTURE_DATE "2016-06-01"
#define PLATFORM_VERSION "0.0.1"
#define OS_VERSION "10"
#define HW_VERSION "1.1.0"
#define FIRMWARE_VERSION "1.1.1"
#define SUPPORT_URL "support.default-vendor.com"
#define SYSTEM_TIME "2016-06-20T10:10:10Z"
#define DEVICE_NAME "IotivitySmartRoom"

#define DEFAULT_RESOURCE_TYPE "core.res"
#define AC_TEMPERATURE_URI "/TemperatureResURI"
#define TEMPERATURE_RESOURCE_TYPE "oic.r.temperature"
#define TEMPERATURE_RESOURCE_INTERFACE "oic.if.a"

#define DEFAULT_PRESENCE_TIME 30
#define PRESENCE_TIME_MIN 0

const int CALLBACK_WAIT_MIN = 1;
const int CALLBACK_WAIT_DEFAULT = 5;
const int CALLBACK_WAIT_MAX = 10;

class RIHelper
{
private:
    static RIHelper* s_riHelperInstance;
    static std::mutex s_mutex;
    const uint8_t m_ResourceProperty = OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE;
    OCResourceHandle m_ResourceHandle;

    OCEntityHandlerResult entityHandler(shared_ptr< OCResourceRequest > request);bool ocResourcePayloadAddStringLL(
            OCStringLL **stringLL, string value);
    void duplicateString(char ** targetString, string sourceString);

    string m_failureMsg = "";
    OCRepresentation m_temperatureRep;

public:
    int m_temp = 10;
    int m_hour = 10;bool isServerOk = true;
    ObservationIds m_listOfObservers;

    RIHelper();
    virtual ~RIHelper();

    /**
     * @brief    Function is for getting singleton instance of RIHelper
     * @return   singleton instance of RIHelper
     */
    static RIHelper* getInstance(void);

    /**
     * @brief    Function is for configuring resource server
     */
    void configClientServerPlatform(void);

    /**
     * @brief     Function is for getting platform info
     * @return    OCPlatformInfo which is created.
     */
    OCPlatformInfo getPlatformInfo();

    /**
     * @brief     Function is for getting device info
     * @return    OCDeviceInfo which is created.
     */
    OCDeviceInfo getDeviceInfo();

    /**
     * @brief     Function is for register a resource and return the handler
     * @param     uri is the uri of the resource which will register
     * @param     type is the type of the resource which will register, default is "core.res"
     * @param     iface is the interface of the resource which will register, default is DEFAULT_INTERFACE
     * @return    OCResourceHandle of the registered resource
     */
    OCResourceHandle registerResource(string uri, string type = DEFAULT_RESOURCE_TYPE,
            string iface = DEFAULT_INTERFACE);

    /**
     * @brief     Function is for construct a resource
     * @param     host is the host of the resource which will be constructed
     * @param     uri is the uri of the resource which will be constructed
     * @return    OCResource of the constructed resource
     */
    OCResource::Ptr constructResourceObject(string host, string uri);

    /**
     * @brief     Function is for setting temperature representation
     */
    void setTemperatureRep();

    /**
     * @brief     Function is for getting failure message
     */
    string getFailure();
};

#endif /* RI_TC_HELPER_H_ */

