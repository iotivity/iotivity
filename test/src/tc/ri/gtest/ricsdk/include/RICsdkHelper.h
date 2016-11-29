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

#ifndef RI_CSDK_HELPER_H_
#define RI_CSDK_HELPER_H_

#include <string.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <mutex>
#include <algorithm>
#include <pthread.h>
#include <gtest/gtest.h>
#include <OCApi.h>
#include <oic_malloc.h>
#include "CommonUtil.h"
#include "IotivityTest_Logger.h"
#include "OCPlatform.h"
#include "ocstack.h"
#include "ocpayload.h"

using namespace OC;
using namespace std;
namespace PH = std::placeholders;

#define EMPTY_STRING ""
#define INT_ZERO 0
#define DATABASE_FILE "oic_svr_db_server.dat"

//timeouts for callback
#define CALLBACK_WAIT_MIN 1
#define CALLBACK_WAIT_DEFAULT 5
#define CALLBACK_WAIT_MAX 5

//Set of strings for each of deviceInfo fields
#define MANUFACTURE_DATE "myDateOfManufacture"
#define FIRMWARE_VERSION "my.Firmware.Version"
#define MANUFACTURER_NAME "myManufacturer"
#define OS_VERSION "myOS"
#define HW_VERSION "myHardwareVersion"
#define PLATFORM_ID "myPlatformID"
#define MANUFACTURER_URL "www.myurl.com"
#define MODEL_NO "myModelNumber"
#define PLATFORM_VERSION "platformVersion"
#define SUPPORT_URL "www.mysupporturl.com"
#define SYSTEM_TIME "mySystemTime"

//Set of strings for each of platform info fields
#define DEVICE_NAME "TestName"
#define SPEC_VERSION "myDeviceSpecVersion"
#define DATA_MODEL_VERSION "myDeviceModelVersions"

//Context value set
#define DEFAULT_CONTEXT_VALUE 0x99

//query for doResource
constexpr char DEVICE_DISCOVERY_QUERY[]
{ "%s/oic/d" };
constexpr char PLATFORM_DISCOVERY_QUERY[]
{ "%s/oic/p" };
constexpr char ALL_RESOURCE_DISCOVERY_QUERY[]
{ "%s/oic/res" };
constexpr char TEMPERATURE_RESOURCE_DISCOVERY_QUERY[]
{ "%s/oic/res?rt=oic.r.temperature" };
constexpr char LIGHT_RESOURCE_DISCOVERY_QUERY[]
{ "%s/oic/res?rt=core.light" };
constexpr char INTERFACE_DISCOVERY_QUERY[]
{ "%s/oic/res?if=oic.if.new" };

//resource types
constexpr char RESOURCE_TYPE_LIGHT[]
{ "core.light" };
constexpr char RESOURCE_TYPE_FAN[]
{ "core.fan" };
constexpr char RESOURCE_TYPE_TEMPERATURE[]
{ "oic.r.temperature" };
constexpr char RESOURCE_TYPE_PLATFORM[]
{ "oic.wk.p" };
constexpr char RESOURCE_TYPE_DEVICE[]
{ "oic.wk.d" };
constexpr char RESOURCE_URI_LIGHT[]
{ "/a/light" };
constexpr char RESOURCE_URI_FAN[]
{ "/a/fan" };
constexpr char RESOURCE_URI_TEMPERATURE[]
{ "/TemperatureResURI" };
constexpr char RESOURCE_INTERFACE_DEFAULT[]
{ "oc.if.a" };
constexpr char RESOURCE_INTERFACE_RW[]
{ "core.rw" };

constexpr char KEY_TEMPERATURE[]
{ "temperature" };
constexpr char KEY_UNITS[]
{ "units" };
constexpr char KEY_HOUR[]
{ "x.samsung.hour" };

constexpr char KEY_DEVICE_NAME[]
{ "n" };
constexpr char KEY_SPEC_VERSION[]
{ "lcv" };

constexpr char KEY_PLATFORM_ID[]
{ "pi" };
constexpr char KEY_MANUFACTURER_NAME[]
{ "mnmn" };
constexpr char KEY_MANUFACTURER_URL[]
{ "mnml" };
constexpr char KEY_MODEL_NUMBER[]
{ "mnmo" };
constexpr char KEY_DATE_OF_MANUFACTURE[]
{ "mndt" };
constexpr char KEY_PLATFORM_VERSION[]
{ "mnpv" };
constexpr char KEY_OPERATING_SYSTEM[]
{ "mnos" };
constexpr char KEY_HARDWARE_VERSION[]
{ "mnhw" };
constexpr char KEY_FIRMWARE_VERSION[]
{ "mnfv" };
constexpr char KEY_SUPPORT_URL[]
{ "mnsl" };
constexpr char KEY_SYSTEM_TIME[]
{ "st" };

class RICsdkHelper
{
private:
    static RICsdkHelper* s_riCsdkHelperInstance;
    static std::mutex s_mutex;
    const uint8_t m_ResourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;
    const std::string m_ResourceTypeName = "core.res";
    uint16_t m_ServerPort = 0;
    const char* m_ServerIp = nullptr;
    OCResourceHandle m_handle;
    string m_failureMsg;
    pthread_t m_p_thread;

    //client variables
    uint16_t m_ClientPort = 0;
    const char* m_ClientIp = nullptr;
    OCCallbackData m_cbData;
    OCPayload* m_targetPayload;

public:
    static bool s_quitFlag;
    static bool s_isCallback;
    static bool s_isPayloadCorrect;
    static bool s_isServerResponse;
    static string s_failureMsg;
    static bool s_isResourceFound;
    static OCEntityHandlerResult s_responseErrorCode;
    static int s_unicastDiscovery;

    static char s_szQueryUri[100];
    static char s_discoveryAddr[100];
    static string s_coapQuery;
    static OCDevAddr *s_remoteAddr;
    static OCResourceHandle s_handle;
    static int64_t s_temp;
    static char* s_units;
    static int64_t s_hour;

    static bool s_isBtc;
    OCStackResult m_result;

public:
    RICsdkHelper();
    virtual ~RICsdkHelper();
    static RICsdkHelper* getInstance(void);

    //server functions
    OCStackResult initServer();
    void duplicateString(char ** targetString, std::string sourceString);
    OCPlatformInfo getPlatformInfo();bool OCResourcePayloadAddStringLL(OCStringLL **stringLL,
            std::string value);
    OCDeviceInfo getDeviceInfo();
    static OCEntityHandlerResult OCEntityHandlerCb(OCEntityHandlerFlag flag,
            OCEntityHandlerRequest *entityHandlerRequest, void* /*callback*/);
    OCResourceHandle createResource(const char *resourceTypeName, const char *resourceInterfaceName,
            const char *uri);
    //OCResourceHandle RICsdkHelper::createTemperatureResource();

    //client functions
    OCStackResult initClient();
    OCDoHandle doResource(OCMethod method, const char *requestUri, OCQualityOfService qos);
    static OCStackApplicationResult PlatformDiscoveryReqCB(void* ctx, OCDoHandle /*handle*/,
            OCClientResponse * clientResponse);
    static OCStackApplicationResult DeviceDiscoveryReqCB(void* ctx, OCDoHandle /*handle*/,
            OCClientResponse * clientResponse);
    static OCStackApplicationResult ResourceDiscoveryReqCB(void* ctx, OCDoHandle /*handle*/,
            OCClientResponse * clientResponse);
    static OCStackApplicationResult getReqCB(void* ctx, OCDoHandle /*handle*/,
            OCClientResponse * clientResponse);
    static OCStackApplicationResult putReqCB(void* ctx, OCDoHandle /*handle*/,
            OCClientResponse * clientResponse);
    static OCStackApplicationResult postReqCB(void* ctx, OCDoHandle /*handle*/,
            OCClientResponse * clientResponse);
    static OCStackApplicationResult deleteReqCB(void* ctx, OCDoHandle /*handle*/,
            OCClientResponse * clientResponse);
    static OCStackApplicationResult obsReqCB(void* ctx, OCDoHandle /*handle*/,
            OCClientResponse * clientResponse);
    static OCStackApplicationResult allReqCB(void* ctx, OCDoHandle /*handle*/,
                OCClientResponse * clientResponse);

    //stc functions
    OCStackResult initClientServer();

    void startOCProcess();
    static void runThreadForOCProcess();

    static OCEntityHandlerResult ProcessGetRequest(OCEntityHandlerRequest *ehRequest,
            OCRepPayload **payload);
    static OCEntityHandlerResult ProcessPutRequest(OCEntityHandlerRequest *ehRequest,
            OCRepPayload** payload);
    static OCEntityHandlerResult ProcessPostRequest(OCEntityHandlerRequest *ehRequest,
            OCRepPayload** payload);
    static OCEntityHandlerResult ProcessDeleteRequest(OCEntityHandlerRequest *ehRequest);

    //payload functions
    static OCRepPayload* getPayload(int64_t temp, char* units, int64_t hour);
    static OCRepPayload* constructResponse(OCEntityHandlerRequest *ehRequest);
    static void getPayloadData(OCClientResponse * clientResponse);
    static bool payloadLogDiscovery(OCDiscoveryPayload* payload);
    static bool payloadLogDevice(OCRepPayload* payload);
    static bool payloadLogPlatform(OCRepPayload* payload);
    static bool payloadLogRepResource(OCRepPayload* payload);

    //failure message get
    string getFailureMessage();
};

#endif

