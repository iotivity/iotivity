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
 * @objective       test provisionACL positively with max range of Acl permission
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

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT, m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, MAX_PERMISSION_RANGE, m_OwnedDevList);

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
 * @objective       test provisionACL positively with minimum range of Acl permission
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

    OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, NO_PERMISSION, m_OwnedDevList);

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
TEST_F(PMCppTest_stc, ProvisionCredentialsCredType_REV_P)
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
 *                  7. call provisionACL
 * @post_condition  None
 * @expected        provisionACL will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_stc, ProvisionAclCBAcl_NV_N)
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

    OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    if(!m_PMCppHelper.provisionACL(m_OwnedDevList, acl1, PMCppHelper::provisionCB, OC_STACK_OK))
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

/**
 * @since           2016-07-26
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionACL positively calling API multiple times
 * @target          OCStackResult provisionACL(const OicSecAcl_t* acl, ResultCallBack resultCallback)
 * @test_data       Regular data for provisionACL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionACL
 *                  7. call provisionACL
 * @post_condition  None
 * @expected        provisionACL will return OC_STACK_OK while calling it multiple times.
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_stc, ProvisionAclMultipleTimes_SQV_P)
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

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT, m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    if(!m_PMCppHelper.provisionACL(m_OwnedDevList, acl1, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.provisionACL(m_OwnedDevList, acl1, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-07-26
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionCredentials negatively, calling provisionCredentials multiple times.
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       Regular data for provisionCredentials
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionCredentials
 *                  7. call provisionCredentials
 * @post_condition  None
 * @expected        provisionCredentials will return OC_STACK_INVALID_PARAM while calling for the second time.
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_stc, ProvisionCredentialsMultipleTimes_EG_N)
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

    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[1].get(), PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[1].get(), PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-07-26
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionPairwiseDevices negatively calling the API multiple times.
 * @target          OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* acl1, const OCSecureResource &device2, const OicSecAcl_t* acl2, ResultCallBack resultCallback)
 * @test_data       Regular data for provisionPairwiseDevices
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionPairwiseDevices
 *                  7. call provisionPairwiseDevices
 * @post_condition  None
 * @expected        provisionPairwiseDevices will return OC_STACK_INVALID_PARAM while calling for the second time.
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_stc, ProvisionPairwiseDevicesMultipleTimes_EG_N)
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

    OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);
    OicSecAcl_t *acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, m_OwnedDevList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type , keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, acl1, *m_OwnedDevList[1].get(), acl2, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, acl1, *m_OwnedDevList[1].get(), acl2, PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-07-26
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* acl1, const OCSecureResource &device2, const OicSecAcl_t* acl2, ResultCallBack resultCallback)
 * @objective       test unlinkDevices  negatively calling the API multiple times.
 * @target          OCStackResult unlinkDevices(const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       Regular data for removeDevice
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionPairwiseDevices
 *                  7. call unlinkDevices
 *                  8. call unlinkDevices
 * @post_condition  None
 * @expected        unlinkDevices will return OC_STACK_INVALID_PARAM while calling for the second time.
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_stc, UnlinkDeviceMultipleTimes_EG_N)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT,m_UnownedDevList, OC_STACK_OK))
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

    OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);
    OicSecAcl_t *acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, m_OwnedDevList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, acl1, *m_OwnedDevList[1].get(), acl2, PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.unlinkDevices(m_OwnedDevList, *m_OwnedDevList[1].get(), PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.unlinkDevices(m_OwnedDevList, *m_OwnedDevList[1].get(), PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-07-26
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* acl1, const OCSecureResource &device2, const OicSecAcl_t* acl2, ResultCallBack resultCallback)
 * @objective       test removeDevice negatively calling the API multiple times.
 * @target          OCStackResult removeDevice(unsigned short waitTimeForOwnedDeviceDiscovery, ResultCallBack resultCallback)
 * @test_data       Regular data for removeDevice
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionPairwiseDevices
 *                  7. call removeDevice
 *                  8. call removeDevice
 * @post_condition  None
 * @expected        removeDevice will return OC_STACK_INVALID_PARAM while calling for the second time.
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_stc, RemoveDeviceMultipleTimes_EG_N)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT,m_UnownedDevList, OC_STACK_INVALID_PARAM))
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

    OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);
    OicSecAcl_t *acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, m_OwnedDevList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, acl1, *m_OwnedDevList[1].get(), acl2, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.removeDevice(m_OwnedDevList, DISCOVERY_TIMEOUT, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.removeDevice(m_OwnedDevList, DISCOVERY_TIMEOUT, PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif
