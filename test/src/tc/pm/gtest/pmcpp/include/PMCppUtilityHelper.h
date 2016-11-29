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

#ifndef PMCppUtilityHelper_H_
#define PMCppUtilityHelper_H_

#include <stddef.h>
#include <string>

#include "cathreadpool.h"
#include "logger.h"
#include "occloudprovisioning.h"
#include "ocpayload.h"
#include "ocprovisioningmanager.h"
#include "ocstack.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "oxmjustworks.h"
#include "oxmrandompin.h"
#include "OCAccountManager.h"
#include "OCApi.h"
#include "OCCloudProvisioning.hpp"
#include "OCPlatform.h"
#include "OCPlatform_impl.h"
#include "OCProvisioningManager.hpp"
#include "payload_logging.h"
#include "pmtypes.h"
#include "rd_client.h"
#include "securevirtualresourcetypes.h"
#include "srmutility.h"
#include "utils.h"
#include "utlist.h"

#include "CommonUtil.h"
#include "IotivityTest_Logger.h"

using namespace std;
using namespace OC;

#define EMPTY_STRING ""
#define DASH '-'
#define DBPATH "./dbPath"
#define CBOR_DB_PATH "./oic_svr_db_client.dat"
#define RANDOM_PIN_TEXT_FILE "server_pincode.txt"
#define PIN_MAX_SIZE 9
#define DP_PIN_LENGTH 8

/**
 * DEVICE INDEX
 */
#define DEVICE_INDEX_ONE 1
#define DEVICE_INDEX_TWO 2

/**
 *  Time Related Resources
 */
#define DELAY_SHORT 1
#define DELAY_MEDIUM 5
#define DELAY_LONG 10
#define DISCOVERY_TIMEOUT 5
#define DISCOVERY_TIMEOUT_ZERO 0
#define DISCOVERY_TIMEOUT_ONE 1
#define DISCOVERY_TIMEOUT_TWO 2
#define DISCOVERY_TIMEOUT_INVALID -1

/*
 * Callback Releated Resources
 */
#define CALLBACK_TIMEOUT    60  // 1 min
#define CALLBACK_INVOKED 1
#define CALLBACK_NOT_INVOKED 0

/**
 * Secured Resource
 */

#define KILL_SERVERS "server"

#define SVR_DB_FILE_NAME "oic_svr_db_client.dat"
#define PRVN_DB_FILE_NAME "oic_prvn_mng.db"
#define JUSTWORKS_SERVER1 "./test_server oic_svr_db_server.dat 1"
#define JUSTWORKS_SERVER2 "./test_server oic_svr_db_server_justworks.dat 1"
#define RANDOMPIN_SERVER "./test_server oic_svr_db_server_randompin.dat 2"

#define JUSTWORKS_SERVER1_CBOR "./oic_svr_db_server.dat"
#define JUSTWORKS_SERVER1_CBOR_BACKUP "../oic_svr_db_server.dat"
#define JUSTWORKS_SERVER2_CBOR "./oic_svr_db_server_justworks.dat"
#define JUSTWORKS_SERVER2_CBOR_BACKUP "../oic_svr_db_server_justworks.dat"
#define RANDOMPIN_SERVER_CBOR "./oic_svr_db_server_randompin.dat"
#define RANDOMPIN_SERVER_CBOR_BACKUP "../oic_svr_db_server_randompin.dat"
#define CLIENT_CBOR "./oic_svr_db_client.dat"
#define CLIENT_CBOR_BACKUP "../oic_svr_db_client.dat"
#define CLIENT_DATABASE "./oic_prvn_mng.db"
#define DATABASE_PDM "./PDM.db"

/**
 * MOT Resources
 */
#define MOT_CTX "MOT Context"
#define MOT_CBOR_FILE "oic_svr_db_subowner_client.dat"
#define MOT_PRVN_DB_FILE_NAME "oic_pdm_subowner.db"
#define MOT_CLIENT_CBOR "./oic_svr_db_subowner_client.dat"
#define MOT_CLIENT_CBOR_BACKUP "../oic_svr_db_subowner_client.dat"
#define MOT_CLIENT_DATABASE "./oic_pdm_subowner.db"
#define MOT_DEFAULT_PRE_CONFIG_PIN "12341234"
#define OIC_MULTIPLE_OWNER_UOBV 10

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
#define DEVICE_INDEX_TWO 2

#define ACL_RESOURCE_URI "sensor/light"
#define ACL_RESOURCE_LENGTH 6 // Check ACL_RESOURCE_NAME for the length. ACL_RESOURCE_LENGTH = len(ACL_RESOURCE_URI) + 1
#define ACL_RES_TYPE_NAME "light"
#define ACL_RES_IF_TYPE_NAME "sensor"

// CERT
#define CRED_ID_ONE 1

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

static const OicSecPrm_t SUPPORTED_PRMS[1] =
{ PRM_PRE_CONFIGURED, };

class PMCppUtilityHelper
{

public:
    static char *getOCStackResult(OCStackResult ocstackresult);
    static OCProvisionDev_t* getDevInst(OCProvisionDev_t* dev_lst, const int dev_num);
    static int printDevList(OCProvisionDev_t*);
    static int printResultList(const OCProvisionResult_t*, const int);
    static int waitCallbackRet(void);
    static size_t printUuidList(const OCUuidList_t*);
    static void printUuid(const OicUuid_t*);
    static std::string setFailureMessage(OCStackResult actualResult, OCStackResult expectedResult);
    static std::string setFailureMessage(std::string errorMessage);
};

#endif /* PMCppUtilityHelper_H_ */
