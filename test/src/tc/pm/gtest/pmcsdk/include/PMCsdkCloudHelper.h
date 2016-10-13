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

#ifndef PMCsdkCloudHelper_H_
#define PMCsdkCloudHelper_H_

#if defined(__CLOUD__)
#include <memory>
#include <iostream>
#include <stdexcept>
#include <condition_variable>
#include <map>
#include <vector>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include "logger.h"
#include "ocstack.h"
#include "rd_client.h"
#include "OCApi.h"
#include "OCPlatform.h"
#include "OCPlatform_impl.h"
#include "camutex.h"
#include "cathreadpool.h"
#include "ocpayload.h"
#include "payload_logging.h"
#include "occloudprovisioning.h"
#include "ocprovisioningmanager.h"
#include "utils.h"
#include "cloudAuth.h"
#include "cloudWrapper.h"
#include "cloudDiscovery.h"
#include "OCAccountManager.h"
#include "OCApi.h"

#include "BinarySwitchResourceHelper.h"
#include "CloudResourceHelper.h"

using namespace OC;
using namespace std;

#define TAG "cloudAuth"
#define CONTROLEE 0
#define CONTROLLER 1
#define NONE -1
#define DEFAULT_PRESENCE_TTL 60
#define DEFAULT_CONTEXT_VALUE 0x99
#define DEVICE_TYPE_VALUE "oic.d.binarySwitch"
#define RES_DEVICE_NAME "FAC_2016"
#define SWITCH_RES_URI "/power/0"
#define SWITCH_RES_TYPE "oic.r.switch.binary"
#define QUERY_FOR_SWITCH "/oic/res"
#define PRVN_DB_FILE_NAME "oic_cloud_mng.db"
#define CLIENT_DATABASE "./oic_cloud_mng.db"
#define JUSTWORKS_SERVER_DP "./sampleserver_justworks 6"
#define KILL_JUSTWORKS_SERVER_DP "./sampleserver_justworks"
#define JUSTWORKS_SERVER_DP_C_DAT "./justworks_server_dp_c.dat"
#define DIRECT_PAIRING_CLIENT_C_DAT "./direct_pairing_c.dat"
#define JUSTWORKS_SERVER_DP_DAT_C_BACKUP "../justworks_server_dp_c.dat"
#define DIRECT_PAIRING_CLIENT_DAT_C_BACKUP "../direct_pairing_c.dat"

FILE* controleeOpen(const char * /*path*/, const char *mode);

/**
 * Helper Class for Provisioning Manager
 */
class PMCsdkCloudHelper
{
private:
    std::string m_failureMessage;

public:

    PMCsdkCloudHelper();

    bool initCloudACLClient();
    bool signUp(OCAccountManager::Ptr accountMgr, std::string authprovider, std::string authCode,
            PostCallback cloudConnectHandler, std::string &devID, std::string &devAccessToken,
            std::string &devRefreshToken, OCStackResult expectedResult);
    bool signIn(OCAccountManager::Ptr accountMgr, const std::string& userUuid,
            const std::string& accessToken, PostCallback cloudConnectHandler,
            OCStackResult expectedResult);
    bool signOut(OCAccountManager::Ptr accountMgr, PostCallback cloudConnectHandler,
            OCStackResult expectedResult);
    bool publishResourceToRD(std::string host, OCConnectivityType connectivityType,
            PublishResourceCallback callback, OCStackResult expectedResult);

    bool cloudCertificateIssueRequest(void* ctx, const OCDevAddr *endPoint,
            OCCloudResponseCB callback, OCStackResult expectedResult, bool checkCallback = true);
    bool provisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId,
            const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback,
            OCStackResult expectedResult);
    bool saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize,
            OicEncodingType_t encodingType, uint16_t *credId, OCStackResult expectedResult);

    bool cloudAclIdCreate(void* ctx, const char *ownerId, const char *deviceId,
            const OCDevAddr *endPoint, OCCloudResponseCB callback, OCStackResult expectedResult, bool checkCallback = true);
    bool cloudAclIdGetByDevice(void* ctx, const char *deviceId, const OCDevAddr *endPoint,
            OCCloudResponseCB callback, OCStackResult expectedResult, bool checkCallback = true);
    bool cloudAclIndividualGetInfo(void* ctx, const char *aclId, const OCDevAddr *endPoint,
            OCCloudResponseCB callback, OCStackResult expectedResult, bool checkCallback = true);
    bool cloudAclIndividualUpdateAce(void* ctx, const char *aclId, const cloudAce_t *aces,
            const OCDevAddr *endPoint, OCCloudResponseCB callback, OCStackResult expectedResult, bool checkCallback = true);

    bool cloudGetCRL(void* ctx, const OCDevAddr *endPoint, OCCloudResponseCB callback,
            OCStackResult expectedResult, bool checkCallback = true);

    static void handleLoginoutCB(const HeaderOptions &, const OCRepresentation &rep,
            const int ecode);
    static void provisionResultCB(void* ctx, int nOfRes, OCProvisionResult_t* arr, bool hasError);
    static void cloudResponseCB(void* ctx, OCStackResult result, void* data);
    static void onPublish(const OCRepresentation &, const int &eCode);
    static ByteArray getTrustCertChainArray();

    /**
     * All Utility Methods for Provision Manager
     */
    static int waitCallbackRet();
    std::string getFailureMessage();
    std::string setFailureMessage(OCStackResult actualResult, OCStackResult expectedResult);
    std::string setFailureMessage(std::string errorMessage);
};
#endif /*__CLOUD*/
#endif /*PMCsdkCloudHelper_H_*/
