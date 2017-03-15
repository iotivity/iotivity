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
#include "CSCsdkCloudHelper.h"
#include "CSCsdkUtilityHelper.h"

int CSCsdkCloudHelper::s_isCbInvoked = CALLBACK_NOT_INVOKED;
std::string CSCsdkCloudHelper::s_uid = "";
std::string CSCsdkCloudHelper::s_accesstoken = "";
std::string CSCsdkCloudHelper::s_refreshToken = "";
std::string CSCsdkCloudHelper::s_groupId = "";
std::string CSCsdkCloudHelper::s_aclResponse = "";
OCPersistentStorage CSCsdkCloudHelper::s_pst = {0, 0, 0, 0, 0};

int CSCsdkCloudHelper::waitCallbackRet()
{
    IOTIVITYTEST_LOG(DEBUG, "Waiting for Callback to be invoked");

    for (int i = 0; CALLBACK_TIMEOUT > i; ++i)
    {
        if (CALLBACK_INVOKED == s_isCbInvoked)
        {
            return CALLBACK_INVOKED;
        }

        sleep (DELAY_SHORT);
        printf("Second Elapsed : %d seconds\n", i);

        if (OC_STACK_OK != OCProcess())
        {
            printf("OCStack process error\n");
            return CALLBACK_NOT_INVOKED;
        }
    }

    return CALLBACK_NOT_INVOKED;
}

void CSCsdkCloudHelper::printRepresentation(OCRepresentation rep)
{
    for (auto itr = rep.begin(); itr != rep.end(); ++itr)
    {
        cout << "\t" << itr->attrname() << ":\t" << itr->getValueToString() << endl;

        if (itr->attrname().compare("accesstoken") == 0)
        {
            s_accesstoken = itr->getValueToString();
        }

        if (itr->attrname().compare("uid") == 0)
        {
            s_uid = itr->getValueToString();
        }

        if (itr->attrname().compare("gid") == 0)
        {
            s_groupId = itr->getValueToString();
        }
    }
}

FILE* CSCsdkCloudHelper::controleeOpen(const char * /*path*/, const char *mode)
{
    return fopen(CLOUD_ACL_CONTROLLER_DAT, mode);
}

CSCsdkCloudHelper::CSCsdkCloudHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[CSCsdkCloudHelper] Constructor Called");
}

void CSCsdkCloudHelper::handleLoginoutCB(const HeaderOptions &, const OCRepresentation &rep,
        const int ecode)
{
    IOTIVITYTEST_LOG(INFO, "Auth response received code: %d", ecode);
    if (rep.getPayload() != NULL)
    {
        printRepresentation(rep);
    }

    s_isCbInvoked = CALLBACK_INVOKED;
}

void CSCsdkCloudHelper::onPublish(const OCRepresentation &, const int &eCode)
{
    IOTIVITYTEST_LOG(DEBUG, "Publish resource response received code: %d", eCode);
    s_isCbInvoked = CALLBACK_INVOKED;
}

bool CSCsdkCloudHelper::initCloudACLClient()
{
    __FUNC_IN__

    CSCsdkCloudHelper::s_pst.open = CSCsdkCloudHelper::controleeOpen;
    CSCsdkCloudHelper::s_pst.read = fread;
    CSCsdkCloudHelper::s_pst.write = fwrite;
    CSCsdkCloudHelper::s_pst.close = fclose;
    CSCsdkCloudHelper::s_pst.unlink = unlink;

    if (OC_STACK_OK != OCRegisterPersistentStorageHandler(&CSCsdkCloudHelper::s_pst))
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] OCRegisterPersistentStorageHandler error");
        return false;
    }

    // initialize OC stack and provisioning manager
    if (OC_STACK_OK != OCInit(NULL, 0, OC_CLIENT_SERVER))
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] OCStack init error");
        return false;
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::signUp(OCAccountManager::Ptr accountMgr, std::string authprovider,
        std::string authCode, PostCallback cloudConnectHandler, std::string &uid,
        std::string &devAccessToken, std::string &devRefreshToken, OCStackResult expectedResult)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = accountMgr->signUp(authprovider, authCode, cloudConnectHandler);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] signUp returns %s", CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    uid = s_uid;
    devAccessToken = s_accesstoken;
    OC_UNUSED(devRefreshToken);
    //devRefreshToken = s_refreshToken; /* Will be enabled if a cloud Service that returns refresh token is available*/
    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::signIn(OCAccountManager::Ptr accountMgr, const std::string& userUuid,
        const std::string& accesstoken, PostCallback cloudConnectHandler,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    s_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->signIn(userUuid, accesstoken, cloudConnectHandler);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] signIn returns %s", CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::signOut(OCAccountManager::Ptr accountMgr, const std::string accessToken,
        PostCallback cloudConnectHandler, OCStackResult expectedResult)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->signOut(accessToken, cloudConnectHandler);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] signIn returns %s", CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::provisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId,
        const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback,
        OCStackResult expectedResult)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = OCProvisionTrustCertChain(ctx, type, credId, selectedDeviceInfo,
            resultCallback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] provisionTrustCertChain returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::readFile(const char *name, OCByteString *out)
{
    FILE *file = NULL;
    int length = 0;
    uint8_t *buffer = NULL;
    bool result = false;
    size_t count, realCount;
    //Open file
    file = fopen(name, "rb");
    if (!file)
    {
        OIC_LOG_V(ERROR, TAG, "Unable to open file %s", name);
        return result;
    }

    //Get file length
    if (fseek(file, 0, SEEK_END))
    {
        OIC_LOG(ERROR, TAG, "Failed to SEEK_END");
        goto exit;
    }

    length = ftell(file);
    if (length < 0)
    {
        OIC_LOG(ERROR, TAG, "Failed to ftell");
        goto exit;
    }

    if (fseek(file, 0, SEEK_SET))
    {
        OIC_LOG(ERROR, TAG, "Failed to SEEK_SET");
        goto exit;
    }

    //Allocate memory
    buffer = (uint8_t *) malloc(length);
    if (!buffer)
    {
        OIC_LOG(ERROR, TAG, "Failed to allocate buffer");
        goto exit;
    }

    //Read file contents into buffer
    count = 1;
    realCount = fread(buffer, length, count, file);
    if (realCount != count)
    {
        OIC_LOG_V(ERROR, TAG, "Read %d bytes %zu times instead of %zu", length, realCount, count);
        goto exit;
    }

    out->bytes = buffer;
    out->len = length;

    result = true;
    exit: fclose(file);
    return result;
}

bool CSCsdkCloudHelper::saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize,
        OicEncodingType_t encodingType, uint16_t *credId, OCStackResult expectedResult)
{
    __FUNC_IN__

    OIC_LOG_BUFFER(DEBUG, "CLOUD ACL", trustCertChain, chainSize);
    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCSaveTrustCertChain(trustCertChain, chainSize, encodingType, credId);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCSaveTrustCertChain returns %s",
            CommonUtil::getOCStackResult(result));

    IOTIVITYTEST_LOG(INFO, "CredId of Saved Trust Cert. Chain into Cred of SVR : %d", *credId);

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::cloudCertificateIssueRequest(void* ctx, const OCDevAddr *endPoint,
        OCCloudResponseCB callback, OCStackResult expectedResult, bool checkCallback)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = OCCloudCertificateIssueRequest(ctx, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudCertificateIssueRequest returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::cloudGetCRL(void* ctx, const OCDevAddr *endPoint,
        OCCloudResponseCB callback, OCStackResult expectedResult, bool checkCallback)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudGetCRL(ctx, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudGetCRL returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::cloudPostCRL(void* ctx, const char *thisUpdate, const char *nextUpdate,
        const OCByteString *crl, const stringArray_t *serialNumbers, const OCDevAddr *endPoint,
        OCCloudResponseCB callback, OCStackResult expectedResult, bool checkCallback)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudPostCRL(ctx, thisUpdate, nextUpdate, crl, serialNumbers, endPoint,
            callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudPostCRL returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::cloudAclIdCreate(void* ctx, const char *ownerId, const char *deviceId,
        const OCDevAddr *endPoint, OCCloudResponseCB callback, OCStackResult expectedResult,
        bool checkCallback)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclIdCreate(ctx, ownerId, deviceId, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclIdCreate returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::cloudAclIdDelete(void* ctx, const char *aclId, const OCDevAddr *endPoint,
        OCCloudResponseCB callback, OCStackResult expectedResult, bool checkCallback)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclIdDelete(ctx, aclId, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclIdDelete returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::cloudAclIdGetByDevice(void* ctx, const char *deviceId,
        const OCDevAddr *endPoint, OCCloudResponseCB callback, std::string &devAclID,
        OCStackResult expectedResult, bool checkCallback)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudGetAclIdByDevice(ctx, deviceId, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] cloudAclIdGetByDevice returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }

        devAclID = s_aclResponse;

        if ("" == devAclID)
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Retrieving ACL ID failed");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::cloudAclIndividualGetInfo(void* ctx, const char *aclId,
        const OCDevAddr *endPoint, OCCloudResponseCB callback, OCStackResult expectedResult,
        bool checkCallback)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclIndividualGetInfo(ctx, aclId, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclIndividualGetInfo returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::cloudAclIndividualUpdateAce(void* ctx, const char *aclId,
        const cloudAce_t *aces, const OCDevAddr *endPoint, OCCloudResponseCB callback,
        OCStackResult expectedResult, bool checkCallback)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclIndividualUpdateAce(ctx, aclId, aces, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclIndividualUpdateAce returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::cloudAclCreateGroup(void* ctx, const char *groupType,
        const char *groupMasterId, const OCDevAddr *endPoint, OCCloudResponseCB callback,
        std::string &groupId, OCStackResult expectedResult, bool checkCallback)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclCreateGroup(ctx, groupType, groupMasterId, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclCreateGroup returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }

        groupId = s_groupId;
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::cloudAclFindMyGroup(void* ctx, const char *memberId,
        const OCDevAddr *endPoint, OCCloudResponseCB callback, OCStackResult expectedResult,
        bool checkCallback)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclFindMyGroup(ctx, memberId, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclFindMyGroup returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::cloudAclDeleteGroup(void* ctx, const char *groupId,
        const char *groupMasterId, const OCDevAddr *endPoint, OCCloudResponseCB callback,
        OCStackResult expectedResult, bool checkCallback)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclDeleteGroup(ctx, groupId, groupMasterId, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclDeleteGroup returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::cloudAclJoinToInvitedGroup(void* ctx, const char *groupId,
        const OCDevAddr *endPoint, OCCloudResponseCB callback, OCStackResult expectedResult,
        bool checkCallback)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclJoinToInvitedGroup(ctx, groupId, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclDeleteGroup returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::cloudAclObserveGroup(void* ctx, const char *groupId,
        const OCDevAddr *endPoint, OCCloudResponseCB callback, OCStackResult expectedResult,
        bool checkCallback)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclObserveGroup(ctx, groupId, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclObserveGroup returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::cloudAclShareDeviceIntoGroup(void* ctx, const char *groupId,
        const stringArray_t *memberIds, const stringArray_t *deviceIds, const OCDevAddr *endPoint,
        OCCloudResponseCB callback, OCStackResult expectedResult, bool checkCallback)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclShareDeviceIntoGroup(ctx, groupId, memberIds, deviceIds,
            endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclShareDeviceIntoGroup returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::cloudAclDeleteDeviceFromGroup(void* ctx, const char *groupId,
        const stringArray_t *memberIds, const stringArray_t *deviceIds, const OCDevAddr *endPoint,
        OCCloudResponseCB callback, OCStackResult expectedResult, bool checkCallback)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclDeleteDeviceFromGroup(ctx, groupId, memberIds, deviceIds,
            endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclShareDeviceIntoGroup returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::cloudAclGroupGetInfo(void* ctx, const char *groupId, const char *memberId,
        const OCDevAddr *endPoint, OCCloudResponseCB callback, OCStackResult expectedResult,
        bool checkCallback)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclGroupGetInfo(ctx, groupId, memberId, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclGroupGetInfo returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::cloudAclIndividualUpdate(void* ctx, const char *aclId, const char *aceId,
        const cloudAce_t *aces, const OCDevAddr *endPoint, OCCloudResponseCB callback,
        OCStackResult expectedResult, bool checkCallback)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclIndividualUpdate(ctx, aclId, aceId, aces, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclIndividualUpdate returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::cloudAclIndividualDelete(void* ctx, const char *aclId,
        const OCDevAddr *endPoint, OCCloudResponseCB callback, OCStackResult expectedResult,
        bool checkCallback)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclIndividualDelete(ctx, aclId, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclIndividualDelete returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::cloudAclIndividualDeleteAce(void* ctx, const char *aclId, const char *aceId,
        const OCDevAddr *endPoint, OCCloudResponseCB callback, OCStackResult expectedResult,
        bool checkCallback)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclIndividualDeleteAce(ctx, aclId, aceId, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclIndividualDeleteAce returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::cloudAclInviteUser(void* ctx, const char *userId,
        const stringArray_t *groupIds, const stringArray_t *memberIds, const OCDevAddr *endPoint,
        OCCloudResponseCB callback, OCStackResult expectedResult, bool checkCallback)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclInviteUser(ctx, userId, groupIds, memberIds, endPoint,
            callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclInviteUser returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::cloudAclGetInvitation(void* ctx, const char *userId,
        const OCDevAddr *endPoint, OCCloudResponseCB callback, OCStackResult expectedResult,
        bool checkCallback)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclGetInvitation(ctx, userId, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclGetInvitation returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::cloudAclDeleteInvitation(void* ctx, const char *userId, const char *groupId,
        const OCDevAddr *endPoint, OCCloudResponseCB callback, OCStackResult expectedResult,
        bool checkCallback)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclDeleteInvitation(ctx, userId, groupId, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclDeleteInvitation returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::cloudAclCancelInvitation(void* ctx, const char *userId, const char *groupId,
        const char *memberId, const OCDevAddr *endPoint, OCCloudResponseCB callback,
        OCStackResult expectedResult, bool checkCallback)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclCancelInvitation(ctx, userId, groupId, memberId, endPoint,
            callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclCancelInvitation returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCsdkCloudHelper::cloudAclPolicyCheck(void* ctx, const char *subjectId, const char *deviceId,
        const char *method, const char *uri, const OCDevAddr *endPoint, OCCloudResponseCB callback,
        OCStackResult expectedResult, bool checkCallback)
{
    __FUNC_IN__
    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclPolicyCheck(ctx, subjectId, deviceId, method, uri, endPoint,
            callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclPolicyCheck returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

void CSCsdkCloudHelper::provisionResultCB(void* ctx, int nOfRes, OCProvisionResult_t* arr,
bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(INFO, "Provision SUCCEEDED - ctx: %s\n", (char* ) ctx);
    }
    else
    {
        IOTIVITYTEST_LOG(ERROR, "Provision Failed - ctx: %s", (char* ) ctx);
        CSCsdkUtilityHelper::printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }

    s_isCbInvoked = true;
}

void CSCsdkCloudHelper::cloudResponseCB(void* ctx, OCStackResult result, void* data)
{
    OC_UNUSED(ctx);
    OC_UNUSED(data);

    char* dataChar = (char*) data;

    IOTIVITYTEST_LOG(INFO, "%s: Received result = %d for ctx : %s", __func__, result, (char* )ctx);
    IOTIVITYTEST_LOG(INFO, "Received Data: %s", dataChar);

    if (result <= OC_STACK_RESOURCE_CHANGED)
    {
        s_isCbInvoked = true;
    }
}

void CSCsdkCloudHelper::aclResponseCB(void* ctx, OCStackResult result, void* data)
{
    __FUNC_IN__
    OC_UNUSED(ctx);
    OC_UNUSED(data);

    s_aclResponse = string((char*) data);

    IOTIVITYTEST_LOG(INFO, "%s: Received result = %d for ctx : %s", __func__, result, (char* )ctx);
    IOTIVITYTEST_LOG(INFO, "Received Data: %s", data);

    if (result <= OC_STACK_RESOURCE_CHANGED)
    {
        s_isCbInvoked = true;
    }

    __FUNC_OUT__
}

void CSCsdkCloudHelper::createGroupResponseCB(void* ctx, OCStackResult result, void* data)
{
    __FUNC_IN__
    OC_UNUSED(ctx);
    OC_UNUSED(data);

    s_groupId = string((char*) data);

    IOTIVITYTEST_LOG(INFO, "%s: Received result = %d for ctx : %s", __func__, result, (char* )ctx);
    IOTIVITYTEST_LOG(INFO, "Received Data: %s", data);

    if (result <= OC_STACK_RESOURCE_CHANGED)
    {
        s_isCbInvoked = true;
    }

    __FUNC_OUT__
}

std::string CSCsdkCloudHelper::getFailureMessage()
{
    return m_failureMessage;
}

