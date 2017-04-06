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

#ifndef PMCsdkUtilityHelper_H_
#define PMCsdkUtilityHelper_H_

#ifdef __GNUC__
#pragma GCC system_header
#endif

#include <stddef.h>
#include <string>
#include "casecurityinterface.h"
#include "cathreadpool.h"
#include "occloudprovisioning.h"
#include "ocpayload.h"
#include "ocprovisioningmanager.h"
#include "ocstack.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "crlresource.h"
#include "oxmjustworks.h"
#include "oxmrandompin.h"
#include "oxmverifycommon.h"
#include "OCAccountManager.h"
#include "OCApi.h"
#include "OCPlatform.h"
#include "OCPlatform_impl.h"
#include "payload_logging.h"
#include "pmtypes.h"
#include "rd_client.h"
#include "securevirtualresourcetypes.h"
#include "srmutility.h"
#include "ssl_ciphersuites.h"
#include "utils.h"
#include "utlist.h"

#include "CommonUtil.h"
#include "CommonTestUtil.h"
#include "CommonProperties.h"
#include "IotivityTest_Logger.h"

using namespace std;
using namespace OC;

#define TAG "PM CPP"
/**
 *  Server and Client Resources
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
#define PRECONFIG_SERVER2_CBOR "../preconfig_server_2.dat"
#define PRECONFIG_SERVER2_CBOR_BACKUP "../preconfig_server_2.dat"

#define MOT_CTX "MOT Context"
#define MOT_CBOR_FILE "oic_svr_db_subowner_client.dat"
#define MOT_PRVN_DB_FILE_NAME "oic_pdm_subowner.db"
#define MOT_CLIENT_CBOR "./oic_svr_db_subowner_client.dat"
#define MOT_CLIENT_CBOR_BACKUP "../oic_svr_db_subowner_client.dat"
#define MOT_CLIENT_DATABASE "./oic_pdm_subowner.db"
#define MOT_DEFAULT_PRE_CONFIG_PIN "12341234"
#define MOT_PRECONFIG_PIN_ZERO_SIZE 0
#define OIC_MULTIPLE_OWNER_UOBV 10

#define CTX_CERT_REQ_ISSUE "Cert Request Context"
#define CTX_PROV_TRUST_CERT "Trust Cert Context"
#define ROOT_CERT_FILE "rootca.crt"
#define ROOT_CERT_FILE_BACKUP "../rootca.crt"

class PMCsdkUtilityHelper
{

public:

    static OCProvisionDev_t* getDevInst(OCProvisionDev_t* dev_lst, const int dev_num);

    static int printDevList(OCProvisionDev_t*);

    static int printResultList(const OCProvisionResult_t*, const size_t);

    static size_t printUuidList(const OCUuidList_t*);

    static void printUuid(const OicUuid_t*);

    static char* getOxmType(OicSecOxm_t oxmType);

    static void removeAllResFile();

    static std::string setFailureMessage(OCStackResult actualResult, OCStackResult expectedResult);

    static std::string setFailureMessage(OicSecOxm_t actualResult, OicSecOxm_t expectedResult);

    static std::string setFailureMessage(std::string errorMessage);
};

#endif /* PMCsdkUtilityHelper_H_ */
