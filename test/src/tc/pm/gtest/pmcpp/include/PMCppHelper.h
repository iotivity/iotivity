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

#ifndef PMCppHelper_H_
#define PMCppHelper_H_

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <gtest/gtest.h>
#include <OCPlatform_impl.h>
#include <OCApi.h>
#include <OCProvisioningManager.h>
#include <oxmjustworks.h>
#include <oxmrandompin.h>
#include "oic_malloc.h"
#include "utlist.h"
#include "ocprovisioningmanager.h"
#include "secureresourceprovider.h"
#include "oxmjustworks.h"
#include "oxmrandompin.h"
#include "pinoxmcommon.h"
#include "oic_string.h"
#include "utlist.h"
#include "OCPlatform.h"
#include "OCApi.h"
#include "OCProvisioningManager.h"

#include "CommonUtil.h"
#include "IotivityTest_Logger.h"

using namespace OC;

#define EMPTY_STRING ""
#define DASH '-'
#define DBPATH "./dbPath"
#define TAG  "provisioningclient"
#define CBOR_DB_PATH "./oic_svr_db_client.dat"
#define RANDOM_PIN_TEXT_FILE "server_pincode.txt"
#define PIN_MAX_SIZE 9
#define DP_PIN_LENGTH 8
/*
 * Time Releated Resources
 */
#define DISCOVERY_TIMEOUT 5
#define DISCOVERY_TIMEOUT_ZERO 0
#define DISCOVERY_TIMEOUT_ONE   1
#define DISCOVERY_TIMEOUT_TWO   2
#define DELAY_SHORT 1
#define DELAY_MEDIUM 5
#define DELAY_LONG 10
#define DELAY_LONG_LONG 50

/*
 * Callback Related Resources
 */
#define CALLBACK_TIMEOUT    60
#define CALLBACK_INVOKED 1
#define CALLBACK_NOT_INVOKED 0

/*
 * ACL Related Resources
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
#define FULL_PERMISSION 31
#define DEFAULT_PERMISSION 31
#define NO_PERMISSION 0
#define MAX_PERMISSION_RANGE 65535
#define DEFAULT_DP_PROVSIONING_PIN "00000000"
#define DEVICE_INDEX_ONE 1
#define DEVICE_INDEX_TWO 1

#define ACL_RESOURCE_URI "sensor/light"
#define ACL_RESOURCE_LENGTH 6 // Check ACL_RESOURCE_NAME for the length. ACL_RESOURCE_LENGTH = len(ACL_RESOURCE_URI) + 1
#define ACL_RES_TYPE_NAME "light"
#define ACL_RES_IF_TYPE_NAME "sensor"

/*
 * Device Status
 */
#define ON 1
#define OFF 0
#define DEVICE_OWNED true
#define DEVICE_UNOWNED false

/*
 * Ownership Transfer Related Resource
 */
#define OTM_INVALID_LOBV -1
#define OTM_INVALID_UOBV 10
#define OTM_NONE 0
#define OTM_JUSTWORK 0
#define OTM_RANDOMPIN 2
#define OTM_ALL 3

/*
 * Credential Representative value
 */
#define SYM_PAIR_WISE_KEY 0
#define CRED_TYPE_LOBV -1
#define CRED_TYPE_UOBV 100

/**
 *  Server and Client Resources
 */
#define SVR_DB_FILE_NAME "oic_svr_db_client.dat"
#define PRVN_DB_FILE_NAME "oic_prvn_mng.db"
#define JUSTWORKS_SERVER "./sampleserver_justworks"
#define JUSTWORKS_SERVER1 "'./sampleserver_justworks 1'"
#define JUSTWORKS_SERVER2 "'./sampleserver_justworks 2'"
#define JUSTWORKS_SERVER1_CBOR "./oic_svr_db_server.dat"
#define JUSTWORKS_SERVER2_CBOR "./oic_svr_db_server_justworks.dat"
#define JUSTWORKS_SERVER1_CBOR_BACKUP "../oic_svr_db_server.dat"
#define JUSTWORKS_SERVER2_CBOR_BACKUP "../oic_svr_db_server_justworks.dat"
#define RANDOMPIN_SERVER "./sampleserver_randompin"
#define CLIENT_CBOR "./oic_svr_db_client.dat"
#define CLIENT_CBOR_BACKUP "../oic_svr_db_client.dat"
#define CLIENT_DATABASE "./oic_prvn_mng.db"
#define DATABASE_PDM "./PDM.db"
#define PRVN_DATABASE "./oic_prvn_mng.db"

static const OicSecPrm_t SUPPORTED_PRMS[1] =
{ PRM_PRE_CONFIGURED, };

const char *getResult(OCStackResult result);
void InputPinCB(char* pinBuf, size_t bufSize);
std::string setFailureMessage(OCStackResult expectedResult, OCStackResult actualResult);
std::string getOCStackResultCPP(OCStackResult ocstackresult);
OicSecAcl_t* createAcl(const int dev_num, int nPermission, DeviceList_t &m_OwnedDevList);
OicSecPdAcl_t* createPdAcl(int nPermission);

class PMCppHelper
{
private:
    std::string m_failureMessage;

public:

    PMCppHelper();
    bool provisionInit();
    bool discoverUnownedDevices(int time, DeviceList_t& data, OCStackResult expectedResult);
    bool discoverOwnedDevices(int time, DeviceList_t& data, OCStackResult expectedResult);
    bool getDevInfoFromNetwork(unsigned short time, DeviceList_t& ownedDevList,
            DeviceList_t &unownedDevList, OCStackResult expectedResult);
    bool setOwnerTransferCallbackData(int num, OTMCallbackData_t& data, InputPinCallback inputPin,
            OCStackResult expectedResult);
    bool doOwnershipTransfer(DeviceList_t &data, ResultCallBack resultCallback,
            OCStackResult expectedResult);
    bool provisionACL(DeviceList_t& deviceList, const OicSecAcl_t* acl,
            ResultCallBack resultCallback, OCStackResult expectedResult);
    bool provisionCredentials(DeviceList_t& deviceList, const Credential &cred,
            const OCSecureResource &device2, ResultCallBack resultCallback,
            OCStackResult expectedResult);
    bool provisionPairwiseDevices(DeviceList_t& deviceList, const Credential &cred,
            const OicSecAcl_t* acl1, const OCSecureResource &device2, const OicSecAcl_t* acl2,
            ResultCallBack resultCallback, OCStackResult expectedResult);
    bool provisionDirectPairing(DeviceList_t& deviceList, const OicSecPconf_t pconf,
            ResultCallBack resultCallback, OCStackResult expectedResult);
    bool getLinkedDevices(DeviceList_t& deviceList, UuidList_t &uuidList,
            OCStackResult expectedResult);
    bool unlinkDevices(DeviceList_t& deviceList, const OCSecureResource &device2,
            ResultCallBack resultCallback, OCStackResult expectedResult);
    bool removeDevice(DeviceList_t& deviceList, unsigned short waitTimeForOwnedDeviceDiscovery,
            ResultCallBack resultCallback, OCStackResult expectedResult);

    static void ownershipTransferCB(PMResultList_t *result, int hasError);
    static void provisionCB(PMResultList_t *result, int hasError);

    std::string getFailureMessage();
    static std::string setFailureMessage(OCStackResult expectedResult, OCStackResult actualResult);
    OCProvisionDev_t* getDevInst(const OCProvisionDev_t* dev_lst, const int dev_num);
    int waitCallbackRet();
};
#endif
