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
#ifdef __SECURED__
#ifndef SecurityCommonUtil_H_
#define SecurityCommonUtil_H_

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <array>
#include <stddef.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <signal.h>
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif
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
#include "securevirtualresourcetypes.h"
#include "oxmrandompin.h"
#include "oxmverifycommon.h"
#include "OCAccountManager.h"
#include "OCApi.h"
#include "OCPlatform.h"
#include "OCPlatform_impl.h"
#include "payload_logging.h"
#include "pmtypes.h"
#include "rd_client.h"
#include "srmutility.h"
#include "ssl_ciphersuites.h"
#include "utils.h"
#include "utlist.h"
#include "mbedtls/config.h"
#include "mbedtls/pem.h"
#include "mbedtls/x509_csr.h"
#include "occertutility.h"

#include "logger.h"
#include "srmutility.h"
#include "doxmresource.h"
#include "pinoxmcommon.h"
#include "oxmverifycommon.h"
#include "ocstack.h"
#include "ocrandom.h"
#include "srmutility.h"
#include "base64.h"

#include "CommonUtil.h"
#include "CommonProperties.h"
#include "IotivityTest_Logger.h"

#define OTM_NONE 0
#define OTM_JUSTWORK 1
#define OTM_RANDOMPIN 2
#define OTM_ALL 3
#define PM_CLIENT_DB "pmclient.db"
#define SVR_DB_FILE_NAME "oic_svr_db_client.dat"
#define RANDOM_PIN_TEXT_FILE "server_pincode.txt"
#define PIN_MAX_SIZE 9

#define ROOT_CERT "rootca.crt"
#define JUSTWORKS_SERVER_CBOR_O1 "oic_svr_db_server.dat"
#define JUSTWORKS_SERVER_CBOR_O2 "oic_svr_db_server_justworks.dat"
#define RANDOMPIN_SERVER_CBOR_01 "oic_svr_db_server_randompin.dat"
#define MV_JUSTWORKS_CBOR_01 "oic_svr_db_server_mvjustworks.dat"
#define CLIENT_CBOR_01 "oic_svr_db_client.dat"
#define DEVICE_PROP_CBOR_01 "device_properties.dat"
#define CLIENT_DB_01 "pmclient.db"

#define ROOT_CERT_UNOWNED_BACKUP "./unowned/rootca.crt"
#define JUSTWORKS_SERVER_CBOR_O1_UNOWNED_BACKUP "./unowned/oic_svr_db_server.dat"
#define JUSTWORKS_SERVER_CBOR_O2_UNOWNED_BACKUP "./unowned/oic_svr_db_server_justworks.dat"
#define RANDOMPIN_SERVER_CBOR_01_UNOWNED_BACKUP "./unowned/oic_svr_db_server_randompin.dat"
#define MV_JUSTWORKS_CBOR_01_UNOWNED_BACKUP "./unowned/oic_svr_db_server_mvjustworks.dat"
#define CLIENT_CBOR_01_UNOWNED_BACKUP "./unowned/oic_svr_db_client.dat"
#define DEVICE_PROP_CBOR_01_UNOWNED_BACKUP "./unowned/device_properties.dat"

#define JUSTWORKS_SERVER_CBOR_O1_OWNED_BACKUP "./owned/oic_svr_db_server.dat"
#define JUSTWORKS_SERVER_CBOR_O2_OWNED_BACKUP "./owned/oic_svr_db_server_justworks.dat"
#define RANDOMPIN_SERVER_CBOR_01_OWNED_BACKUP "./owned/oic_svr_db_server_randompin.dat"
#define CLIENT_CBOR_01_OWNED_BACKUP "./owned/oic_svr_db_client.dat"
#define DEVICE_PROP_CBOR_01_OWNED_BACKUP "./owned/device_properties.dat"
#define CLIENT_DB_01_OWNED_BACKUP "./owned/pmclient.db"

#define JUSTWORKS_SERVER_CBOR_O1_PAIRWISE_BACKUP "./pairwise/oic_svr_db_server.dat"
#define JUSTWORKS_SERVER_CBOR_O2_PAIRWISE_BACKUP "./pairwise/oic_svr_db_server_justworks.dat"
#define RANDOMPIN_SERVER_CBOR_01_PAIRWISE_BACKUP "./pairwise/oic_svr_db_server_randompin.dat"
#define CLIENT_CBOR_01_PAIRWISE_BACKUP "./pairwise/oic_svr_db_client.dat"
#define DEVICE_PROP_CBOR_01_PAIRWISE_BACKUP "./pairwise/device_properties.dat"
#define CLIENT_DB_01_PAIRWISE_BACKUP "./pairwise/pmclient.db"

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
#define PRECONFIG_SERVER2_CBOR "./preconfig_server_2.dat"
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
#define TEST_CERT_ROLE1 "IoTivity-test-role1"

static char g_allowedDevId[][UUID_STRING_SIZE] =
{   "12345678-1234-1234-1234-123456789012", "12345678-1234-1234-1234-123456780001", "87654321-4321-4321-4321-cba987654321"};

/**
 * Helper Class for Provisioning Manager
 */
class SecurityCommonUtil
{

public:

    SecurityCommonUtil();

    static FILE* fopenProvManager(const char*, const char*);

    OicSecAcl_t* createAcl(const int dev_num, int permission, OCProvisionDev_t** m_own_list,
            bool multiple_dev = true);

    int waitCallbackRet(void);

    bool initProvisionClient(int clientOTMType = OTM_ALL, char* chDBPath = (char*) PM_CLIENT_DB);
    bool setOxmAllowStatus(const OicSecOxm_t oxm, const bool allowStatus,
            OCStackResult expectedResult);

    bool discoverAllDevices(int nTime, OCProvisionDev_t** own_list, OCProvisionDev_t** unown_list,
            OCStackResult expectedResult);

    bool discoverUnownedDevices(int nTime, OCProvisionDev_t** unown_list,
            OCStackResult expectedResult);

    bool discoverOwnedDevices(int nTime, OCProvisionDev_t** own_list, OCStackResult expectedResult);

    bool doOwnerShipTransfer(void *ctx, OCProvisionDev_t** unown_list,
            OCProvisionResultCB resultCallback, OCStackResult expectedResult, bool checkCallback =
            true);

    bool provisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize,
            const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2,
            OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback,
            OCStackResult expectedResult);

    bool deleteDiscoveredDevices(OCProvisionDev_t *pList);
    bool deleteUuidList(OCUuidList_t* pList);
    bool deleteACLList(OicSecAcl_t* pList);

    static void ownershipTransferCB(void*, size_t, OCProvisionResult_t*, bool);
    static void provisionCB(void*, size_t, OCProvisionResult_t*, bool);

    static bool convertStrToUuid(std::string uuid, OicUuid_t* deviceID,
            OCStackResult expectedResult);

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

    static void pushDevices(OCProvisionDev_t** head_ref, OCProvisionDev_t *new_node);

    static OCProvisionDev_t* filterDevices(OCProvisionDev_t *deviceList);

    static bool filterDevicesByIp(std::string ip);

    static std::string exec(const char* cmd);

    OTMCallbackData_t otmCbRegister(int otmType);
};
#endif
#endif //#ifdef __SECURED__
