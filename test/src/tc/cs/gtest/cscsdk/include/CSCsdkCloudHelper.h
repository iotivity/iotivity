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

#ifndef CSCsdkCloudHelper_H_
#define CSCsdkCloudHelper_H_

#if defined(__CLOUD__)
#include "CSCsdkUtilityHelper.h"

using namespace OC;
using namespace std;

#define TAG "cloudAuth"
#define NONE -1
#define DEFAULT_PRESENCE_TTL 60
#define DEFAULT_CONTEXT_VALUE 0x99
#define DEVICE_TYPE_VALUE "oic.d.binarySwitch"
#define RES_DEVICE_NAME "FAC_2016"
#define SWITCH_RES_URI "/power/0"
#define SWITCH_RES_TYPE "oic.r.switch.binary"
#define QUERY_FOR_SWITCH "/oic/res"

FILE* controleeOpen(const char * /*path*/, const char *mode);
bool readFile(const char *name, OCByteString *out);

/**
 * Helper Class for Provisioning Manager
 */
class CSCsdkCloudHelper
{
private:
    std::string m_failureMessage;

public:

    int m_expiresin;
    std::string m_accesstoken;
    std::string m_redirecturi;
    std::string m_refreshtoken;
    std::string m_sid;
    std::string m_tokentype;
    std::string m_uid;

    CSCsdkCloudHelper();

    bool initCloudACLClient();
    bool signUp(OCAccountManager::Ptr accountMgr, std::string authprovider, std::string authCode,
            PostCallback cloudConnectHandler, std::string &uid, std::string &devAccessToken,
            std::string &devRefreshToken, OCStackResult expectedResult);
    bool signIn(OCAccountManager::Ptr accountMgr, const std::string& userUuid,
            const std::string& accessToken, PostCallback cloudConnectHandler,
            OCStackResult expectedResult);
    bool signOut(OCAccountManager::Ptr accountMgr,
            const std::string accessToken, PostCallback cloudConnectHandler,
            OCStackResult expectedResult);

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
            OCCloudResponseCB callback, std::string &devAclID, OCStackResult expectedResult, bool checkCallback = true);
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
    static void aclResponseCB(void* ctx, OCStackResult result, void* data);
    static void onPublish(const OCRepresentation &, const int &eCode);

    /**
     * All Utility Methods for Provision Manager
     */
    static void printRepresentation(OCRepresentation rep);
    static int waitCallbackRet();
    std::string getFailureMessage();
};
#endif /*__CLOUD*/
#endif /*CSCsdkCloudHelper_H_*/
