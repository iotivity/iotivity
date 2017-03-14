/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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
#include <memory>
#include <stdexcept>
#include <condition_variable>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <vector>

#include "gtest/gtest.h"
#include "gtest_custom.h"

#include "OCResource.h"
#include "octypes.h"
#include "OCApi.h"
#include "OCPlatform.h"
#include "OCRepresentation.h"
#include "cbor.h"

#include <InitializeException.h>
#include "OCException.h"

#include <ocstack.h>
#include "ocpayload.h"
#include "rd_client.h"
#include "rd_server.h"
#include "rd_database.h"
#include "RDClient.h"

#include "ICResource.h"
#include "IotivityTest_Logger.h"
#include "ICTemperatureResource.h"
#include "ICBinarySwitchResource.h"
#include "ICAirconditionerResource.h"
#include "CloudCommonUtil.h"
#include "CommonUtil.h"
#include "CommonProperties.h"
#include "CommonTestUtil.h"

#define SERVER                                  ""
#define IC_HOST_ADDRESS_INVALID                 "AAA.BBB:GGG"
#define IC_HOST_ADDRESS_EMPTY                   ""
#define IC_CONNECTIVITY_TYPE                    CT_ADAPTER_TCP
#define IC_OC_QUALITY_OF_SERVICE                QualityOfService::LowQos
#define IC_OC_QUALITY_OF_SERVICE_NULL           NULL
#define IC_CALLBACK_HANDLER_NULL                NULL

#define IC_HOST_ADDRESS_EMPTY                   ""
#define IC_SLEEPTIME_MAX                         5
#define IC_SLEEPTIME_MIN                         2

#define IC_ACCOUNT_MANAGER_HOST_ADDRESS         "192.168.43.158:5683"
//#define IC_CLOUD_INTERFACE_HOST_ADDRESS         "127.0.0.1:5683" // use for ethernet communication
#define IC_CLOUD_INTERFACE_HOST_ADDRESS         "192.168.43.158:5683" //use for wifi communicatin
#define IC_CLOUD_RD_PORT                        "5684"
#define IC_CLOUD_ACCOUNT_PORT                   "5685"
#define IC_CLOUD_MSG_PORT                       "5686"
#define AUTH_PROVIDER                           "github"
#define AUTH_CODE                               "8a3f4b35695b09d875b2"
#define RESOURCE_SERVER_DAT                     "./resource_server.dat"
#define CALLBACK_TIMEOUT                        7

#define IC_RESOURCE_URI_AIRCON                  "/sec/aircon/0"
#define IC_RESOURCE_TYPE_AIRCON                 "x.com.samsung.da.device"
#define IC_RESOURCE_URI_TEMP                    "/temperature/0"
#define IC_RESOURCE_TYPE_TEMP                   "oic.r.temperature"
#define IC_RESOURCE_URI_BSWITCH                 "/power/0"
#define IC_RESOURCE_TYPE_BSWITCH                "oic.r.switch.binary"

const string IC_HOST_ADDRESS = "coap+tcp://127.0.0.1:5683";

#define INIT_GET_LOGIN_URL "https://github.com/login"
#define LOGIN_URL "https://github.com/session"
#define IOTIVITY_CLOUD_URL "https://github.com/login?return_to=%2Flogin%2Foauth%2Fauthorize%3Fclient_id%3Dea9c18f540323b0213d0%26redirect_uri%3Dhttp%253A%252F%252Fwww.example.com%252Foauth_callback%252F"
#define PAR_POST_VAL "commit=Sign+in&utf8=%E2%9C%93"//"commit=Sign+in&utf8=✓";//"commit=Sign+in&utf8=%E2%9C%93";
#define AUTHENTICITY_TOKEN "&authenticity_token="
#define EMAIL "iotsrbd@gmail.com"
#define NAME "SRBD"
#define PHONE "+8801710321734"
#define EMPTY_STRING ""
#define INVALID "invalid#mail.b"
#define maxSequenceNumber 0xFFFFFF
#define URI "/resource"
#define TOPIC_URI "/ps/nweTopic"

#define HOST            "coap://192.168.1.2:5000"
#define ACCESS_TOKEN    "AnyAccessToken"
#define REFRESH_TOKEN   "AnyRefreshToken"
#define USER_ID         "AnyUserId"
#define DEVICE_ID       "AnyDeviceId"
#define GROUP_ID        "AnyGroupId"
#define KEY             "AnyKey"
#define VALUE           "AnyValue"

#define DEFAULT_MQ_BROKER_URI "/oic/ps"
#define TOPIC_URI "/oic/ps/light21212"
#define LIGHT "light"
#define MESSAGE "Published from SRBD"
#define MQPUBLISHER "./mq_publisher.dat"

#define INVALID_PARAMETER ""
#define EMPTY_VALUE       ""
#define ZERO_VALUE        0

typedef struct stringstr
{
    char *ptr;
    size_t len;
} stringstr;

using namespace std;
using namespace OC;
using namespace OC::OCPlatform;

class ICHelper
{

private:
    static ICHelper* s_ICHelper;
    pthread_t m_helperThread;

public:
    ICHelper(void);
    ~ICHelper(void);
    static string s_Uid;
    static string s_AccessToken;
    static string s_GroupID;
    static string s_GroupType;
    static string s_GName;
    static string s_GroupUUID;
    static std::vector< std::string > s_DeviceID;

    static std::mutex s_Mutex;
    static bool s_IsServerResponsed;
    static bool s_IsSignedUp;

    static ResourceHandles s_ResourceHandles;
    static OC::OCResource::Ptr s_binaryswitchResource;
    static bool s_IsPostRequestSuccess;
    static bool s_IsGetSuccessful;
    static bool s_IsObserveSuccessful;
    static bool s_IsDeleteSuccessful;
    static bool s_IsPublishSuccessful;
    static string s_SearchQuery;

    /**
     * @brief     function for getting helper instance
     * @return    helper instance
     */
    bool initCloudClient();
    static ICHelper* getInstance(void);
    static void icPrintRepresentation(OCRepresentation rep);
    static FILE *readResourceDATFile(const char * /*path*/, const char *mode);
    static void *executeThread(void *threadID);
    static void waitForServerResponse();bool isResourceRegistered();
    void init_string(stringstr *str);
    char* get_authenticity_token(const char* resposeTxt);
    char* get_auth_token_code(const char* resposeTxt, char *code);
    static void cloudConnectGetHandler(const HeaderOptions &head, const OCRepresentation &rep,
            const int ecode);
    static void cloudConnectObserveHandler(const HeaderOptions /*headerOptions*/,
            const OCRepresentation &rep, const int &eCode, const int &sequenceNumber);
    static void onDeleteHandler(const HeaderOptions & /*headerOptions*/, const int eCode);
    static void createTopicCB(const int ecode, const string &originUri,
            shared_ptr< OC::OCResource > topic);
    static void onPublish(const OCRepresentation &, const int &eCode);
    static void onDelete(const int& eCode);
    static void foundDevice(shared_ptr< OC::OCResource > resource);
    static void errorFoundDevice(const std::string &uri, const int ecode);
    static bool isUnResourceRegistered();
    static void deleteResponse(const HeaderOptions&, const int);
};
#endif

