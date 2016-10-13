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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include "logger.h"

#include "ocprovisioningmanager.h"
#include "oxmjustworks.h"
#include "oxmrandompin.h"
#include "securevirtualresourcetypes.h"
#include "srmutility.h"
#include "pmtypes.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "utlist.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ocstack.h"
#include "logger.h"
#include "camutex.h"
#include "cathreadpool.h"
#include "ocpayload.h"
#include "payload_logging.h"
#include "ocprovisioningmanager.h"

#include "utils.h"


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

/**
 * CLoud Acl Resource
 */
#define CLOUD_ACL_CONTROLLER_DAT "./cloud.dat"
#define CLOUD_ACL_CONTROLLER_DAT_BACKUP "../cloud.dat"
#define ROOT_CERT_FILE "rootca.crt"
#define ROOT_CERT_FILE_BACKUP "../rootca.crt"


const string HOST_ADDRESS = "coap+tcp://127.0.0.1:5683";
const string g_authProvider = "github";
const string CRL_THIS_UPDATE = "201712010000000";
const string CRL_NEXT_DATE = "20171202000000";

#define DEFAULT_HOST "127.0.0.1"//"127.0.0.1"
#define DEFAULT_PORT 5683
#define DEFAULT_AUTH_PROVIDER "github"

#define DEFAULT_DEV_ID "9cfbeb8e-5a1e-4d1c-9d01-2ae6fdb"
#define DEFAULT_ACL_ID "020afada-894f-4b0c-a59d-1702c1e7de8e"
#define DEFAULT_OWNER_ID "1123e4567-e89b-12d3-a456-4266554"
#define SUBJECT_ID_EXAMPLE "72616E64-5069-6E44-6576-557569643030"

#define CTX_CERT_REQ_ISSUE "Cert Request Context"
#define CTX_GET_ACL_ID_BY_DEV "Get Acl Id By Dev Context"
#define CTX_INDIVIDUAL_GET__INFO "Individual Get Info"
#define CTX_INDIVIDUAL_UPDATE_ACE "Individual Update Ace"
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

// CERT
#define CRED_ID_ONE 1

class PMCppUtilityHelper
{

public:

    static OCDevAddr getOCDevAddrEndPoint();
    static char *getOCStackResult(OCStackResult ocstackresult);
    static OCProvisionDev_t* getDevInst(OCProvisionDev_t* dev_lst, const int dev_num);
    static int printDevList(OCProvisionDev_t*);
    static int printResultList(const OCProvisionResult_t*, const int);
    static int waitCallbackRet(void);
    static size_t printUuidList(const OCUuidList_t*);
    static void printUuid(const OicUuid_t*);
};

#endif /* PMCppUtilityHelper_H_ */
