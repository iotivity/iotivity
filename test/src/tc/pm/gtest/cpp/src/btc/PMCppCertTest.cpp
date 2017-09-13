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

class PMCppCertTest_btc: public ::testing::Test
{
protected:
    PMCppHelper m_PMCppHelper;
    PMCppCertHelper m_PMCppCertHelper;
    DeviceList_t m_UnownedDevList, m_OwnedDevList;
    OicSecAcl_t *m_acl1, *m_acl2;

    virtual void SetUp()
    {
        CommonTestUtil::runCommonTCSetUpPart();
        CommonUtil::killApp(KILL_SERVERS);
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        PMCppUtilityHelper::removeAllResFile();
        CommonUtil::copyFile(JUSTWORKS_SERVER_CBOR_O1_UNOWNED_BACKUP, JUSTWORKS_SERVER_CBOR_O1);
        CommonUtil::copyFile(JUSTWORKS_SERVER_CBOR_O2_UNOWNED_BACKUP, JUSTWORKS_SERVER_CBOR_O2);
        CommonUtil::copyFile(RANDOMPIN_SERVER_CBOR_01_UNOWNED_BACKUP, RANDOMPIN_SERVER_CBOR_01);
        CommonUtil::copyFile(MV_JUSTWORKS_CBOR_01_UNOWNED_BACKUP, MV_JUSTWORKS_CBOR_01);
        CommonUtil::copyFile(CLIENT_CBOR_01_UNOWNED_BACKUP, CLIENT_CBOR_01);
        CommonUtil::copyFile(ROOT_CERT_UNOWNED_BACKUP, ROOT_CERT);
        CommonUtil::waitInSecond(DELAY_LONG);
        m_UnownedDevList.clear();
        m_OwnedDevList.clear();
        m_acl1 = NULL;
        m_acl2 = NULL;

        if(!m_PMCppCertHelper.provisionInit())
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
 * @since           2016-02-28
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult registerTrustCertChangeNotifier(CertChainCallBack);
 * @objective       test saveTrustCertChain positively
 * @target          static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @test_data       Regular data for saveTrustCertChain
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call registerTrustCertChangeNotifier
 *                  3. call saveTrustCertChain
 * @post_condition  None
 * @expected        saveTrustCertChain will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppCertTest_btc, SaveTrustCertChain_SRC_RV_P)
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
}
#endif

/**
 * @since           2017-02-28
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test saveTrustCertChain negatively with credId as NULL
 * @target          static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @test_data       credId as NULL
 * @pre_condition   None
 * @procedure       1. call provisionInit
 *                  2. call saveTrustCertChain
 * @post_condition  None
 * @expected        saveTrustCertChain will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppCertTest_btc, SaveTrustCertChainCredId_NV_N)
{
    PMCppUtilityHelper::readFile(ROOT_CERT_FILE, (OCByteString *) &PMCppCertHelper::s_trustCertChainArray);

    if (!m_PMCppCertHelper.saveTrustCertChain(PMCppCertHelper::s_trustCertChainArray.data, PMCppCertHelper::s_trustCertChainArray.len,
                    OIC_ENCODING_PEM, NULL, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-28
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test saveTrustCertChain negatively with chainSize as -1
 * @target          static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @test_data       chainSize as -1
 * @pre_condition   None
 * @procedure       1. call provisionInit
 *                  2. call saveTrustCertChain
 * @post_condition  None
 * @expected        saveTrustCertChain will return OC_STACK_NO_MEMORY
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppCertTest_btc, SaveTrustCertChainCertChainLen_LOBV_N)
{
    PMCppUtilityHelper::readFile(ROOT_CERT_FILE, (OCByteString *) &PMCppCertHelper::s_trustCertChainArray);

    if (!m_PMCppCertHelper.saveTrustCertChain(PMCppCertHelper::s_trustCertChainArray.data, -1,
                    OIC_ENCODING_PEM, &PMCppCertHelper::s_credId, OC_STACK_NO_MEMORY))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-28
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test saveTrustCertChain negatively with trustCertChain as NULL
 * @target          static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @test_data       trustCertChain as NULL
 * @pre_condition   None
 * @procedure       1. call provisionInit
 *                  2. call saveTrustCertChain
 * @post_condition  None
 * @expected        saveTrustCertChain will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppCertTest_btc, SaveTrustCertChainCertChain_NV_N)
{
    PMCppUtilityHelper::readFile(ROOT_CERT_FILE, (OCByteString *) &PMCppCertHelper::s_trustCertChainArray);

    if (!m_PMCppCertHelper.saveTrustCertChain(NULL, PMCppCertHelper::s_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &PMCppCertHelper::s_credId, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-28
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       test readTrustCertChain positively
 * @target          static OCStackResult readTrustCertChain(uint16_t credId, uint8_t **trustCertChain, size_t *chainSize);
 * @test_data       trustCertChain as NULL
 * @pre_condition   None
 * @procedure       1. call provisionInit
 *                  2. call saveTrustCertChain
 *                  3. call readTrustCertChain
 * @post_condition  None
 * @expected        readTrustCertChain will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppCertTest_btc, ReadTrustCertChain_SRC_RV_P)
{
    PMCppUtilityHelper::readFile(ROOT_CERT_FILE, (OCByteString *) &PMCppCertHelper::s_trustCertChainArray);

    if (!m_PMCppCertHelper.saveTrustCertChain(PMCppCertHelper::s_trustCertChainArray.data, PMCppCertHelper::s_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &PMCppCertHelper::s_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }

    ByteArray_t trustCertChainArray =
    {   0, 0};

    if (!m_PMCppCertHelper.readTrustCertChain(PMCppCertHelper::s_credId, &trustCertChainArray.data,
                    &trustCertChainArray.len, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-28
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       test readTrustCertChain negatively as credId as -1
 * @target          static OCStackResult readTrustCertChain(uint16_t credId, uint8_t **trustCertChain, size_t *chainSize);
 * @test_data       credId as -1
 * @pre_condition   None
 * @procedure       1. call provisionInit
 *                  2. call saveTrustCertChain
 *                  3. call readTrustCertChain
 * @post_condition  None
 * @expected        readTrustCertChain will return OC_STACK_ERROR
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppCertTest_btc, ReadTrustCertChainCredId_LOBV_N)
{
    PMCppUtilityHelper::readFile(ROOT_CERT_FILE, (OCByteString *) &PMCppCertHelper::s_trustCertChainArray);

    if (!m_PMCppCertHelper.saveTrustCertChain(PMCppCertHelper::s_trustCertChainArray.data, PMCppCertHelper::s_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &PMCppCertHelper::s_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }

    ByteArray_t trustCertChainArray =
    {   0, 0};

    if (!m_PMCppCertHelper.readTrustCertChain(-1, &trustCertChainArray.data,
                    &trustCertChainArray.len, OC_STACK_ERROR))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-28
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       test readTrustCertChain negatively with trustCertChain as NULL
 * @target          static OCStackResult readTrustCertChain(uint16_t credId, uint8_t **trustCertChain, size_t *chainSize);
 * @test_data       trustCertChain as NULL
 * @pre_condition   None
 * @procedure       1. call provisionInit
 *                  2. call saveTrustCertChain
 *                  3. call readTrustCertChain
 * @post_condition  None
 * @expected        readTrustCertChain will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppCertTest_btc, ReadTrustCertChainChainArray_NV_N)
{
    PMCppUtilityHelper::readFile(ROOT_CERT_FILE, (OCByteString *) &PMCppCertHelper::s_trustCertChainArray);

    if (!m_PMCppCertHelper.saveTrustCertChain(PMCppCertHelper::s_trustCertChainArray.data, PMCppCertHelper::s_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &PMCppCertHelper::s_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }

    ByteArray_t trustCertChainArray =
    {   0, 0};

    if (!m_PMCppCertHelper.readTrustCertChain(PMCppCertHelper::s_credId, NULL,
                    &trustCertChainArray.len, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-28
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       test readTrustCertChain negatively with chainSize as NULL
 * @target          static OCStackResult readTrustCertChain(uint16_t credId, uint8_t **trustCertChain, size_t *chainSize);
 * @test_data       chainSize as NULL
 * @pre_condition   None
 * @procedure       1. call provisionInit
 *                  2. call saveTrustCertChain
 *                  3. call readTrustCertChain
 * @post_condition  None
 * @expected        readTrustCertChain will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppCertTest_btc, ReadTrustCertChainChainLen_NV_N)
{
    PMCppUtilityHelper::readFile(ROOT_CERT_FILE, (OCByteString *) &PMCppCertHelper::s_trustCertChainArray);

    if (!m_PMCppCertHelper.saveTrustCertChain(PMCppCertHelper::s_trustCertChainArray.data, PMCppCertHelper::s_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &PMCppCertHelper::s_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }

    ByteArray_t trustCertChainArray =
    {   0, 0};

    if (!m_PMCppCertHelper.readTrustCertChain(PMCppCertHelper::s_credId, &trustCertChainArray.data,
                    NULL, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-28
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test registerTrustCertChangeNotifier positively
 * @target          static OCStackResult registerTrustCertChangeNotifier(CertChainCallBack);
 * @test_data       Regular data for registerTrustCertChangeNotifier
 * @pre_condition   None
 * @procedure       1. call provisionInit
 *                  2. call registerTrustCertChangeNotifier
 * @post_condition  None
 * @expected        registerTrustCertChangeNotifier will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppCertTest_btc, RegisterTrustCertChangeNotifier_SRC_RV_P)
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
}
#endif

/**
 * @since           2016-02-28
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test registerTrustCertChangeNotifier negatively calling it multiple time
 * @target          static OCStackResult registerTrustCertChangeNotifier(CertChainCallBack);
 * @test_data       Regular data for registerTrustCertChangeNotifier
 * @pre_condition   None
 * @procedure       1. call provisionInit
 *                  2. call registerTrustCertChangeNotifier
 *                  3. call registerTrustCertChangeNotifier
 * @post_condition  None
 * @expected        registerTrustCertChangeNotifier will return OC_STACK_ERROR
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppCertTest_btc, RegisterTrustCertChangeNotifierMultipleTime_EG_N)
{
    if (!m_PMCppCertHelper.registerTrustCertChangeNotifier(PMCppCertHelper::certChainCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }

    if (!m_PMCppCertHelper.registerTrustCertChangeNotifier(PMCppCertHelper::certChainCB, OC_STACK_ERROR))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-28
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test registerTrustCertChangeNotifier negatively with CertChainCallBack as NULL
 * @target          static OCStackResult registerTrustCertChangeNotifier(CertChainCallBack);
 * @test_data       CertChainCallBack as NULL
 * @pre_condition   None
 * @procedure       1. call provisionInit
 *                  2. call registerTrustCertChangeNotifier
 * @post_condition  None
 * @expected        registerTrustCertChangeNotifier will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppCertTest_btc, RegisterTrustCertChangeNotifierCb_NV_N)
{
    if (!m_PMCppCertHelper.registerTrustCertChangeNotifier(NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-28
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult registerTrustCertChangeNotifier(CertChainCallBack);
 * @objective       test removeTrustCertChangeNotifier positively
 * @target          static OCStackResult removeTrustCertChangeNotifier
 * @test_data       None
 * @pre_condition   None
 * @procedure       1. call provisionInit
 *                  2. call registerTrustCertChangeNotifier
 *                  3. call removeTrustCertChangeNotifier
 * @post_condition  None
 * @expected        removeTrustCertChangeNotifier will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppCertTest_btc, RemoveTrustCertChangeNotifier_SRC_P)
{
    if (!m_PMCppCertHelper.registerTrustCertChangeNotifier(PMCppCertHelper::certChainCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }

    if (!m_PMCppCertHelper.removeTrustCertChangeNotifier())
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-28
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult registerTrustCertChangeNotifier(CertChainCallBack);
 * @objective       test removeTrustCertChangeNotifier positively without registering CertChainCallBack
 * @target          static OCStackResult removeTrustCertChangeNotifier
 * @test_data       None
 * @pre_condition   None
 * @procedure       1. call provisionInit
 *                  2. call registerTrustCertChangeNotifier
 *                  3. call removeTrustCertChangeNotifier
 * @post_condition  None
 * @expected        removeTrustCertChangeNotifier will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppCertTest_btc, RemoveTrustCertChangeNotifier_RV_P)
{
    if (!m_PMCppCertHelper.removeTrustCertChangeNotifier())
    {
        SET_FAILURE(m_PMCppCertHelper.getFailureMessage());
    }
}
#endif
