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
#include "PMCppCallbackHelper.h"
#include "PMCppUtilityHelper.h"
#include "CommonTestUtil.h"

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

        if(!m_PMCppHelper.provisionInit())
        {
            SET_FAILURE(m_PMCppHelper.getFailureMessage());
            return;
        }
    }

    virtual void TearDown()
    {
        CommonTestUtil::runCommonTCTearDownPart();
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
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppCredTest_btc, GetCredentialType_SRC_P)
{
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
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
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
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppCredTest_btc, SetCredentialType_SRC_P)
{
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
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppCredTest_btc, SetCredentialKeySize_SRC_P)
{
    Credential cred(SYMMETRIC_PAIR_WISE_KEY, OWNER_PSK_LENGTH_128);
    cred.setCredentialKeySize(OWNER_PSK_LENGTH_256);

    if (OWNER_PSK_LENGTH_256 != cred.getCredentialKeySize())
    {
        SET_FAILURE("getCredentialKeySize Failed");
    }
}
#endif
