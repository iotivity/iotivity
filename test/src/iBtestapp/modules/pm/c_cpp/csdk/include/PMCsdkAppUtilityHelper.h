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

#ifndef PMCsdkAppUtilityHelper_H_
#define PMCsdkAppUtilityHelper_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <sstream>

// OCStack
#include "oic_malloc.h"
#include "oic_string.h"
#include "utlist.h"
#include "ocstack.h"
#include "ocpayload.h"

// Provisioning
#include "oxmjustworks.h"
#include "oxmrandompin.h"
#include "securevirtualresourcetypes.h"
#include "srmutility.h"
#include "pmtypes.h"
#include "ocprovisioningmanager.h"

#include "CommonUtil.h"
#include "IotivityTest_Logger.h"

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
#define CRED_ID_NEGATIVE -1;

/*
 * Callback Releated Resources
 */
#define CALLBACK_TIMEOUT    60  // 1 min
#define CALLBACK_INVOKED 1
#define CALLBACK_NOT_INVOKED 0

/**
 * Secured Resource
 */
//static const OicSecPrm_t SUPPORTED_PRMS[1] =
//{ PRM_PRE_CONFIGURED, };
/**
 *  Server and Client Resources
 */
#define KILL_SERVERS "./test_server"

#define SVR_DB_FILE_NAME "oic_svr_db_client.dat"
#define PRVN_DB_FILE_NAME "oic_prvn_mng.db"
#define JUSTWORKS_SERVER1 "./test_server oic_svr_db_server.dat 1"
#define JUSTWORKS_SERVER2 "./test_server oic_svr_db_server_justworks.dat 1"
#define  RANDOMPIN_SERVER "./test_server oic_svr_db_server_randompin.dat 2"

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

/*
 * Direct Pairing Resource
 */
#if defined(__DIRECTPAIRING__)
#define JUSTWORKS_SERVER_DP_C_DAT "./justworks_server_dp_c.dat"
#define JUSTWORKS_SERVER_DP_DAT_C_BACKUP "../justworks_server_dp_c.dat"
#define DIRECT_PAIRING_CLIENT_C_DAT "./direct_pairing_c.dat"
#define DIRECT_PAIRING_CLIENT_DAT_C_BACKUP "../direct_pairing_c.dat"
#define JUSTWORKS_SERVER_DP "./test_server justworks_server_dp_c.dat 1"
#endif /*__DIRECTPAIRING__*/

/**
 * MOT Resources
 */
#define JUSTWORKS_SERVER "./sampleserver_justworks"
#define JUSTWORKS_SERVER7 "./test_server mot_preconfig_pin_server.dat 1"
#define JUSTWORKS_SERVER7_CBOR "./mot_preconfig_pin_server.dat"
#define JUSTWORKS_SERVER7_CBOR_BACKUP "../mot_preconfig_pin_server.dat"
#define JUSTWORKS_SERVER2_CBOR "./oic_svr_db_server_justworks.dat"
#define JUSTWORKS_SERVER2_CBOR_BACKUP "../oic_svr_db_server_justworks.dat"

#define MOT_CTX "MOT Context"
#define MOT_CBOR_FILE "oic_svr_db_subowner_client.dat"
#define MOT_PRVN_DB_FILE_NAME "oic_pdm_subowner.db"
#define MOT_CLIENT_CBOR "./oic_svr_db_subowner_client.dat"
#define MOT_CLIENT_CBOR_BACKUP "../oic_svr_db_subowner_client.dat"
#define MOT_CLIENT_DATABASE "./oic_pdm_subowner.db"
#define MOT_DEFAULT_PRE_CONFIG_PIN "12341234"
#define OIC_MULTIPLE_OWNER_UOBV 10

/**
 * CLoud Acl Resource
 */
#define CLOUD_ACL_CONTROLLER_DAT "./cloud.dat"
#define CLOUD_ACL_CONTROLLER_DAT_BACKUP "../cloud.dat"
#define ROOT_CERT_FILE "rootca.crt"
#define ROOT_CERT_FILE_BACKUP "../rootca.crt"

#define DEFAULT_HOST "127.0.0.1"//"127.0.0.1"
#define DEFAULT_PORT 5683
#define DEFAULT_AUTH_PROVIDER "github"
#define CLOUD_ACL_CONTROLLER_DAT "./cloud.dat"
#define ROOT_CERT_FILE "rootca.crt"
#define DEFAULT_ACL_ID "a67f3f95-162a-45d7-aea1-1c43a0a019fb"

#define DEFAULT_DEV_ID "9cfbeb8e-5a1e-4d1c-9d01-2ae6fdb"
#define DEFAULT_OWNER_ID "1123e4567-e89b-12d3-a456-4266554"
#define UUID_EXAMPLE_3 "987e6543-e21b-12d3-a456-4266554"
#define SUBJECT_ID_EXAMPLE "72616E64-5069-6E44-6576-557569643030"

#define CTX_CERT_REQ_ISSUE "Cert Request Context"
#define CTX_GET_ACL_ID_BY_DEV "Get Acl Id By Dev Context"
#define CTX_INDIVIDUAL_GET__INFO "Individual Get Info"
#define CTX_INDIVIDUAL_UPDATE_ACE "Individual Update Ace"
#define CTX_PROV_TRUST_CERT "Provision Trust Cert"
#define CTX_GET_CRL "Get CRL"

/*
 * Aces
 */
#define MAX_ID_LENGTH       (64)
#define MAX_STRING_LENGTH   (256)
#define UUID_EXAMPLE_1 "9cfbeb8e-5a1e-4d1c-9d01-2ae6fdb"
#define UUID_EXAMPLE_2 "123e4567-e89b-12d3-a456-4266554"
#define UUID_EXAMPLE_3 "987e6543-e21b-12d3-a456-4266554"
#define SUBJECT_ID_EXAMPLE "72616E64-5069-6E44-6576-557569643030"

#define ACL_ID_EXAMPLE "0b3fe9e1-9173-4ad2-af4d-a060cf08fef5"
#define ACE_ID_EXAMPLE "test1"

#define ID_EXAMPLE_1   "78f98b4f25f21e2487e8"
#define ID_EXAMPLE_2   "6caa7009386290fd3681"

#define RESOURCE_URI_EXAMPLE "/a/light/0"
#define RESOURCE_TYPE_EXAMPLE "core.light"
#define INTERFACE_EXAMPLE "oic.if.baseline"

OCDevAddr getOCDevAddrEndPoint();
char *getOCStackResult(OCStackResult ocstackresult);
OCProvisionDev_t* getDevInst(OCProvisionDev_t* dev_lst, const int dev_num);
int printDevList(OCProvisionDev_t*);
int printResultList(const OCProvisionResult_t*, const int);
int waitCallbackRet(void);
size_t printUuidList(const OCUuidList_t*);
void printUuid(const OicUuid_t*);

std::string setFailureMessage(OCStackResult actualResult, OCStackResult expectedResult);
std::string setFailureMessage(std::string errorMessage);

#endif /* PMCsdkUtilityHelper_H_ */
