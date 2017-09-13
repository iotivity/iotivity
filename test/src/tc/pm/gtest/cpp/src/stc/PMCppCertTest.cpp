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

#include "PMCppCertHelper.h"
#include "CommonTestUtil.h"

class PMCppCertTest_stc: public ::testing::Test
{
protected:
    PMCppHelper m_PMCppHelper;
    PMCppCertHelper m_PMCppCertHelper;
    DeviceList_t m_OwnedDevList;

    virtual void SetUp()
    {
        CommonTestUtil::runCommonTCSetUpPart();
        CommonUtil::killApp(KILL_SERVERS);
        PMCppUtilityHelper::removeAllResFile();
        CommonUtil::copyFile(JUSTWORKS_SERVER_CBOR_O1_OWNED_BACKUP, JUSTWORKS_SERVER_CBOR_O1);
        CommonUtil::copyFile(JUSTWORKS_SERVER_CBOR_O2_OWNED_BACKUP, JUSTWORKS_SERVER_CBOR_O2);
        CommonUtil::copyFile(CLIENT_CBOR_01_OWNED_BACKUP, CLIENT_CBOR_01);
        CommonUtil::copyFile(DEVICE_PROP_CBOR_01_OWNED_BACKUP, DEVICE_PROP_CBOR_01);
        CommonUtil::copyFile(CLIENT_DB_01_OWNED_BACKUP, CLIENT_DB_01);
        CommonUtil::copyFile(ROOT_CERT_UNOWNED_BACKUP, ROOT_CERT);
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        CommonUtil::launchApp(JUSTWORKS_SERVER1);
        CommonUtil::launchApp(JUSTWORKS_SERVER2);
        CommonUtil::waitInSecond(DELAY_MEDIUM);

        m_OwnedDevList.clear();

        if(!m_PMCppHelper.provisionInit())
        {
            SET_FAILURE(m_PMCppHelper.getFailureMessage());
            return;
        }

        if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
        {
            SET_FAILURE(m_PMCppHelper.getFailureMessage());
            return;
        }
    }

    virtual void TearDown()
    {
        CommonTestUtil::runCommonTCTearDownPart();
        CommonUtil::killApp(KILL_SERVERS);
    }
};

/**
 * @since           2016-10-24
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             static OCStackResult registerTrustCertChangeNotifier(CertChainCallBack)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       test provisionTrustCertChain positively
 * @target          OCStackResult provisionTrustCertChain(OicSecCredType_t type, uint16_t credId, ResultCallBack resultCallback)
 * @test_data       Regular data for provisionTrustCertChain
 * @pre_condition   start two justworks simulators using cbor files from 'unowned' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call registerTrustCertChangeNotifier
 *                  4. call saveTrustCertChain
 *                  5. call provisionTrustCertChain
 * @post_condition  None
 * @expected        provisionTrustCertChain will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppCertTest_stc, ProvisionTrustCertChain_RV_SRC_P)
{
    if (!m_PMCppCertHelper.registerTrustCertChangeNotifier(PMCppCertHelper::certChainCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }

    PMCppUtilityHelper::readFile(ROOT_CERT_FILE, (OCByteString *) &PMCppCertHelper::s_trustCertChainArray);

    if (!m_PMCppCertHelper.saveTrustCertChain(PMCppCertHelper::s_trustCertChainArray.data, PMCppCertHelper::s_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &PMCppCertHelper::s_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }

    if(!m_PMCppCertHelper.provisionTrustCertChain(m_OwnedDevList, SIGNED_ASYMMETRIC_KEY, PMCppCertHelper::s_credId, PMCppCallbackHelper::provisionPostCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-10-24
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list) * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             static OCStackResult registerTrustCertChangeNotifier(CertChainCallBack)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       test provisionTrustCertChain negatively with OicSecCredType_t as SYMMETRIC_PAIR_WISE_KEY
 * @target          OCStackResult provisionTrustCertChain(OicSecCredType_t type, uint16_t credId, ResultCallBack resultCallback)
 * @test_data       OicSecCredType_t as SYMMETRIC_PAIR_WISE_KEY
 * @pre_condition   start two justworks simulators using cbor files from 'unowned' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call registerTrustCertChangeNotifier
 *                  4. call saveTrustCertChain
 *                  5. call provisionTrustCertChain
 * @post_condition  None
 * @expected        provisionTrustCertChain will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppCertTest_stc, ProvisionTrustCertChainCredType_EG_N)
{
    if (!m_PMCppCertHelper.registerTrustCertChangeNotifier(PMCppCertHelper::certChainCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }

    PMCppUtilityHelper::readFile(ROOT_CERT_FILE, (OCByteString *) &PMCppCertHelper::s_trustCertChainArray);

    if (!m_PMCppCertHelper.saveTrustCertChain(PMCppCertHelper::s_trustCertChainArray.data, PMCppCertHelper::s_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &PMCppCertHelper::s_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }

    if(!m_PMCppCertHelper.provisionTrustCertChain(m_OwnedDevList, SYMMETRIC_PAIR_WISE_KEY, PMCppCertHelper::s_credId, PMCppCallbackHelper::provisionPostCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-10-24
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list) * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             static OCStackResult registerTrustCertChangeNotifier(CertChainCallBack)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       test provisionTrustCertChain negatively with OicSecCredType_t as SYMMETRIC_PAIR_WISE_KEY
 * @target          OCStackResult provisionTrustCertChain(OicSecCredType_t type, uint16_t credId, ResultCallBack resultCallback)
 * @test_data       resultCallback as NULL
 * @pre_condition   start two justworks simulators using cbor files from 'unowned' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call registerTrustCertChangeNotifier
 *                  4. call saveTrustCertChain
 *                  5. call provisionTrustCertChain
 * @post_condition  None
 * @expected        provisionTrustCertChain will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppCertTest_stc, ProvisionTrustCertChainCb_NV_N)
{
    if (!m_PMCppCertHelper.registerTrustCertChangeNotifier(PMCppCertHelper::certChainCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }

    PMCppUtilityHelper::readFile(ROOT_CERT, (OCByteString *) &PMCppCertHelper::s_trustCertChainArray);

    if (!m_PMCppCertHelper.saveTrustCertChain(PMCppCertHelper::s_trustCertChainArray.data, PMCppCertHelper::s_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &PMCppCertHelper::s_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }

    if(!m_PMCppCertHelper.provisionTrustCertChain(m_OwnedDevList, SIGNED_ASYMMETRIC_KEY, PMCppCertHelper::s_credId, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-28
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list) * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             static OCStackResult registerTrustCertChangeNotifier(CertChainCallBack)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       test provisionTrustCertChain negatovely wiht credId as -1
 * @target          OCStackResult provisionTrustCertChain(OicSecCredType_t type, uint16_t credId, ResultCallBack resultCallback)
 * @test_data       credId as -1
 * @pre_condition   start two justworks simulators using cbor files from 'unowned' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call registerTrustCertChangeNotifier
 *                  4. call saveTrustCertChain
 *                  5. call provisionTrustCertChain
 * @post_condition  None
 * @expected        provisionTrustCertChain will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppCertTest_stc, ProvisionTrustCertChainCredId_LOBV_N)
{
    if (!m_PMCppCertHelper.registerTrustCertChangeNotifier(PMCppCertHelper::certChainCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }

    PMCppUtilityHelper::readFile(ROOT_CERT_FILE, (OCByteString *) &PMCppCertHelper::s_trustCertChainArray);

    if (!m_PMCppCertHelper.saveTrustCertChain(PMCppCertHelper::s_trustCertChainArray.data, PMCppCertHelper::s_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &PMCppCertHelper::s_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }

    if(!m_PMCppCertHelper.provisionTrustCertChain(m_OwnedDevList, SIGNED_ASYMMETRIC_KEY, -1, PMCppCallbackHelper::provisionPostCB, OC_STACK_NO_RESOURCE))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }
}
#endif
