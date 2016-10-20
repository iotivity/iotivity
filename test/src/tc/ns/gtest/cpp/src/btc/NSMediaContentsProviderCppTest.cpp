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

#include "NSCommonCppHelper.h"
#include "NSMediaContents.h"
#include "NSProviderService.h"

class NSMediaContentsProviderCppTest_btc: public ::testing::Test
{
public:
    OIC::Service::NSMediaContents* m_pNSMediaContents;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

        m_pNSMediaContents = nullptr;
        m_pNSMediaContents = new OIC::Service::NSMediaContents();
        ASSERT_NE(nullptr,m_pNSMediaContents)<<
                "NSMediaContents constructor did not return instance";
        IOTIVITYTEST_LOG(INFO, "SetUp called");
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();

        IOTIVITYTEST_LOG(INFO, "TearDown called");
    }
};

/**
 * @since 2016-09-29
 * @objective Test 'NSMediaContents' constructor with positive basic way
 * @target NSMediaContents()
 * @test_data None
 * @pre_condition None
 * @procedure Perform NSMediaContents() API
 * @post_condition None
 * @expected Constructor should return NSMediaContents instance
 **/
#if defined(__LINUX__)
TEST_F(NSMediaContentsProviderCppTest_btc, MediaContentsConstructor_SRC_P)
{
    OIC::Service::NSMediaContents* nsMediaContents = nullptr;

    nsMediaContents = new OIC::Service::NSMediaContents();
    ASSERT_NE(nullptr,nsMediaContents)<< "NSMediaContents instance is not returned by constructor";
}
#endif

/**
 * @since 2016-09-29
 * @see NSMediaContents()
 * @objective Test 'setIconImage' API with positive basic way
 * @target void setIconImage(const std::string &iconImage)
 * @test_data iconImage = "iconimage1"
 * @pre_condition Perform NSMediaContents() constructor to get NSMediaContents instance
 * @procedure Perform setIconImage() API
 * @post_condition None
 * @expected Exceptions should not occur
 **/
#if defined(__LINUX__)
TEST_F(NSMediaContentsProviderCppTest_btc, SetIconImage_SRC_P)
{
    try
    {
        m_pNSMediaContents->setIconImage(ICONIMAGE_TO_SET);
    }
    catch (exception &e)
    {
        SET_FAILURE(
                "Exception occurs while calling setIconImage. Exception is: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-09-29
 * @see NSMediaContents()
 * @see void setIconImage(const std::string &iconImage)
 * @objective Test 'getIconImage' API with positive basic way
 * @target std::string getIconImage() const
 * @test_data None
 * @pre_condition   1. Perform NSMediaContents() constructor to get NSMediaContents instance
 *                  2. Perform setIconImage() API
 * @procedure Perform getIconImage() API
 * @post_condition None
 * @expected Icon image should be returned and it should be same as set icon image
 **/
#if defined(__LINUX__)
TEST_F(NSMediaContentsProviderCppTest_btc, GetIconImage_SRC_P)
{
    string iconImage = "";
    m_pNSMediaContents->setIconImage(ICONIMAGE_TO_SET);
    iconImage = m_pNSMediaContents->getIconImage();
    ASSERT_EQ(ICONIMAGE_TO_SET,iconImage)<< "getIconImage did not return the set icon image";
}
#endif
