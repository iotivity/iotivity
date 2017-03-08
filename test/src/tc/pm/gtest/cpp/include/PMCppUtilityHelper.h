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
#include "oxmverifycommon.h"
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
#include "CommonTestUtil.h"
#include "CommonProperties.h"
#include "IotivityTest_Logger.h"

using namespace std;
using namespace OC;

#define TAG "PMCppUtilityHelper"
#define EMPTY_STRING ""
#define DBPATH "./dbPath"
#define CBOR_DB_PATH "./oic_svr_db_client.dat"
#define RANDOM_PIN_TEXT_FILE "server_pincode.txt"
#define PIN_MAX_SIZE 9
#define INVALID_PIN_TYPE 5
#define DP_PIN_LENGTH 8
#define CRED_ID_ONE 1
/**
 * Secured Resource
 */

#define KILL_SERVERS "iotivity_pm_server"

#define SVR_DB_FILE_NAME "oic_svr_db_client.dat"
#define PRVN_DB_FILE_NAME "oic_prvn_mng.db"
#define JUSTWORKS_SERVER1 "./iotivity_pm_server oic_svr_db_server.dat 1"
#define JUSTWORKS_SERVER2 "./iotivity_pm_server oic_svr_db_server_justworks.dat 1"
#define RANDOMPIN_SERVER "./iotivity_pm_server oic_svr_db_server_randompin.dat 2"

#define MV_JUSTWORKS_SERVER "./iotivity_pm_server oic_svr_db_server_mvjustworks.dat 4"

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

#define MV_JUSTWORKS_SERVER_CBOR "./oic_svr_db_server_mvjustworks.dat"
#define MV_JUSTWORKS_SERVER_CBOR_BACKUP "../oic_svr_db_server_mvjustworks.dat"

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
#define MOT_PRECONFIG_PIN_ZERO_SIZE 0
#define OIC_MULTIPLE_OWNER_UOBV 10

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

class PMCppUtilityHelper
{

public:
    static bool readFile(const char *name, OCByteString *out);

    static char *getOxmType(OicSecOxm_t oxmType);

    static void printDevices(DeviceList_t &list);

    static void printUuid(OicUuid_t uuid);

    static OCProvisionDev_t* getDevInst(OCProvisionDev_t* dev_lst, const int dev_num);

    static int printDevList(OCProvisionDev_t*);

    static int printResultList(const OCProvisionResult_t*, const int);

    static size_t printUuidList(const OCUuidList_t*);

    static void printUuid(const OicUuid_t*);

    static void removeAllResFile();

    static std::string setFailureMessage(OCStackResult actualResult, OCStackResult expectedResult);

    static std::string setFailureMessage(OicSecOxm_t actualResult, OicSecOxm_t expectedResult);

    static std::string setFailureMessage(std::string errorMessage);
};

#endif /* PMCppUtilityHelper_H_ */
