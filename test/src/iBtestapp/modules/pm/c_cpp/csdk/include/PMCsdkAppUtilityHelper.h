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
#include "logger.h"
#include "platform_features.h"

// Provisioning
#include "oxmjustworks.h"
#include "oxmrandompin.h"
#include "securevirtualresourcetypes.h"
#include "srmutility.h"
#include "pmtypes.h"
#include "pmutility.h"
#include "ocprovisioningmanager.h"
#include "oxmverifycommon.h"

#include "CommonUtil.h"
#include "CommonProperties.h"
#include "IotivityTest_Logger.h"

/**
 * Secured Resource
 */
//static const OicSecPrm_t SUPPORTED_PRMS[1] =
//{ PRM_PRE_CONFIGURED, };
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

/**
 *  Server and Client Resources
 */
#define KILL_SERVERS "./iotivity_pm_server"

#define SVR_DB_FILE_NAME "oic_svr_db_client.dat"
#define PRVN_DB_FILE_NAME "oic_prvn_mng.db"
#define JUSTWORKS_SERVER1 "./iotivity_pm_server oic_svr_db_server.dat 1"
#define JUSTWORKS_SERVER2 "./iotivity_pm_server oic_svr_db_server_justworks.dat 1"
#define RANDOMPIN_SERVER "./iotivity_pm_server oic_svr_db_server_randompin.dat 2"
#define MVJUSTWORKS_SERVER "./iotivity_pm_server oic_svr_db_server_mvjustworks.dat 5"

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
#define JUSTWORKS_SERVER_DP "./iotivity_pm_server justworks_server_dp_c.dat 1"
#endif /*__DIRECTPAIRING__*/

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

#define MVJUSTWORKS_SERVER_CBOR "./oic_svr_db_server_mvjustworks.dat"
#define MVJUSTWORKS_SERVER_CBOR_BACKUP "../oic_svr_db_server_mvjustworks.dat"

#define MOT_CTX "MOT Context"
#define MOT_CBOR_FILE "oic_svr_db_subowner_client.dat"
#define MOT_PRVN_DB_FILE_NAME "oic_pdm_subowner.db"
#define MOT_CLIENT_CBOR "./oic_svr_db_subowner_client.dat"
#define MOT_CLIENT_CBOR_BACKUP "../oic_svr_db_subowner_client.dat"
#define MOT_CLIENT_DATABASE "./oic_pdm_subowner.db"
#define MOT_DEFAULT_PRE_CONFIG_PIN "12341234"
#define OIC_MULTIPLE_OWNER_UOBV 10
#define MOT_LED_RESOURCE "/a/led"

/**
 * CLoud Acl Resource
 */
#define CLOUD_ACL_CONTROLLER_DAT "./cloud.dat"
#define CLOUD_ACL_CONTROLLER_DAT_BACKUP "../cloud.dat"
#define ROOT_CERT_FILE "rootca.crt"
#define ROOT_CERT_FILE_BACKUP "../rootca.crt"

#define CTX_CERT_REQ_ISSUE "Cert Request Context"
#define CTX_GET_ACL_ID_BY_DEV "Get Acl Id By Dev Context"
#define CTX_INDIVIDUAL_GET__INFO "Individual Get Info"
#define CTX_INDIVIDUAL_UPDATE_ACE "Individual Update Ace"
#define CTX_PROV_TRUST_CERT "Provision Trust Cert"
#define CTX_GET_CRL "Get CRL"

OCDevAddr getOCDevAddrEndPoint();
char *getOCStackResult(OCStackResult ocstackresult);
OCProvisionDev_t* getDevInst(OCProvisionDev_t* dev_lst, const int dev_num);
int printDevList(OCProvisionDev_t*);
int printResultList(const OCProvisionResult_t*, const int);
int waitCallbackRet(void);
size_t printUuidList(const OCUuidList_t*);
void printUuid(const OicUuid_t*);
void removeAllResFile(int resFile);
void copyAllResFile(int resFile);

std::string setFailureMessage(OCStackResult actualResult, OCStackResult expectedResult);
std::string setFailureMessage(std::string errorMessage);

#endif /* PMCsdkUtilityHelper_H_ */
