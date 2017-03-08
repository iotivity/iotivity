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
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionACL positively
 * @target          OCStackResult provisionACL(const OicSecAcl_t* acl, ResultCallBack resultCallback)
 * @test_data       Regular data for provisionACL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionACL
 *                  6. call deleteACLList for m_acl1
 * @post_condition  None
 * @expected        provisionACL will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppProvTest_btc, ProvisionAcl_RV_P)
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

    m_acl1 = (OicSecAcl_t *)OICCalloc(1,sizeof(OicSecAcl_t));
    PMCppHelper::createAcl(m_acl1, DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    if(!m_PMCppHelper.provisionACL(m_OwnedDevList, m_acl1, PMCppHelper::provisionCB, OC_STACK_OK))
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
 * @objective       test provisionACL negatively with acl as NULL
 * @target          OCStackResult provisionACL(const OicSecAcl_t* acl, ResultCallBack resultCallback)
 * @test_data       acl = NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionACL
 * @post_condition  None
 * @expected        provisionACL will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppProvTest_btc, ProvisionAcl_NV_N)
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

    if(!m_PMCppHelper.provisionACL(m_OwnedDevList, NULL, PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
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
 * @objective       test provisionACL negatively with resultCallback as NULL
 * @target          OCStackResult provisionACL(const OicSecAcl_t* acl, ResultCallBack resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionACL
 *                  6. call provisionACL
 *                  7. call deleteACLList for m_acl1
 * @post_condition  None
 * @expected        provisionACL will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppProvTest_btc, ProvisionAclCB_NV_N)
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

    if(!m_PMCppHelper.provisionACL(m_OwnedDevList, m_acl1, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }

    m_PMCppHelper.deleteACLList(m_acl1);
}
#endif

/**
 * @since           2017-01-31
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionACL(const OicSecAcl_t* acl, ResultCallBack resultCallback)
 * @objective       test saveACL positively
 * @target          static OCStackResult saveACL(const OicSecAcl_t* acl)
 * @test_data       Regular data for saveACL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionACL
 *                  6. call saveACL
 *                  7. call deleteACLList for m_acl1
 * @post_condition  None
 * @expected        saveACL will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppProvTest_btc, saveACL_SRC_P)
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

//    if(!m_PMCppHelper.provisionACL(m_OwnedDevList, m_acl1, PMCppHelper::provisionCB, OC_STACK_OK))
//    {
//        SET_FAILURE(m_PMCppHelper.getFailureMessage());
//    }

    if(!m_PMCppHelper.saveACL(m_acl1, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }

    m_PMCppHelper.deleteACLList(m_acl1);
}
#endif

/**
 * @since           2017-01-31
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionACL(const OicSecAcl_t* acl, ResultCallBack resultCallback)
 * @objective       test saveACL negatively with acl=NULL
 * @target          static OCStackResult saveACL(const OicSecAcl_t* acl)
 * @test_data       acl=NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionACL
 *                  6. call saveACL
 *                  7. call deleteACLList for m_acl1
 * @post_condition  None
 * @expected        saveACL will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppProvTest_btc, saveACL_NV_N)
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
    }

    if(!m_PMCppHelper.saveACL(NULL, OC_STACK_INVALID_PARAM))
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
 * @objective       test provisionPairwiseDevices positively
 * @target          OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @test_data       Regular data for provisionPairwiseDevices
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionPairwiseDevices
 *                  6. call deleteACLList for m_acl1
 *                  7. call deleteACLList for m_acl2
 * @post_condition  None
 * @expected        provisionPairwiseDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppProvTest_btc, ProvisionPairwiseDevicesKeySize_LBV_P)
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

    m_acl2 = (OicSecAcl_t *)OICCalloc(1,sizeof(OicSecAcl_t));
    PMCppHelper::createAcl(m_acl2, DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    Credential cred(SYMMETRIC_PAIR_WISE_KEY , OWNER_PSK_LENGTH_128);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, m_acl1, *m_OwnedDevList[1].get(), m_acl2, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }

    m_PMCppHelper.deleteACLList(m_acl1);
    m_PMCppHelper.deleteACLList(m_acl2);
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionPairwiseDevices positively with keySize as OWNER_PSK_LENGTH_256
 * @target          OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @test_data       keySize = OWNER_PSK_LENGTH_256
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionPairwiseDevices
 *                  6. call deleteACLList for m_acl1
 *                  7. call deleteACLList for m_acl2
 * @post_condition  None
 * @expected        provisionPairwiseDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppProvTest_btc, ProvisionPairwiseDevicesKeySize_FSV_UBV_P)
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

    m_acl2 = (OicSecAcl_t *)OICCalloc(1,sizeof(OicSecAcl_t));
    PMCppHelper::createAcl(m_acl2, DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    Credential cred(SYMMETRIC_PAIR_WISE_KEY, OWNER_PSK_LENGTH_256);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, m_acl1, *m_OwnedDevList[1].get(), m_acl2, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }

    m_PMCppHelper.deleteACLList(m_acl1);
    m_PMCppHelper.deleteACLList(m_acl2);
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionPairwiseDevices negatively with Outer Lower Boundary Value of keySize
 * @target          OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @test_data       Outer Lower Boundary Value of keySize
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionPairwiseDevices
 *                  6. call deleteACLList for m_acl1
 *                  7. call deleteACLList for m_acl2
 * @post_condition  None
 * @expected        provisionPairwiseDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppProvTest_btc, ProvisionPairwiseDevicesKeySize_LOBV_N)
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

    m_acl2 = (OicSecAcl_t *)OICCalloc(1,sizeof(OicSecAcl_t));
    PMCppHelper::createAcl(m_acl2, DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128-1;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, m_acl1, *m_OwnedDevList[1].get(), m_acl2, PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }

    m_PMCppHelper.deleteACLList(m_acl1);
    m_PMCppHelper.deleteACLList(m_acl2);
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionPairwiseDevices negatively with Outer Upper Boundary Value of keySize
 * @target          OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @test_data       Outer Upper Boundary Value of keySize
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionPairwiseDevices
 *                  6. call deleteACLList for m_acl1
 *                  7. call deleteACLList for m_acl2
 * @post_condition  None
 * @expected        provisionPairwiseDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppProvTest_btc, ProvisionPairwiseDevicesKeySize_UOBV_N)
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

    m_acl2 = (OicSecAcl_t *)OICCalloc(1,sizeof(OicSecAcl_t));
    PMCppHelper::createAcl(m_acl2, DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_256+1;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, m_acl1, *m_OwnedDevList[1].get(), m_acl2, PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }

    m_PMCppHelper.deleteACLList(m_acl1);
    m_PMCppHelper.deleteACLList(m_acl2);
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionPairwiseDevices negatively with resultCallback as null
 * @target          OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionPairwiseDevices
 *                  6. call deleteACLList for m_acl1
 *                  7. call deleteACLList for m_acl2
 * @post_condition  None
 * @expected        provisionPairwiseDevices will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppProvTest_btc, ProvisionPairwiseDevicesCB_NV_N)
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

    m_acl2 = (OicSecAcl_t *)OICCalloc(1,sizeof(OicSecAcl_t));
    PMCppHelper::createAcl(m_acl2, DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_256;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, m_acl1, *m_OwnedDevList[1].get(), m_acl2, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }

    m_PMCppHelper.deleteACLList(m_acl1);
    m_PMCppHelper.deleteACLList(m_acl2);
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionPairwiseDevices positively with m_acl1 = NULL
 * @target          OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @test_data       m_acl1 = NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionPairwiseDevices
 *                  6. call deleteACLList for m_acl2
 * @post_condition  None
 * @expected        provisionPairwiseDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppProvTest_btc, ProvisionPairwiseDevicesm_acl1_NV_P)
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

    m_acl2 = (OicSecAcl_t *)OICCalloc(1,sizeof(OicSecAcl_t));
    PMCppHelper::createAcl(m_acl2, DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_256;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, NULL, *m_OwnedDevList[1].get(), m_acl2, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }

    m_PMCppHelper.deleteACLList(m_acl2);
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionPairwiseDevices positively with m_acl2 = NULL
 * @target          OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @test_data       m_acl2 = NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionPairwiseDevices
 *                  6. call deleteACLList for m_acl1
 * @post_condition  None
 * @expected        provisionPairwiseDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppProvTest_btc, ProvisionPairwiseDevicesm_acl2_NV_P)
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

    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_256;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, m_acl1, *m_OwnedDevList[1].get(), NULL, PMCppHelper::provisionCB, OC_STACK_OK))
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
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @objective       test getLinkedDevices positively
 * @target          OCStackResult getLinkedDevices(UuidList_t &uuidList)
 * @test_data       Regular data for getLinkedDevices
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionPairwiseDevices
 *                  6. call getLinkedDevices
 *                  7. call deleteACLList for m_acl1
 *                  8. call deleteACLList for m_acl2
 * @post_condition  None
 * @expected        getLinkedDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppProvTest_btc, GetLinkedDevices_FSV_P)
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

    m_acl2 = (OicSecAcl_t *)OICCalloc(1,sizeof(OicSecAcl_t));
    PMCppHelper::createAcl(m_acl2, DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, m_acl1, *m_OwnedDevList[1].get(), m_acl2, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    UuidList_t uuidList1;

    if(!m_PMCppHelper.getLinkedDevices(m_OwnedDevList, uuidList1, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    m_PMCppHelper.deleteACLList(m_acl1);
    m_PMCppHelper.deleteACLList(m_acl2);
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @objective       test unlinkDevices positively
 * @target          OCStackResult unlinkDevices(const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       Regular data for unlinkDevices
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionPairwiseDevices
 *                  6. call unlinkDevices
 *                  7. call deleteACLList for m_acl1
 *                  8. call deleteACLList for m_acl2
 * @post_condition  None
 * @expected        unlinkDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppProvTest_btc, UnlinkDevices_RV_P)
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

    m_acl2 = (OicSecAcl_t *)OICCalloc(1,sizeof(OicSecAcl_t));
    PMCppHelper::createAcl(m_acl2, DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, m_acl1, *m_OwnedDevList[1].get(), m_acl2, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.unlinkDevices(m_OwnedDevList, *m_OwnedDevList[1].get(), PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }

    m_PMCppHelper.deleteACLList(m_acl1);
    m_PMCppHelper.deleteACLList(m_acl2);
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @objective       test unlinkDevices negatively with resultCallback as NULL
 * @target          OCStackResult unlinkDevices(const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionPairwiseDevices
 *                  6. call unlinkDevices
 *                  7. call deleteACLList for m_acl1
 *                  8. call deleteACLList for m_acl2
 * @post_condition  None
 * @expected        unlinkDevices will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppProvTest_btc, UnlinkDevicesCB_NV_N)
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

    m_acl2 = (OicSecAcl_t *)OICCalloc(1,sizeof(OicSecAcl_t));
    PMCppHelper::createAcl(m_acl2, DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, m_acl1, *m_OwnedDevList[1].get(), m_acl2, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.unlinkDevices(m_OwnedDevList, *m_OwnedDevList[1].get(), NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }

    m_PMCppHelper.deleteACLList(m_acl1);
    m_PMCppHelper.deleteACLList(m_acl2);
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @objective       test removeDevice positively
 * @target          OCStackResult removeDevice(unsigned short waitTimeForOwnedDeviceDiscovery, ResultCallBack resultCallback)
 * @test_data       regular data
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionPairwiseDevices
 *                  6. call removeDevice
 *                  7. call deleteACLList for m_acl1
 *                  8. call deleteACLList for m_acl2
 * @post_condition  None
 * @expected        removeDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppProvTest_btc, RemoveDeviceTime_RV_SRC_P)
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

    m_acl2 = (OicSecAcl_t *)OICCalloc(1,sizeof(OicSecAcl_t));
    PMCppHelper::createAcl(m_acl2, DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, m_acl1, *m_OwnedDevList[1].get(), m_acl2, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.removeDevice(m_OwnedDevList, DISCOVERY_TIMEOUT, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }

    m_PMCppHelper.deleteACLList(m_acl1);
    m_PMCppHelper.deleteACLList(m_acl2);
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @objective       test removeDevice positively with waitTimeForOwnedDeviceDiscovery as DISCOVERY_TIMEOUT_ONE
 * @target          OCStackResult removeDevice(unsigned short waitTimeForOwnedDeviceDiscovery, ResultCallBack resultCallback)
 * @test_data       waitTimeForOwnedDeviceDiscovery = DISCOVERY_TIMEOUT_ONE
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionPairwiseDevices
 *                  6. call removeDevice
 *                  7. call deleteACLList for m_acl1
 *                  8. call deleteACLList for m_acl2
 * @post_condition  None
 * @expected        removeDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppProvTest_btc, RemoveDeviceTime_LBV_P)
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

    m_acl2 = (OicSecAcl_t *)OICCalloc(1,sizeof(OicSecAcl_t));
    PMCppHelper::createAcl(m_acl2, DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, m_acl1, *m_OwnedDevList[1].get(), m_acl2, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.removeDevice(m_OwnedDevList, DISCOVERY_TIMEOUT_ONE, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }

    m_PMCppHelper.deleteACLList(m_acl1);
    m_PMCppHelper.deleteACLList(m_acl2);
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @objective       test removeDevice negatively with waitTimeForOwnedDeviceDiscovery as DISCOVERY_TIMEOUT_ZERO
 * @target          OCStackResult removeDevice(unsigned short waitTimeForOwnedDeviceDiscovery, ResultCallBack resultCallback)
 * @test_data       waitTimeForOwnedDeviceDiscovery = DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionPairwiseDevices
 *                  6. call removeDevice
 *                  7. call deleteACLList for m_acl1
 *                  8. call deleteACLList for m_acl2
 * @post_condition  None
 * @expected        removeDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppProvTest_btc, RemoveDeviceTime_LOBV_N)
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

    m_acl2 = (OicSecAcl_t *)OICCalloc(1,sizeof(OicSecAcl_t));
    PMCppHelper::createAcl(m_acl2, DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, m_acl1, *m_OwnedDevList[1].get(), m_acl2, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.removeDevice(m_OwnedDevList, DISCOVERY_TIMEOUT_ZERO, PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }

    m_PMCppHelper.deleteACLList(m_acl1);
    m_PMCppHelper.deleteACLList(m_acl2);
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @objective       test removeDevice negatively with resultCallback as NULL
 * @target          OCStackResult removeDevice(unsigned short waitTimeForOwnedDeviceDiscovery, ResultCallBack resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionPairwiseDevices
 *                  6. call removeDevice
 *                  7. call deleteACLList for m_acl1
 *                  8. call deleteACLList for m_acl2
 * @post_condition  None
 * @expected        removeDevice will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppProvTest_btc, RemoveDeviceCB_NV_N)
{
    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT,m_UnownedDevList, OC_STACK_OK))
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

    m_acl2 = (OicSecAcl_t *)OICCalloc(1,sizeof(OicSecAcl_t));
    PMCppHelper::createAcl(m_acl2, DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, m_acl1, *m_OwnedDevList[1].get(), m_acl2, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.removeDevice(m_OwnedDevList, DISCOVERY_TIMEOUT, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }

    m_PMCppHelper.deleteACLList(m_acl1);
    m_PMCppHelper.deleteACLList(m_acl2);
}
#endif

/**
 * @since           2017-03-06
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @objective       test removeDeviceWithUuid positively
 * @target          static OCStackResult removeDeviceWithUuid(unsigned short waitTimeForOwnedDeviceDiscovery,std::string uuid, ResultCallBack resultCallback)
 * @test_data       regular data
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionPairwiseDevices
 *                  6. call removeDeviceWithUuid
 *                  7. call deleteACLList for m_acl1
 *                  8. call deleteACLList for m_acl2
 * @post_condition  None
 * @expected        removeDeviceWithUuid will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppProvTest_btc, RemoveDeviceTimeWithUuid_RV_SRC_P)
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

    m_acl2 = (OicSecAcl_t *)OICCalloc(1,sizeof(OicSecAcl_t));
    PMCppHelper::createAcl(m_acl2, DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, m_acl1, *m_OwnedDevList[1].get(), m_acl2, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.removeDeviceWithUuid(DISCOVERY_TIMEOUT, m_OwnedDevList[0]->getDeviceID(),PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }

    m_PMCppHelper.deleteACLList(m_acl1);
    m_PMCppHelper.deleteACLList(m_acl2);
}
#endif

/**
 * @since           2017-03-06
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @objective       test removeDeviceWithUuid positively with waitTimeForOwnedDeviceDiscovery as DISCOVERY_TIMEOUT_ONE
 * @target          static OCStackResult removeDeviceWithUuid(unsigned short waitTimeForOwnedDeviceDiscovery,std::string uuid, ResultCallBack resultCallback)
 * @test_data       waitTimeForOwnedDeviceDiscovery = DISCOVERY_TIMEOUT_ONE
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionPairwiseDevices
 *                  6. call removeDeviceWithUuid
 *                  7. call deleteACLList for m_acl1
 *                  8. call deleteACLList for m_acl2
 * @post_condition  None
 * @expected        removeDeviceWithUuid will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppProvTest_btc, RemoveDeviceTimeWithUuid_LBV_P)
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

    m_acl2 = (OicSecAcl_t *)OICCalloc(1,sizeof(OicSecAcl_t));
    PMCppHelper::createAcl(m_acl2, DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, m_acl1, *m_OwnedDevList[1].get(), m_acl2, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.removeDeviceWithUuid(DISCOVERY_TIMEOUT_ONE, m_OwnedDevList[0]->getDeviceID(),PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }

    m_PMCppHelper.deleteACLList(m_acl1);
    m_PMCppHelper.deleteACLList(m_acl2);
}
#endif

/**
 * @since           2017-02-01
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @objective       test removeDeviceWithUuid negatively with waitTimeForOwnedDeviceDiscovery as DISCOVERY_TIMEOUT_ZERO
 * @target          static OCStackResult removeDeviceWithUuid(unsigned short waitTimeForOwnedDeviceDiscovery,std::string uuid, ResultCallBack resultCallback)
 * @test_data       waitTimeForOwnedDeviceDiscovery = DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionPairwiseDevices
 *                  6. call removeDeviceWithUuid
 *                  7. call deleteACLList for m_acl1
 *                  8. call deleteACLList for m_acl2
 * @post_condition  None
 * @expected        removeDeviceWithUuid will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppProvTest_btc, RemoveDeviceTimeWithUuid_LOBV_N)
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

    m_acl2 = (OicSecAcl_t *)OICCalloc(1,sizeof(OicSecAcl_t));
    PMCppHelper::createAcl(m_acl2, DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, m_acl1, *m_OwnedDevList[1].get(), m_acl2, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.removeDeviceWithUuid(DISCOVERY_TIMEOUT_ZERO, m_OwnedDevList[0]->getDeviceID(),PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }

    m_PMCppHelper.deleteACLList(m_acl1);
    m_PMCppHelper.deleteACLList(m_acl2);
}
#endif

/**
 * @since           2017-02-01
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @objective       test removeDeviceWithUuid negatively with uuid as NULL
 * @target          static OCStackResult removeDeviceWithUuid(unsigned short waitTimeForOwnedDeviceDiscovery,std::string uuid, ResultCallBack resultCallback)
 * @test_data       uuid = NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionPairwiseDevices
 *                  6. call removeDeviceWithUuid
 *                  7. call deleteACLList for m_acl1
 *                  8. call deleteACLList for m_acl2
 * @post_condition  None
 * @expected        removeDeviceWithUuid will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppProvTest_btc, RemoveDeviceTimeWithUuid_NV_N)
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

    m_acl2 = (OicSecAcl_t *)OICCalloc(1,sizeof(OicSecAcl_t));
    PMCppHelper::createAcl(m_acl2, DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, m_acl1, *m_OwnedDevList[1].get(), m_acl2, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.removeDeviceWithUuid(DISCOVERY_TIMEOUT,EMPTY_STRING,PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }

    m_PMCppHelper.deleteACLList(m_acl1);
    m_PMCppHelper.deleteACLList(m_acl2);
}
#endif

/**
 * @since           2017-02-01
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @objective       test removeDeviceWithUuid negatively with Callback as NULL
 * @target          static OCStackResult removeDeviceWithUuid(unsigned short waitTimeForOwnedDeviceDiscovery,std::string uuid, ResultCallBack resultCallback)
 * @test_data       Callback = NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call provisionPairwiseDevices
 *                  6. call removeDeviceWithUuid
 *                  7. call deleteACLList for m_acl1
 *                  8. call deleteACLList for m_acl2
 * @post_condition  None
 * @expected        removeDeviceWithUuid will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppProvTest_btc, RemoveDeviceTimeWithUuidCB_NV_N)
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

    m_acl2 = (OicSecAcl_t *)OICCalloc(1,sizeof(OicSecAcl_t));
    PMCppHelper::createAcl(m_acl2, DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, m_acl1, *m_OwnedDevList[1].get(), m_acl2, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.removeDeviceWithUuid(DISCOVERY_TIMEOUT, m_OwnedDevList[0]->getDeviceID(),NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }

    m_PMCppHelper.deleteACLList(m_acl1);
    m_PMCppHelper.deleteACLList(m_acl2);
}
#endif
