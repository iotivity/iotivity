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
#include "PMCppUtilityHelper.h"
#include "CommonTestUtil.h"

class PMCppProvTest_btc: public ::testing::Test
{
protected:
    PMCppHelper m_PMCppHelper;
    DeviceList_t m_UnownedDevList, m_OwnedDevList;
    OicSecAcl_t *m_acl1, *m_acl2;

    virtual void SetUp()
    {
        CommonTestUtil::runCommonTCSetUpPart();
        CommonUtil::killApp(KILL_SERVERS);
        PMCppUtilityHelper::removeAllResFile();
        CommonUtil::copyFile(JUSTWORKS_SERVER_CBOR_O1_UNOWNED_BACKUP, JUSTWORKS_SERVER_CBOR_O1);
        CommonUtil::copyFile(JUSTWORKS_SERVER_CBOR_O2_UNOWNED_BACKUP, JUSTWORKS_SERVER_CBOR_O2);
        CommonUtil::copyFile(CLIENT_CBOR_01_UNOWNED_BACKUP, CLIENT_CBOR_01);
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        CommonUtil::launchApp(JUSTWORKS_SERVER1);
        CommonUtil::launchApp(JUSTWORKS_SERVER2);
        CommonUtil::waitInSecond(DELAY_MEDIUM);

        m_UnownedDevList.clear();
        m_OwnedDevList.clear();
        m_acl1 = NULL;
        m_acl2 = NULL;

        if(!m_PMCppHelper.provisionInit())
        {
            SET_FAILURE(m_PMCppHelper.getFailureMessage());
            return;
        }

        if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
        {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
        }

        if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppCallbackHelper::provisionPostCB, OC_STACK_OK))
        {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        }

        if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
        {
            SET_FAILURE(m_PMCppHelper.getFailureMessage());
            return;
        }

        m_acl1 = (OicSecAcl_t *)OICCalloc(1,sizeof(OicSecAcl_t));
        PMCppHelper::createAcl(m_acl1, DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

        m_acl2 = (OicSecAcl_t *)OICCalloc(1,sizeof(OicSecAcl_t));
        PMCppHelper::createAcl(m_acl2, DEVICE_INDEX_TWO, FULL_PERMISSION, m_OwnedDevList);
    }

    virtual void TearDown()
    {
        CommonTestUtil::runCommonTCTearDownPart();
        m_PMCppHelper.deleteACLList(m_acl1);
        m_PMCppHelper.deleteACLList(m_acl2);
        CommonUtil::killApp(KILL_SERVERS);
    }
};

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionACL positively
 * @target          OCStackResult provisionACL(const OicSecAcl_t* acl, ResultCallBack resultCallback)
 * @test_data       Regular data for provisionACL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor file from 'pairwise' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call provisionACL
 *                  4. call deleteACLList for m_acl1 and m_acl2
 * @post_condition  None
 * @expected        provisionACL will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppProvTest_btc, ProvisionAcl_RV_P)
{
    if(!m_PMCppHelper.provisionACL(m_OwnedDevList, m_acl1, PMCppCallbackHelper::provisionPostCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionACL negatively with acl as NULL
 * @target          OCStackResult provisionACL(const OicSecAcl_t* acl, ResultCallBack resultCallback)
 * @test_data       acl = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor file from 'pairwise' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call provisionACL
 *                  4. call deleteACLList for m_acl1 and m_acl2
 * @post_condition  None
 * @expected        provisionACL will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppProvTest_btc, ProvisionAcl_NV_N)
{
    if(!m_PMCppHelper.provisionACL(m_OwnedDevList, NULL, PMCppCallbackHelper::provisionPostCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionACL negatively with resultCallback as NULL
 * @target          OCStackResult provisionACL(const OicSecAcl_t* acl, ResultCallBack resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor file from 'pairwise' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call provisionACL
 *                  4. call deleteACLList for m_acl1 and m_acl2
 * @post_condition  None
 * @expected        provisionACL will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppProvTest_btc, ProvisionAclCB_NV_N)
{
    if(!m_PMCppHelper.provisionACL(m_OwnedDevList, m_acl1, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-31
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionACL(const OicSecAcl_t* acl, ResultCallBack resultCallback)
 * @objective       test saveACL positively
 * @target          static OCStackResult saveACL(const OicSecAcl_t* acl)
 * @test_data       Regular data for saveACL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor file from 'pairwise' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call saveACL
 *                  4. call deleteACLList for m_acl1 and m_acl2
 * @post_condition  None
 * @expected        saveACL will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppProvTest_btc, saveACL_SRC_P)
{
    if(!m_PMCppHelper.saveACL(m_acl1, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-31
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionACL(const OicSecAcl_t* acl, ResultCallBack resultCallback)
 * @objective       test saveACL negatively with acl=NULL
 * @target          static OCStackResult saveACL(const OicSecAcl_t* acl)
 * @test_data       acl=NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor file from 'pairwise' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call provisionACL
 *                  4. call saveACL
 *                  5. call deleteACLList for m_acl1 and m_acl2
 * @post_condition  None
 * @expected        saveACL will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppProvTest_btc, saveACL_NV_N)
{
    if(!m_PMCppHelper.provisionACL(m_OwnedDevList, m_acl1, PMCppCallbackHelper::provisionPostCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }

    if(!m_PMCppHelper.saveACL(NULL, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionPairwiseDevices positively
 * @target          OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @test_data       Regular data for provisionPairwiseDevices
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor file from 'pairwise' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call provisionPairwiseDevices
 *                  4. call deleteACLList for m_acl1 and m_acl2
 * @post_condition  None
 * @expected        provisionPairwiseDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppProvTest_btc, ProvisionPairwiseDevicesKeySize_LBV_P)
{
    Credential cred(SYMMETRIC_PAIR_WISE_KEY , OWNER_PSK_LENGTH_128);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, m_acl1, *m_OwnedDevList[1].get(), m_acl2, PMCppCallbackHelper::provisionPostCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionPairwiseDevices positively with keySize as OWNER_PSK_LENGTH_256
 * @target          OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @test_data       keySize = OWNER_PSK_LENGTH_256
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor file from 'pairwise' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call provisionPairwiseDevices
 *                  4. call deleteACLList for m_acl1 and m_acl2
 * @post_condition  None
 * @expected        provisionPairwiseDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppProvTest_btc, ProvisionPairwiseDevicesKeySize_FSV_UBV_P)
{
    Credential cred(SYMMETRIC_PAIR_WISE_KEY, OWNER_PSK_LENGTH_256);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, m_acl1, *m_OwnedDevList[1].get(), m_acl2, PMCppCallbackHelper::provisionPostCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionPairwiseDevices negatively with Outer Lower Boundary Value of keySize
 * @target          OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @test_data       Outer Lower Boundary Value of keySize
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor file from 'pairwise' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call provisionPairwiseDevices
 *                  4. call deleteACLList for m_acl1 and m_acl2
 * @post_condition  None
 * @expected        provisionPairwiseDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppProvTest_btc, ProvisionPairwiseDevicesKeySize_LOBV_N)
{
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128-1;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, m_acl1, *m_OwnedDevList[1].get(), m_acl2, PMCppCallbackHelper::provisionPostCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionPairwiseDevices negatively with Outer Upper Boundary Value of keySize
 * @target          OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @test_data       Outer Upper Boundary Value of keySize
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor file from 'pairwise' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call provisionPairwiseDevices
 *                  4. call deleteACLList for m_acl1 and m_acl2
 * @post_condition  None
 * @expected        provisionPairwiseDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppProvTest_btc, ProvisionPairwiseDevicesKeySize_UOBV_N)
{
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_256+1;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, m_acl1, *m_OwnedDevList[1].get(), m_acl2, PMCppCallbackHelper::provisionPostCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionPairwiseDevices negatively with resultCallback as null
 * @target          OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor file from 'pairwise' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call provisionPairwiseDevices
 *                  4. call deleteACLList for m_acl1 & m_acl2
 * @post_condition  None
 * @expected        provisionPairwiseDevices will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppProvTest_btc, ProvisionPairwiseDevicesCB_NV_N)
{
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_256;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, m_acl1, *m_OwnedDevList[1].get(), m_acl2, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionPairwiseDevices positively with m_acl1 = NULL
 * @target          OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @test_data       m_acl1 = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor file from 'pairwise' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call provisionPairwiseDevices
 *                  4. call deleteACLList for m_acl1 & m_acl2
 * @post_condition  None
 * @expected        provisionPairwiseDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppProvTest_btc, ProvisionPairwiseDevicesm_acl1_NV_P)
{
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_256;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, NULL, *m_OwnedDevList[1].get(), m_acl2, PMCppCallbackHelper::provisionPostCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionPairwiseDevices positively with m_acl2 = NULL
 * @target          OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @test_data       m_acl2 = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor file from 'pairwise' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call provisionPairwiseDevices
 *                  4. call deleteACLList for m_acl1 & m_acl2
 * @post_condition  None
 * @expected        provisionPairwiseDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppProvTest_btc, ProvisionPairwiseDevicesm_acl2_NV_P)
{
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_256;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, m_acl1, *m_OwnedDevList[1].get(), NULL, PMCppCallbackHelper::provisionPostCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif


/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionCredentials positively
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       Regular data for provisionCredentials
 * @pre_condition   start two justworks simulators using cbor file from 'owned' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call provisionCredentials
 * @post_condition  None
 * @expected        provisionCredentials will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppProvTest_btc, ProvisionCredentialsKeysize_FSV_LBV_P)
{
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[1].get(), PMCppCallbackHelper::provisionPostCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionCredentials positively with Upper Boundary Value of keysize for Credential
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       Credentail with keySize = OWNER_PSK_LENGTH_256
 * @pre_condition   start two justworks simulators using cbor file from 'owned' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call provisionCredentials
 * @post_condition  None
 * @expected        provisionCredentials will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppProvTest_btc, ProvisionCredentialsKeysize_UBV_P)
{
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_256;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[1].get(), PMCppCallbackHelper::provisionPostCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionACL(const OicSecAcl_t* acl, ResultCallBack resultCallback)
 * @objective       test provisionCredentials positively with resultCallback as NULL
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   start two justworks simulators using cbor file from 'owned' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionACL
 *                  6. call provisionCredentials
 *                  7. call deleteACLList for m_acl1
 * @post_condition  None
 * @expected        provisionCredentials will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppProvTest_btc, ProvisionCredentialsCB_NV_N)
{
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[1].get(), NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionCredentials negatively with Outer Lower Boundary Value of keySize
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       Outer Lower Boundary Value of keySize
 * @pre_condition   start two justworks simulators using cbor file from 'owned' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call provisionCredentials
 * @post_condition  None
 * @expected        provisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppProvTest_btc, ProvisionCredentialsKeysize_LOBV_N)
{
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128-1;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[1].get(), PMCppCallbackHelper::provisionPostCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionCredentials negatively with Outer Upper Boundary Value of keySize
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       Outer Upper Boundary Value of keySize
 * @pre_condition   start two justworks simulators using cbor file from 'owned' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call provisionCredentials
 * @post_condition  None
 * @expected        provisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppProvTest_btc, ProvisionCredentialsKeysize_UOBV_N)
{
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_256 + 1;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[1].get(), PMCppCallbackHelper::provisionPostCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionCredentials negatively with Outer Lower Boundary Value of credType
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       Outer Lower Boundary Value of credType
 * @pre_condition   start two justworks simulators using cbor file from 'owned' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call provisionCredentials
 * @post_condition  None
 * @expected        provisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppProvTest_btc, ProvisionCredentialsCredType_LOBV_N)
{
    OicSecCredType_t type = (OicSecCredType_t) CRED_TYPE_LOBV;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[1].get(), PMCppCallbackHelper::provisionPostCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionCredentials negatively with Outer Upper Boundary Value of credType
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       Outer Upper Boundary Value of credType
 * @pre_condition   start two justworks simulators using cbor file from 'owned' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call provisionCredentials
 * @post_condition  None
 * @expected        provisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppProvTest_btc, ProvisionCredentialsCredType_UOBV_N)
{
    OicSecCredType_t type = (OicSecCredType_t) CRED_TYPE_UOBV;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[1].get(), PMCppCallbackHelper::provisionPostCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionCredentials negatively for same device
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       credential between same devices
 * @pre_condition   start two justworks simulators using cbor file from 'owned' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call provisionCredentials
 * @post_condition  None
 * @expected        provisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppProvTest_btc, ProvisionCredentialsSameDev_EG_N)
{
    OicSecCredType_t type = (OicSecCredType_t) 1;
    size_t keySize = OWNER_PSK_LENGTH_256;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[0].get(), PMCppCallbackHelper::provisionPostCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif
