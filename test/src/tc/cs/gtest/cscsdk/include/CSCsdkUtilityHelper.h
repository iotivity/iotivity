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

#ifndef CSCsdkUtilityHelper_H_
#define CSCsdkUtilityHelper_H_

#ifdef __GNUC__
#pragma GCC system_header
#endif

#include <stddef.h>
#include <string>

#include "casecurityinterface.h"
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
#include "IotivityTest_Logger.h"

using namespace std;
using namespace OC;

#define DASH '-'

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
#define CRED_ID_NEGATIVE -1

/*
 * Callback Releated Resources
 */
#define CALLBACK_TIMEOUT    60  // 1 min
#define CALLBACK_INVOKED 1
#define CALLBACK_NOT_INVOKED 0


/**
 * CLoud Acl Resource
 */
#define CLOUD_ACL_CONTROLLER_DAT "./cloud.dat"
#define CLOUD_ACL_CONTROLLER_DAT_BACKUP "../cloud.dat"
#define ROOT_CERT_FILE "rootca.crt"
#define ROOT_CERT_FILE_BACKUP "../rootca.crt"

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 5683
#define DEFAULT_AUTH_PROVIDER "github"
#define CLOUD_ACL_CONTROLLER_DAT "./cloud.dat"
#define DEFAULT_ACL_ID "5736bf74-b767-425f-bc31-3cae0870c7e2"
#define DEFAULT_DEV_ID "9cfbeb8e-5a1e-4d1c-9d01-2ae6fdb"
#define DEFAULT_OWNER_ID "1123e4567-e89b-12d3-a456-4266554"

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

class CSCsdkUtilityHelper
{

public:

    static OCDevAddr getOCDevAddrEndPoint();
    static cloudAce_t* createCloudAces();
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

#endif /* CSCsdkUtilityHelper_H_ */
