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

#ifndef CSCppCloudHelper_H_
#define CSCppCloudHelper_H_

#if defined(__CLOUD__)

#include "CSCppUtilityHelper.h"

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

FILE* controleeOpen(const char * /*path*/, const char *mode);
bool readFile(const char *name, OCByteString *out);

/**
 * Helper Class for Provisioning Manager
 */
class CSCppCloudHelper
{
private:
    std::string m_failureMessage;

public:

    CSCppCloudHelper();

    bool initCloudACLClient();
    bool signUp(OCAccountManager::Ptr accountMgr, std::string authprovider, std::string authCode,
            PostCallback cloudConnectHandler, std::string &devID, std::string &devAccessToken,
            std::string &devRefreshToken, OCStackResult expectedResult);
    bool signIn(OCAccountManager::Ptr accountMgr, const std::string& userUuid,
            const std::string& accessToken, PostCallback cloudConnectHandler,
            OCStackResult expectedResult);
    bool signOut(OCAccountManager::Ptr accountMgr, PostCallback cloudConnectHandler,
            OCStackResult expectedResult);

    bool saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize,
            OicEncodingType_t encodingType, uint16_t *credId, OCStackResult expectedResult);

    bool requestCertificate(OCCloudProvisioning& ptr, ResponseCallBack callback,
            OCStackResult expectedResult);
    bool getAclIdByDevice(OCCloudProvisioning& ptr, const std::string deviceId,
            AclIdResponseCallBack callback, std::string &aclId, OCStackResult expectedResult);
    bool getIndividualAclInfo(OCCloudProvisioning& ptr, const std::string aclId,
            ResponseCallBack callback, OCStackResult expectedResult);
    bool getCRL(OCCloudProvisioning& ptr, ResponseCallBack callback, OCStackResult expectedResult);
    bool postCRL(OCCloudProvisioning& ptr, const std::string thisUpdate, const std::string nextUpdate,
            const OCByteString *crl, const stringArray_t *serialNumbers, ResponseCallBack callback,
            OCStackResult expectedResult);

    static void handleLoginoutCB(const HeaderOptions &, const OCRepresentation &rep,
            const int ecode);
    static void provisionResultCB(void* ctx, int nOfRes, OCProvisionResult_t* arr, bool hasError);
    static void cloudResponseCB(OCStackResult result, void* data);
    static void cloudAclResponseCB(OCStackResult result, std::string aclID);
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
#endif /*__CLOUD__*/
#endif /*PMCsdkCloudHelper_H_*/
