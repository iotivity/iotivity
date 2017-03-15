/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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

#ifndef CSCppUtilityHelper_H_
#define CSCppUtilityHelper_H_

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
#include "ssl_ciphersuites.h"
#include "srmutility.h"
#include "utils.h"
#include "utlist.h"

#include "CommonUtil.h"
#include "CommonTestUtil.h"
#include "CloudCommonUtil.h"
#include "CommonProperties.h"
#include "IotivityTest_Logger.h"

using namespace std;
using namespace OC;

#define TAG "CS Cpp"

/**
 * Secured Resource
 */

#define KILL_SERVERS "server"

#define CTX_CERT_REQ_ISSUE "Cert Request Context"
#define CTX_GET_ACL_ID_BY_DEV "Get Acl Id By Dev Context"
#define CTX_INDIVIDUAL_GET_INFO "Individual Get Info"
#define CTX_INDIVIDUAL_UPDATE_ACE "Individual Update Ace"
#define CTX_PROV_TRUST_CERT "Provision Trust Cert"
#define CTX_GET_CRL "Get CRL"
#define CTX_CREATE_GROUP "Create Group"
#define CTX_FIND_GROUP "Find Group"
#define CTX_DELETE_GROUP "Delete Group"
#define CTX_GET_GROUP_INFO "Get Group Info"
#define CTX_INVITE_USER "Invite User in Group"
#define CTX_SHARE_DEVICE "Share Device"
#define CTX_DELETE_SHARED_DEVICE "Delete Shared Device"
#define CTX_GET_GROUP_INVITATION "Get Group Invitation"
#define CTX_DELETE_INVITATION "Delete Invitation"
#define CTX_JOIN_GROUP "Join Group"
#define CTX_OBSERVER_GROUP "Observe Group"
#define ERROR_SIGN_IN "Sign In Fail"

/*
 * ACL Related Resources
 */
#define FULL_PERMISSION 31
#define DEFAULT_PERMISSION 31
#define NO_PERMISSION 0
#define MAX_PERMISSION_RANGE 65535

#define ACL_RESOURCE_URI "sensor/light"
#define ACL_RESOURCE_LENGTH 6 // Check ACL_RESOURCE_NAME for the length. ACL_RESOURCE_LENGTH = len(ACL_RESOURCE_URI) + 1
#define ACL_RES_TYPE_NAME "light"
#define ACL_RES_IF_TYPE_NAME "sensor"

/*
 * Aces
 */
#define MAX_ID_LENGTH       (64)
#define MAX_STRING_LENGTH   (256)
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

class CSCppUtilityHelper
{

public:
    static OCDevAddr getOCDevAddrEndPoint();

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
