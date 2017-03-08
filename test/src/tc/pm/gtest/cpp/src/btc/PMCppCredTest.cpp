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

class PMCppCredTest_btc: public ::testing::Test
{
protected:
    PMCppHelper m_PMCppHelper;
    DeviceList_t m_UnownedDevList, m_OwnedDevList;
    OicSecAcl_t *m_acl1, *m_acl2;

    virtual void SetUp()
    {
        CommonTestUtil::runCommonTCSetUpPart();
        CommonUtil::killApp(KILL_SERVERS);
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        PMCppUtilityHelper::removeAllResFile();
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        CommonUtil::rmFile(DATABASE_PDM);
        CommonUtil::rmFile(JUSTWORKS_SERVER1_CBOR);
        CommonUtil::rmFile(JUSTWORKS_SERVER2_CBOR);
        CommonUtil::rmFile(CLIENT_CBOR);
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        CommonUtil::copyFile(JUSTWORKS_SERVER1_CBOR_BACKUP, JUSTWORKS_SERVER1_CBOR);
        CommonUtil::copyFile(JUSTWORKS_SERVER2_CBOR_BACKUP, JUSTWORKS_SERVER2_CBOR);
        CommonUtil::copyFile(CLIENT_CBOR_BACKUP, CLIENT_CBOR);
        CommonUtil::launchApp(JUSTWORKS_SERVER1);
        CommonUtil::launchApp(JUSTWORKS_SERVER2);
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        m_UnownedDevList.clear();
        m_OwnedDevList.clear();
        m_acl1 = NULL;
        m_acl2 = NULL;
    }

    virtual void TearDown()
    {
        CommonTestUtil::runCommonTCTearDownPart();
        CommonUtil::killApp(KILL_SERVERS);
    }
};

/**
 * @since 2016-02-29
 * @objective Test getCredentialType
 * @target OicSecCredType_t getCredentialType() const
 * @test_data getCredentialType
 * @pre_condition None
 * @procedure call getCredentialType
 * @post_condition None
 * @expected getCredentialType will return SYMMETRIC_PAIR_WISE_KEY
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppCredTest_btc, GetCredentialType_SRC_P)
{
    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if (SYMMETRIC_PAIR_WISE_KEY != cred.getCredentialType())
    {
        SET_FAILURE("getCredentialType Failed");
    }
}
#endif

/**
 * @since 2016-02-29
 * @see 1. getCredentialKeySize()
 * @objective Test getCredentialKeySize
 * @target size_t getCredentialKeySize() const
 * @test_data getCredentialKeySize
 * @pre_condition None
 * @procedure call getCredentialKeySize
 * @post_condition None
 * @expected getCredentialKeySize will return OWNER_PSK_LENGTH_128
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppCredTest_btc, GetCredentialKeySize_SRC_P)
{
    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    Credential cred(SYMMETRIC_PAIR_WISE_KEY, OWNER_PSK_LENGTH_128);

    if (OWNER_PSK_LENGTH_128 != cred.getCredentialKeySize())
    {
        SET_FAILURE("getCredentialKeySize Failed");
    }
}
#endif

/**
 * @since 2016-02-29
 * @see 1. setCredentialType()
 * @objective Test setCredentialType
 * @target void setCredentialType(OicSecCredType_t type)
 * @test_data setCredentialType
 * @pre_condition None
 * @procedure call setCredentialType
 * @post_condition None
 * @expected setCredentialType will set SYMMETRIC_GROUP_KEY
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppCredTest_btc, SetCredentialType_SRC_P)
{
    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    Credential cred(SYMMETRIC_PAIR_WISE_KEY, OWNER_PSK_LENGTH_128);
    cred.setCredentialType(SYMMETRIC_GROUP_KEY);

    if (SYMMETRIC_GROUP_KEY != cred.getCredentialType())
    {
        SET_FAILURE("getCredentialType Failed");
    }

}
#endif

/**
 * @since 2016-02-29
 * @see 1. setCredentialKeySize()
 * @objective Test setCredentialKeySize
 * @target void setCredentialKeySize(size_t keySize)
 * @test_data setCredentialKeySize
 * @pre_condition None
 * @procedure call setCredentialKeySize
 * @post_condition None
 * @expected setCredentialKeySize will set OWNER_PSK_LENGTH_256
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppCredTest_btc, SetCredentialKeySize_SRC_P)
{
    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    Credential cred(SYMMETRIC_PAIR_WISE_KEY, OWNER_PSK_LENGTH_128);
    cred.setCredentialKeySize(OWNER_PSK_LENGTH_256);

    if (OWNER_PSK_LENGTH_256 != cred.getCredentialKeySize())
    {
        SET_FAILURE("getCredentialKeySize Failed");
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionCredentials positively
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       Regular data for provisionCredentials
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionCredentials
 * @post_condition  None
 * @expected        provisionCredentials will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppCredTest_btc, ProvisionCredentialsKeysize_FSV_LBV_P)
{
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
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionCredentials positively with Upper Boundary Value of keysize for Credential
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       Credentail with keySize = OWNER_PSK_LENGTH_256
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionCredentials
 * @post_condition  None
 * @expected        provisionCredentials will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppCredTest_btc, ProvisionCredentialsKeysize_UBV_P)
{
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
    size_t keySize = OWNER_PSK_LENGTH_256;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[1].get(), PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionACL(const OicSecAcl_t* acl, ResultCallBack resultCallback)
 * @objective       test provisionCredentials positively with resultCallback as NULL
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   start two justworks simulators
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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppCredTest_btc, ProvisionCredentialsCB_NV_N)
{
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

    m_acl1 = (OicSecAcl_t *)OICCalloc(1,sizeof(OicSecAcl_t));
    PMCppHelper::createAcl(m_acl1, DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    if(!m_PMCppHelper.provisionACL(m_OwnedDevList, m_acl1, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[1].get(), NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }

    m_PMCppHelper.deleteACLList(m_acl1);
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionCredentials negatively with Outer Lower Boundary Value of keySize
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       Outer Lower Boundary Value of keySize
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionCredentials
 * @post_condition  None
 * @expected        provisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppCredTest_btc, ProvisionCredentialsKeysize_LOBV_N)
{
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
    size_t keySize = OWNER_PSK_LENGTH_128-1;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[1].get(), PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionCredentials negatively with Outer Upper Boundary Value of keySize
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       Outer Upper Boundary Value of keySize
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionCredentials
 * @post_condition  None
 * @expected        provisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppCredTest_btc, ProvisionCredentialsKeysize_UOBV_N)
{
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
    size_t keySize = OWNER_PSK_LENGTH_256 + 1;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[1].get(), PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionCredentials negatively with Outer Lower Boundary Value of credType
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       Outer Lower Boundary Value of credType
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionCredentials
 * @post_condition  None
 * @expected        provisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppCredTest_btc, ProvisionCredentialsCredType_LOBV_N)
{
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

    OicSecCredType_t type = (OicSecCredType_t) CRED_TYPE_LOBV;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[1].get(), PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionCredentials negatively with Outer Upper Boundary Value of credType
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       Outer Upper Boundary Value of credType
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionCredentials
 * @post_condition  None
 * @expected        provisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppCredTest_btc, ProvisionCredentialsCredType_UOBV_N)
{
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

    OicSecCredType_t type = (OicSecCredType_t) CRED_TYPE_UOBV;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[1].get(), PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionCredentials negatively for same device
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       credential between same devices
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionCredentials
 * @post_condition  None
 * @expected        provisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppCredTest_btc, ProvisionCredentialsSameDev_EG_N)
{
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

    OicSecCredType_t type = (OicSecCredType_t) 1;
    size_t keySize = OWNER_PSK_LENGTH_256;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[0].get(), PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif
