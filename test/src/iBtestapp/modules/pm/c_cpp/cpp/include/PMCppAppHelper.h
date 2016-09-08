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
#ifndef PMCppAppHelper_H_
#define PMCppAppHelper_H_

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <map>
#include <cstdlib>
#include <pthread.h>
#include <mutex>
#include <condition_variable>

#include "logger.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "OCPlatform.h"
#include "OCApi.h"
#include "OCProvisioningManager.h"
#include "oxmjustworks.h"
#include "oxmrandompin.h"
#include "utlist.h"


#include "CommonUtil.h"
#include "IotivityTest_Logger.h"



using namespace OC;

#define DASH "-"
#define EMPTY_STRING ""
#define g_ctx "Provision Manager Client Application Context"
#define ctxProvCreadential "ProvisionCredentials Context"
#define ctxProvPairwise "ctxProvPairwise"
#define ctxProvDirectPairing "ctxProvDirectPairing"
#define ctxUnlinkDevice "ctxUnlinkDevice"
#define ctxRemoveDevice "ctxRemoveDevice"
#define RANDOM_PIN_TEXT_FILE "server_pincode.txt"
#define PIN_MAX_SIZE 9
#define MAX_TIME_INPUT_PROMPT_FOR_PASSWORD 5
#define DEVICE_INDEX_ONE 1
#define DEVICE_INDEX_TWO 2

/*
 * ACL Releated Resources
 */
#define ACL_SUBJECT_UUID_01 "DeviceID_01"
#define ACL_SUBJECT_UUID_02 "DeviceID_02"
#define ACL_RESOURCE_LEN 2
#define LIGHT_RESOURCE_URI_01 "/rsrc/light1"
#define LIGHT_RESOURCE_URI_02 "/rsrc/light2"
#define ACL_ROWNER_UUID_01 "OwnerDeviceID01"
#define ACL_ROWNER_UUID_02 "OwnerDeviceID02"
#define ACL_RESRC_MAX_NUM   16
#define ACL_RESRC_MAX_LEN   128
#define ACL_PEMISN_CNT      5
#define DEFAULT_DP_PROVSIONING_PIN "00000000"
#define DEFAULT_DP_PROVSIONING_PIN2 "00000011"
#define FULL_PERMISSION 31
#define NO_PERMISSION 0
#define MAX_PERMISSION_RANGE 65535

#define ACL_RESOURCE_URI "sensor/light"
#define ACL_RESOURCE_LENGTH 6 // Check ACL_RESOURCE_NAME for the length. ACL_RESOURCE_LENGTH = len(ACL_RESOURCE_URI) + 1
#define ACL_RES_TYPE_NAME "light"
#define ACL_RES_IF_TYPE_NAME "sensor"

/*
 * Callback Releated Resources
 */
#define CALLBACK_TIMEOUT    60  // 1 min
#define CALLBACK_INVOKED 1
#define CALLBACK_NOT_INVOKED 0

/*
 * Ownership Transfer Related Resource
 */
#define OTM_INVALID_LOBV -1
#define OTM_INVALID_UOBV 10
#define OTM_NONE 0
#define OTM_JUSTWORK 1
#define OTM_RANDOMPIN 2
#define OTM_ALL 3

/*
 * Credential Representative value
 */
#define SYM_PAIR_WISE_KEY 1
#define CRED_TYPE_UOBV 30
#define CRED_TYPE_LOBV -3

/**
 *  Server and Client Resources
 */
#define SVR_DB_FILE_NAME "oic_svr_db_client.dat"
#define PRVN_DB_FILE_NAME "oic_prvn_mng.db"
#define JUSTWORKS_SERVER "./sampleserver_justworks"
#define RANDOMPIN_SERVER "./sampleserver_randompin"
#define CLIENT_CBOR "./oic_svr_db_client.dat"
#define DATABASE_PDM "./PDM.db"

/**
 *  Time Related Resources
 */
#define DELAY_SHORT 1
#define DELAY_MEDIUM 5
#define DELAY_LONG 10
#define DISCOVERY_TIMEOUT 10
#define DISCOVERY_TIMEOUT_ZERO 0
#define DISCOVERY_TIMEOUT_ONE 1
#define DISCOVERY_TIMEOUT_TWO 2
#define DISCOVERY_TIMEOUT_INVALID -1

FILE* clientCppAppOpen(const char *UNUSED_PARAM, const char *mode);
OicSecAcl_t* createAcl(const int dev_num, int permission, DeviceList_t& deviceList);
const char *getResult(OCStackResult result);
void InputPinCB(char* pinBuf, size_t bufSize);
std::string getOCStackResultCPP(OCStackResult ocstackresult);

int startServer(int serverType);
bool provisionInit();
bool discoverUnownedDevices(int time, DeviceList_t& data, OCStackResult expectedResult);
bool discoverOwnedDevices(int time, DeviceList_t& data, OCStackResult expectedResult);
bool getDevInfoFromNetwork(unsigned short time, DeviceList_t& ownedDevList,
        DeviceList_t &unownedDevList, OCStackResult expectedResult);
bool setOwnerTransferCallbackData(int num, OTMCallbackData_t& data, InputPinCallback inputPin,
        OCStackResult expectedResult);
bool doOwnershipTransfer(DeviceList_t &data, ResultCallBack resultCallback,
        OCStackResult expectedResult);
bool provisionACL(DeviceList_t& deviceList, const OicSecAcl_t* acl, ResultCallBack resultCallback,
        OCStackResult expectedResult);
bool provisionCredentials(DeviceList_t& deviceList, const Credential &cred,
        const OCSecureResource &device2, ResultCallBack resultCallback,
        OCStackResult expectedResult);
bool provisionPairwiseDevices(DeviceList_t& deviceList, const Credential &cred,
        const OicSecAcl_t* acl1, const OCSecureResource &device2, const OicSecAcl_t* acl2,
        ResultCallBack resultCallback, OCStackResult expectedResult);
bool provisionDirectPairing(DeviceList_t& deviceList, const OicSecPconf_t pconf,
        ResultCallBack resultCallback, OCStackResult expectedResult);
bool getLinkedDevices(DeviceList_t& deviceList, UuidList_t &uuidList, OCStackResult expectedResult);
bool unlinkDevices(DeviceList_t& deviceList, const OCSecureResource &device2,
        ResultCallBack resultCallback, OCStackResult expectedResult);
bool removeDevice(DeviceList_t& deviceList, unsigned short waitTimeForOwnedDeviceDiscovery,
        ResultCallBack resultCallback, OCStackResult expectedResult);
bool removeDeviceWithUuid(unsigned short waitTimeForOwnedDeviceDiscovery, std::string uuid,
        ResultCallBack resultCallback, OCStackResult expectedResult);

void ownershipTransferCB(PMResultList_t *result, int hasError);
void provisionCB(PMResultList_t *result, int hasError);

std::string getFailureMessage();
static std::string setFailureMessage(OCStackResult expectedResult, OCStackResult actualResult);
OCProvisionDev_t* getDevInst(const OCProvisionDev_t* dev_lst, const int dev_num);
int waitCallbackRet();
#endif
