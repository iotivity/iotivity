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
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#include "RDHelper.h"


class RDServerTest_btc: public ::testing::Test
{
protected:

    RDHelper* m_rdHelper;

    RDServerTest_btc()
    {
        m_rdHelper = nullptr;
    }

    virtual void SetUp()
    {
        CommonTestUtil::runCommonTCSetUpPart();
        m_rdHelper = RDHelper::getInstance();
    }

    virtual void TearDown()
    {
        CommonTestUtil::runCommonTCTearDownPart();
        free(m_rdHelper);
    }
};

/**
 * @since             2015-08-26
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see                OCStackResult OCRDStop()
 * @objective         Test OCRDStart function positively
 * @target             OCStackResult OCRDStart();
 * @test_data         none
 * @pre_condition     Call OCInit (This function Initializes the OC Stack)
 * @procedure         Call start RD and check return result
 * @post_condition     OCRDStop
 * @expected         Initialization will succeed and return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__WINDOWS__)
TEST_F(RDServerTest_btc, StartRDServer_SRC_P)
{
    try
    {
        OCStackResult actualResult = m_rdHelper->rdCheckStartServerStatus(RD_START_SERVER_WITH_INIT);
        CommonUtil::waitInSecond(RD_SLEEPTIME_MAX);
        ASSERT_EQ(OC_STACK_OK,actualResult)<< "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResult);

        m_rdHelper->rdStopServer();
        CommonUtil::waitInSecond(RD_SLEEPTIME_MIN);
    }
    catch (std::exception &ex)
    {
        FAIL() << "Exception occurred : " << ex.what();
    }

    SUCCEED();
}
#endif

/**
 * @since             2015-08-26
 * @see                OCStackResult OCRDStop()
 * @objective         Test RD stop without calling OCRDStart and observed that
 *                     API will not crashed and will be return OC_STACK_NO_RESOURCE
 * @target             OCStackResult OCRDStop();
 * @test_data         none
 * @pre_condition     none
 * @procedure         Call stop RD.
 * @post_condition     none
 * @expected         OCRDStop will be executed and will return OC_STACK_NO_RESOURCE
 */
#if defined(__LINUX__) || defined(__WINDOWS__)
TEST_F(RDServerTest_btc, StopRDServer_ETC_P)
{
    try
    {
        OCStackResult actualResult = m_rdHelper->rdStopServer();
        ASSERT_EQ(OC_STACK_NO_RESOURCE,actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResult);
    }
    catch (std::exception &ex)
    {
        FAIL() << "Exception occurred : " << ex.what();
    }

    SUCCEED();
}
#endif

/**
 * @since             2015-10-15
 * @see               OCStackResult OCRDDatabaseGetStorageFilename()
 * @objective         Test OCRDDatabaseGetStorageFilename function positively
 * @target            OCStackResult OCRDDatabaseGetStorageFilename();
 * @test_data         none
 * @pre_condition     none
 * @post_condition    none
 * @expected          OCRDDatabaseGetStorageFilename will be executed and will not return NULL
 */
#if defined(__LINUX__) || defined(__WINDOWS__)
TEST_F(RDServerTest_btc, RDDatabaseGetStorageFilename_SRC_P)
{
    try
    {
        const char *receivedStorageFilename;
        receivedStorageFilename = OCRDDatabaseGetStorageFilename();
        ASSERT_NE((const char *)NULL ,&receivedStorageFilename) << "OCRDDatabaseGetStorageFilename failed.";
    }
    catch (std::exception &ex)
    {
        FAIL() << "Exception occurred : " << ex.what();
    }

    SUCCEED();
}
#endif

/**
 * @since             2015-10-15
 * @see               OCStackResult OCRDDatabaseGetStorageFilename()
 * @objective         Test OCRDDatabaseGetStorageFilename function positively
 * @target            OCStackResult OCRDDatabaseGetStorageFilename();
 * @test_data         none
 * @pre_condition     none
 * @post_condition    none
 * @expected          OCRDDatabaseGetStorageFilename will be executed and will not return emtpy String
 */
#if defined(__LINUX__) || defined(__WINDOWS__)
TEST_F(RDServerTest_btc, RDDatabaseGetStorageFilename_ESV_P)
{
    try
    {
        const char *receivedStorageFilename;
        receivedStorageFilename = OCRDDatabaseGetStorageFilename();
        ASSERT_NE((const char *)" " ,&receivedStorageFilename) << "OCGetServerInstanceIDString failed.";
    }
    catch (std::exception &ex)
    {
        FAIL() << "Exception occurred : " << ex.what();
    }

    SUCCEED();
}
#endif

/**
 * @since             2015-10-15
 * @see               OCStackResult OCRDDatabaseSetStorageFilename()
 * @objective         Test OCRDDatabaseSetStorageFilename function positively
 *                    Sets the filename to be used for database persistent storage
 * @target            OCStackResult OCRDDatabaseSetStorageFilename(const char *filename);
 * @test_data         filename, the name of database persistent storage
 * @pre_condition     none
 * @post_condition    none
 * @expected          OCRDDatabaseSetStorageFilename will be executed and return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__WINDOWS__)
TEST_F(RDServerTest_btc, RDDatabaseSetStorageFilename_SRC_P)
{
    try
    {
        OCStackResult actualResult = m_rdHelper->rdDatabaseSetStorageFilename(RD_PERSISTENT_STORAGE);
        ASSERT_EQ(OC_STACK_OK ,actualResult) << "OCRDDatabaseSetStorageFilename failed.";
    }
    catch (std::exception &ex)
    {
        FAIL() << "Exception occurred : " << ex.what();
    }

    SUCCEED();
}
#endif

/**
 * @since             2015-10-15
 * @see               OCStackResult OCRDDatabaseSetStorageFilename()
 * @objective         Test OCRDDatabaseSetStorageFilename function Negatively
 *                    Sets the filename to be used for database persistent storage
 * @target            OCStackResult OCRDDatabaseSetStorageFilename(const char *filename);
 * @test_data         filename passed as NULL
 * @pre_condition     none
 * @post_condition    none
 * @expected          OCRDDatabaseSetStorageFilename will be executed and return some other value should not return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__WINDOWS__)
TEST_F(RDServerTest_btc, RDDatabaseSetStorageFilename_NV_N)
{
    try
    {
        OCStackResult actualResult = m_rdHelper->rdDatabaseSetStorageFilename(NULL);
        ASSERT_NE(OC_STACK_OK ,actualResult) << "OCRDDatabaseSetStorageFilename failed.";
    }
    catch (std::exception &ex)
    {
        FAIL() << "Exception occurred : " << ex.what();
    }

    SUCCEED();
}
#endif