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
#include "OCProvisioningManager.hpp"
#include "oxmjustworks.h"
#include "oxmrandompin.h"
#include "oxmverifycommon.h"
#include "utlist.h"
#include "srmutility.h"
#include "aclresource.h"

#include "CommonUtil.h"
#include "IotivityTest_Logger.h"

using namespace OC;

/**
 * Remove Resource file
 */
#define JUSTWORK1 0
#define JUSTWORK2 1
#define JUSTWORK7 2
#define RANDOMPIN 3
#define PRECONFIGPIN1 4
#define PRECONFIGPIN2 5
#define CLIENT 6
#define MOTCLIENT 7
#define MVJUSTWORK 8

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
#define KILL_SERVERS "./iotivity_pm_server"
#define JUSTWORKS_SERVER1_CBOR "./oic_svr_db_server.dat"
#define JUSTWORKS_SERVER1_CBOR_BACKUP "../oic_svr_db_server.dat"
#define JUSTWORKS_SERVER2_CBOR "./oic_svr_db_server_justworks.dat"
#define JUSTWORKS_SERVER2_CBOR_BACKUP "../oic_svr_db_server_justworks.dat"
#define RANDOMPIN_SERVER_CBOR "./oic_svr_db_server_randompin.dat"
#define RANDOMPIN_SERVER_CBOR_BACKUP "../oic_svr_db_server_randompin.dat"
#define CLIENT_CBOR_BACKUP "../oic_svr_db_client.dat"
#define SVR_DB_FILE_NAME "oic_svr_db_client.dat"
#define PRVN_DB_FILE_NAME "oic_prvn_mng.db".
#define MOT_DB_FILE_NAME "oic_mot_mng.db"
#define JUSTWORKS_SERVER "./sampleserver_justworks"
#define RANDOMPIN_SERVER "./sampleserver_randompin"
#define CLIENT_CBOR "./oic_svr_db_client.dat"
#define CLIENT_MOT_CBOR "./oic_svr_db_subowner_client.dat"
#define DATABASE_PDM "./PDM.db"

#define MVJUSTWORKS_SERVER "./iotivity_pm_server oic_svr_db_server_mvjustworks.dat 5"
#define MVJUSTWORKS_SERVER_CBOR "./oic_svr_db_server_mvjustworks.dat"
#define MVJUSTWORKS_SERVER_CBOR_BACKUP "../oic_svr_db_server_mvjustworks.dat"

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

/**
 *  MOT Related Resources
 */
#define MOT_DEFAULT_PRE_CONFIG_PIN "12341234"
#define JUSTWORKS_SERVER "./sampleserver_justworks"
#define JUSTWORKS_SERVER7 "./iotivity_pm_server mot_preconfig_pin_server.dat 1"
#define JUSTWORKS_SERVER7_CBOR "./mot_preconfig_pin_server.dat"
#define JUSTWORKS_SERVER7_CBOR_BACKUP "../mot_preconfig_pin_server.dat"
#define JUSTWORKS_SERVER2_CBOR "./oic_svr_db_server_justworks.dat"
#define JUSTWORKS_SERVER2_CBOR_BACKUP "../oic_svr_db_server_justworks.dat"

#define PRECONFIG_SERVER1 "./iotivity_pm_server preconfig_server_1.dat 3"
#define PRECONFIG_SERVER2 "./iotivity_pm_server preconfig_server_2.dat 3"
#define PRECONFIG_SERVER1_CBOR "./preconfig_server_1.dat"
#define PRECONFIG_SERVER1_CBOR_BACKUP "../preconfig_server_1.dat"
#define PRECONFIG_SERVER2_CBOR "./preconfig_server_2.dat"
#define PRECONFIG_SERVER2_CBOR_BACKUP "../preconfig_server_2.dat"
#define MOT_CLIENT_CBOR "./oic_svr_db_subowner_client.dat"
#define MOT_CLIENT_CBOR_BACKUP "../oic_svr_db_subowner_client.dat"

FILE* clientCppAppOpen(const char *UNUSED_PARAM, const char *mode);
FILE* clientCppMOTAppOpen(const char *UNUSED_PARAM, const char *mode);
OicSecAcl_t* createAcl(const int dev_num, int permission, DeviceList_t& deviceList);
const char *getResult(OCStackResult result);
//void InputPinCB(char* pinBuf, size_t bufSize);
void OnInputPinCB(OicUuid_t deviceId, char* pinBuf, size_t bufSize);
std::string getOCStackResultCPP(OCStackResult ocstackresult);
void printDevices(DeviceList_t &list);
void printUuid(OicUuid_t uuid);

int startServer(int serverType);
bool provisionInit();
bool discoverUnownedDevices(int time, DeviceList_t& data, OCStackResult expectedResult);
bool discoverSingleDevice(unsigned short timeout, const OicUuid_t* deviceID,
		std::shared_ptr<OCSecureResource> &foundDevice, OCStackResult expectedResult);
bool discoverOwnedDevices(int time, DeviceList_t& data, OCStackResult expectedResult);
bool getDevInfoFromNetwork(unsigned short time, DeviceList_t& ownedDevList,
        DeviceList_t &unownedDevList, OCStackResult expectedResult);
bool setOwnerTransferCallbackData(int num, OTMCallbackData_t& data, InputPinCallback inputPin,
        OCStackResult expectedResult);
bool doOwnershipTransfer(DeviceList_t &data, ResultCallBack resultCallback,
        OCStackResult expectedResult);
bool provisionACL(DeviceList_t& deviceList, const OicSecAcl_t* acl, ResultCallBack resultCallback,
        OCStackResult expectedResult);
bool saveACL(const OicSecAcl_t* acl,OCStackResult expectedResult);
bool provisionCredentials(DeviceList_t& deviceList, const Credential &cred,
        const OCSecureResource &device2, ResultCallBack resultCallback,
        OCStackResult expectedResult);
bool provisionPairwiseDevices(DeviceList_t& deviceList, const Credential &cred,
        const OicSecAcl_t* acl1, const OCSecureResource &device2, const OicSecAcl_t* acl2,
        ResultCallBack resultCallback, OCStackResult expectedResult);
bool getLinkedDevices(DeviceList_t& deviceList, UuidList_t &uuidList, OCStackResult expectedResult);
bool unlinkDevices(DeviceList_t& deviceList, const OCSecureResource &device2,
        ResultCallBack resultCallback, OCStackResult expectedResult);
bool removeDevice(DeviceList_t& deviceList, unsigned short waitTimeForOwnedDeviceDiscovery,
        ResultCallBack resultCallback, OCStackResult expectedResult);
bool removeDeviceWithUuid(unsigned short waitTimeForOwnedDeviceDiscovery,
        std::string uuid, ResultCallBack resultCallback, OCStackResult expectedResult);

void ownershipTransferCB(PMResultList_t *result, int hasError);
void provisionCB(PMResultList_t *result, int hasError);

std::string getFailureMessage();
OCProvisionDev_t* getDevInst(const OCProvisionDev_t* dev_lst, const int dev_num);
int waitCallbackRet();
bool convertStrToUuid(std::string uuid, OicUuid_t* deviceID,OCStackResult expectedResult);
void removeAllResFile(int resFile);
void copyAllResFile(int resFile);

//MOT

bool changeMOTMode(DeviceList_t& deviceList, const OicSecMomType_t momType, ResultCallBack resultCallback,
        OCStackResult expectedResult);
bool discoverMultipleOwnerEnabledDevices(unsigned short timeout, DeviceList_t &list,
        OCStackResult expectedResult);
bool addPreconfigPIN(DeviceList_t &list,const char * preconfPin,
        size_t preconfPinLength, OCStackResult expectedResult);
bool provisionPreconfPin(DeviceList_t &list,const char * preconfPin, size_t preconfPinLength, ResultCallBack resultCallback,
		OCStackResult expectedResult);
bool selectMOTMethod(DeviceList_t &list, const OicSecOxm_t oxmSelVal,
                    ResultCallBack resultCallback, OCStackResult expectedResult);
bool doMultipleOwnershipTransfer(DeviceList_t &list,ResultCallBack resultCallback,OCStackResult expectedResult);
bool discoverMultipleOwnedDevices(unsigned short timeout, DeviceList_t &list, OCStackResult expectedResult);
bool getLedResource(DeviceList_t &list,GetCallback attributeHandler,OCStackResult expectedResult);

int waitCallbackRetMot(void);
void changeMOTModeCB(PMResultList_t *result, int hasError);
void selectMOTMethodCB(PMResultList_t *result, int hasError);
void preConfigPinCB(PMResultList_t *result, int hasError);
void multipleOwnershipTransferCB(PMResultList_t *result, int hasError);
void getCallback(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode);
OCStackResult displayNumCB(void * ctx, uint8_t mutualVerifNum[MUTUAL_VERIF_NUM_LEN]);
OCStackResult confirmNumCB(void * ctx);

OCStackResult displayMutualVerifNumCB(uint8_t mutualVerifNum[MUTUAL_VERIF_NUM_LEN]);
OCStackResult confirmMutualVerifNumCB(void);
#endif
