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

#include "PMCppHelper.h"

class PMCppTest_stc: public ::testing::Test
{
protected:
    PMCppHelper m_PMCppHelper;
    DeviceList_t m_UnownedDevList, m_OwnedDevList;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();
        CommonUtil::killApp(JUSTWORKS_SERVER);
        CommonUtil::killApp(RANDOMPIN_SERVER);
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        CommonUtil::rmFile(DATABASE_PDM);
        CommonUtil::rmFile(JUSTWORKS_SERVER1_CBOR);
        CommonUtil::rmFile(JUSTWORKS_SERVER2_CBOR);
        CommonUtil::rmFile(CLIENT_CBOR);
        CommonUtil::waitInSecond(DELAY_LONG);
        CommonUtil::copyFile(JUSTWORKS_SERVER1_CBOR_BACKUP, JUSTWORKS_SERVER1_CBOR);
        CommonUtil::copyFile(JUSTWORKS_SERVER2_CBOR_BACKUP, JUSTWORKS_SERVER2_CBOR);
        CommonUtil::copyFile(CLIENT_CBOR_BACKUP, CLIENT_CBOR);
        CommonUtil::launchApp(JUSTWORKS_SERVER1);
        CommonUtil::launchApp(JUSTWORKS_SERVER2);
        CommonUtil::waitInSecond(DELAY_LONG);
        m_UnownedDevList.clear();
        m_OwnedDevList.clear();

        m_PMCppHelper.provisionInit();
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
        CommonUtil::killApp(JUSTWORKS_SERVER);
        CommonUtil::killApp(RANDOMPIN_SERVER);
    }
};

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionACL negatively with max range of Acl permission
 * @target          OCStackResult provisionACL(const OicSecAcl_t* acl, ResultCallBack resultCallback)
 * @test_data       Acl permission with max Range
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionACL
 * @post_condition  None
 * @expected        provisionACL will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_stc, ProvisionAclPermisison_UBV_P)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecAcl_t *acl1 = (OicSecAcl_t *) OICCalloc(1, sizeof(OicSecAcl_t));
    char* owner = (char*) ACL_ROWNER_UUID_02;

    // Subject
    strcpy((char *) acl1->subject.id, (const char*) ACL_SUBJECT_UUID_02);

    // resource
    acl1->resourcesLen = ACL_RESOURCE_LEN;
    acl1->resources = (char **) OICCalloc(acl1->resourcesLen, sizeof(char *));
    acl1->resources[0] = (char*) LIGHT_RESOURCE_URI_01;
    acl1->resources[1] = (char*) LIGHT_RESOURCE_URI_02;

    // Permission
    acl1->permission = MAX_PERMISSION_RANGE;

    // Owner
    memcpy(&acl1->rownerID, &owner, sizeof(OicUuid_t));

    if(!m_PMCppHelper.provisionACL(m_OwnedDevList, acl1, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionACL negatively with minimum range of Acl permission
 * @target          OCStackResult provisionACL(const OicSecAcl_t* acl, ResultCallBack resultCallback)
 * @test_data       Acl permission with minimum Range
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionACL
 * @post_condition  None
 * @expected        provisionACL will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_stc, ProvisionAclPermisison_LBV_P)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecAcl_t *acl1 = (OicSecAcl_t *) OICCalloc(1, sizeof(OicSecAcl_t));
    char* owner = (char*) ACL_ROWNER_UUID_02;

    // Subject
    strcpy((char *) acl1->subject.id, (const char*) ACL_SUBJECT_UUID_02);

    // resource
    acl1->resourcesLen = ACL_RESOURCE_LEN;
    acl1->resources = (char **) OICCalloc(acl1->resourcesLen, sizeof(char *));
    acl1->resources[0] = (char*) LIGHT_RESOURCE_URI_01;
    acl1->resources[1] = (char*) LIGHT_RESOURCE_URI_02;

    // Permission
    acl1->permission = NO_PERMISSION;

    // Owner
    memcpy(&acl1->rownerID, &owner, sizeof(OicUuid_t));

    if(!m_PMCppHelper.provisionACL(m_OwnedDevList, acl1, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionCredentials positively with representative value of SYMMETRIC_PAIR_WISE_KEY
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       representative value of SYMMETRIC_PAIR_WISE_KEY as CredType
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionCredentials
 * @post_condition  None
 * @expected        provisionCredentials will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_stc, ProvisionCredentialsKeysize_REV_P)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecCredType_t type = 1;
    size_t keySize = OWNER_PSK_LENGTH_256;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[1].get(), PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif


/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionACL negatively with resultCallback and acl as NULL
 * @target          OCStackResult provisionACL(const OicSecAcl_t* acl, ResultCallBack resultCallback)
 * @test_data       resultCallback = NULL & acl = NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionACL
 * @post_condition  None
 * @expected        provisionACL will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_stc, ProvisionAclTestNullCallbackNUllAcl_N)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.provisionACL(m_OwnedDevList, NULL, NULL, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif
