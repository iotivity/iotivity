//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include <string>
#include "gtest/gtest.h"
#include "REHelper.h"


class RERepresentationAPITest_stc: public ::testing::Test
{
    virtual void SetUp()
    {
#ifdef __LINUX__        
        CommonUtil::launchApp(RE_SERVER_APP);
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
#endif
    }

    virtual void TearDown()
    {
#ifdef __LINUX__		
        CommonUtil::killApp(RE_SERVER_APP);
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
#endif
    }
};

/**
 * @since 2016-03-11
 * @see RCSResourceObject, RCRemoteResourceObject
 * @objective Test RCSRepresentation() with Get/Set Return Value
 * @target 1. void setUri(std::string uri);
 *         2. void getUri(std::string uri);
 * @test_data uriString = "uri";
 * @pre_condition None
 * @procedure 1. Set uri
 	 	 	  2. perform getUri
 	 	 	  3. check if it returns the same uri
 * @post_condition None
 * @expected returned uri should be matched
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RERepresentationAPITest_stc, RCSRepresentationSetGetURI_GSRV_P)
{
    std::string uriString = "uri";
    std::vector<std::string> interfaces;
    std::vector<std::string> resourceTypes;
    RCSRepresentation representation;

    try
    {
        representation.setUri(uriString);
        if(uriString != representation.getUri())
        {
            SET_FAILURE("Set/Get Uri is not successful");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("exception occurred inside RCSRepresentationSetGetURI_GSRV_P: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-11
 * @see RCSResourceObject, RCRemoteResourceObject
 * @objective Test RCSRepresentation add and get interface
 * @target void addInterface(std::string interface);
 * @target const std::vector< std::string >& getInterfaces() const;
 * @test_data Interface
 * @pre_condition None
 * @procedure 1. call addInterface
 	 	 	  2. call getInterface
 * @post_condition None
 * @expected getInterface should return size of interfaces
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RERepresentationAPITest_stc, RCSRepresentationAddInterfaceGetInterface_SQV_P)
{

    std::string interfaceString = "interface";

    RCSRepresentation representation;

    try
    {
        representation.addInterface(interfaceString);
        std::vector<std::string> interfaceList = representation.getInterfaces();
        if(interfaceList.size() > 0)
        {
            std::string interface = interfaceList[0];
            if(interface != interfaceString)
            {
                SET_FAILURE("Didn't return same interface string");
            }
        }
        else
        {
            SET_FAILURE("Interface size is less then 1 even after adding interface");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("exception occurred: " + std::string(e.what()));
    }

}
#endif
