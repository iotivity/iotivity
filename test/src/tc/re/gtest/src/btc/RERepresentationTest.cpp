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


class RERepresentationTest_btc: public ::testing::Test
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
 * @see RCSResourceObject, RCRemoteResourceObject
 * @objective Test RCSRepresentation() in a positive basic way
 * @target RCSRepresentation(const std::string& uri, const std::vector< std::string >& interfaces,
                    const std::vector< std::string >& resourceTypes);
 * @test_data uriString = "uri";
 * @pre_condition None
 * @procedure Perform RCSRepresentation() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RERepresentationTest_btc, RCSRepresentationWithData_SRC_P)
{

	std::string uriString = "uri";
	std::vector<std::string> interfaces;
	std::vector<std::string> resourceTypes;

    try
    {
    	RCSRepresentation representation(uriString, interfaces, resourceTypes);
    }
    catch (exception& e)
    {
        SET_FAILURE("exception occurred inside RCSRepresentationWithData_SRC_P: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-11
 * @see RCSResourceObject, RCRemoteResourceObject
 * @objective Test RCSRepresentation() With Data ResourceAttributes in a positive basic way
 * @target RCSRepresentation(const std::string& uri, const std::vector< std::string >& interfaces,
                    const std::vector< std::string >& resourceTypes,
                    const RCSResourceAttributes& attrs);
 * @test_data uriString = "uri";
 * @pre_condition None
 * @procedure Perform RCSRepresentation() With Data ResourceAttributes 
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RERepresentationTest_btc, RCSRepresentationWithDataResourceAttributes_SRC_P)
{

	std::string uriString = "uri";
	std::vector<std::string> interfaces;
	std::vector<std::string> resourceTypes;
	RCSResourceAttributes resourceAttributes;
    try
    {
    	RCSRepresentation representation(uriString, interfaces, resourceTypes, resourceAttributes);
    }
    catch (exception& e)
    {
        SET_FAILURE("exception occurred inside RCSRepresentationWithDataResourceAttributes_SRC_P: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-10
 * @see RCSResourceObject, RCRemoteResourceObject
 * @objective Test getUri() in a positive basic way
 * @target std::string getUri() const;
 * @test_data None
 * @pre_condition 
 * @procedure Perform getUri() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RERepresentationTest_btc, RCSRepresentationGetUri_SRC_P)
{
	RCSRepresentation representation;
    try
    {
    	representation.getUri();
    }
    catch (exception& e)
    {
        SET_FAILURE("exception occurred inside RCSRepresentationGetUri_SRC_P: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-10
 * @see RCSResourceObject, RCRemoteResourceObject
 * @objective Test setUri() in a positive basic way
 * @target  void setUri(std::string uri)
 * @test_data uri = "uri";
 * @pre_condition 
 * @procedure Perform setUri() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RERepresentationTest_btc, RCSRepresentationSetUri_SRC_P)
{
	RCSRepresentation representation;
	std::string uri = "uri";
    try
    {
    	representation.setUri(uri);
    }
    catch (exception& e)
    {
        SET_FAILURE("exception occurred inside RCSRepresentationSetUri_SRC_P: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-11
 * @see RCSResourceObject, RCRemoteResourceObject
 * @objective Test setUri() API with empty uri
 * @target  void setUri(std::string uri)
 * @test_data uri = "";
 * @pre_condition 
 * @procedure Perform setUri() with empty value
 * @post_condition None
 * @expected The API should throw exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RERepresentationTest_btc, RCSRepresentationSetUri_ESV_N)
{
	RCSRepresentation representation;
	std::string emptyString = "";
    try
    {
    	representation.setUri(emptyString);
    }
    catch (exception& e)
    {
    	SET_FAILURE("exception occurred inside RCSRepresentationSetUri_ESV_N: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-10
 * @see RCSResourceObject, RCRemoteResourceObject
 * @objective Test addInterface() API in a positive basic way
 * @target void addInterface(std::string interface)
 * @test_data interface = "interface";
 * @pre_condition 
 * @procedure Perform addInterface() API 
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RERepresentationTest_btc, RCSRepresentationAddInterface_SRC_P)
{
	RCSRepresentation representation;
	std::string interface = "interface";
    try
    {
    	representation.addInterface(interface);
    }
    catch (exception& e)
    {
        SET_FAILURE("exception occurred inside RCSRepresentationAddInterface_SRC_P: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-11
 * @see RCSResourceObject, RCRemoteResourceObject
 * @objective Test addInterface() with empty interface
 * @target void addInterface(std::string interface)
 * @test_data interface = "";
 * @pre_condition None
 * @procedure perform addInterface() API with empty value
 * @post_condition None
 * @expected The API Should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RERepresentationTest_btc, RCSRepresentationAddInterface_ESV_N)
{
	RCSRepresentation representation;
	std::string emptyInterface = "";
    try
    {
    	representation.addInterface(emptyInterface);
    }
    catch (exception& e)
    {
        SET_FAILURE("exception occurred inside RCSRepresentationAddInterface_ESV_N: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-10
 * @see RCSResourceObject, RCRemoteResourceObject
 * @objective Test getInterfaces() with positive basic way
 * @target const std::vector< std::string >& getInterfaces() const;
 * @test_data None
 * @pre_condition None
 * @procedure Perform getInterfaces() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RERepresentationTest_btc, RCSRepresentationGetInterfaces_SRC_P)
{
	RCSRepresentation representation;
    try
    {
    	std::vector<std::string> interfaces = representation.getInterfaces();
    }
    catch (exception& e)
    {
        SET_FAILURE("exception occurred inside RCSRepresentationGetInterfaces_SRC_P: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-10
 * @see RCSResourceObject, RCRemoteResourceObject
 * @objective Test clearInterfaces() with positive basic way
 * @target  void clearInterfaces();
 * @test_data None
 * @pre_condition None
 * @procedure perform clearInterfaces() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RERepresentationTest_btc, RCSRepresentationClearInterfaces_SRC_P)
{
	RCSRepresentation representation;
    try
    {
    	representation.clearInterfaces();
    }
    catch (exception& e)
    {
        SET_FAILURE("exception occurred inside RCSRepresentationClearInterfaces_SRC_P: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-10
 * @see RCSResourceObject, RCRemoteResourceObject
 * @objective Test getResourceTypes() in a positive basic way
 * @target  const std::vector< std::string >& getResourceTypes() const;
 * @test_data None
 * @pre_condition None
 * @procedure Perform getResourceTypes() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RERepresentationTest_btc, RCSRepresentationGetResourceTypes_SRC_P)
{
	RCSRepresentation representation;
    try
    {
    	std::vector<std::string> interfaces = representation.getResourceTypes();
    }
    catch (exception& e)
    {
        SET_FAILURE("exception occurred inside RCSRepresentationGetResourceTypes_SRC_P: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-10
 * @see RCSResourceObject, RCRemoteResourceObject
 * @objective Test addResourceType() with positive basic way
 * @target  void addResourceType(std::string resourceType);
 * @test_data resourceType = "resourceType";
 * @pre_condition None
 * @procedure Perform addResourceType() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RERepresentationTest_btc, RCSRepresentationAddResourceTypes_SRC_P)
{
	RCSRepresentation representation;
	std::string resourceType = "resourceType";
    try
    {
    	representation.addResourceType(resourceType);
    }
    catch (exception& e)
    {
        SET_FAILURE("exception occurred inside RCSRepresentationAddResourceTypes_SRC_P: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-10
 * @see RCSResourceObject, RCRemoteResourceObject
 * @objective Test addResourceType() with empty resourceType
 * @target  void addResourceType(std::string resourceType);
 * @test_data resourceType = "";
 * @pre_condition None
 * @procedure Perform addResourceType() API with empty value
 * @post_condition None
 * @expected The API Should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RERepresentationTest_btc, RCSRepresentationAddResourceTypes_ESV_N)
{
	RCSRepresentation representation;
	std::string resourceType = "";
    try
    {
    	representation.addResourceType(resourceType);
    }
    catch (exception& e)
    {
        SET_FAILURE("exception occurred inside RCSRepresentationAddResourceTypes_ESV_N: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-10
 * @see RCSResourceObject, RCRemoteResourceObject
 * @objective Test clearResourceTypes() with positive basic way
 * @target  void clearResourceTypes();
 * @test_data None
 * @pre_condition None
 * @procedure Perform clearResourceTypes() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RERepresentationTest_btc, RCSRepresentationClearResourceTypes_SRC_P)
{
	RCSRepresentation representation;

    try
    {
    	representation.clearResourceTypes();
    }
    catch (exception& e)
    {
        SET_FAILURE("exception occurred inside RCSRepresentationClearResourceTypes_SRC_P: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-10
 * @see RCSResourceObject, RCRemoteResourceObject
 * @objective Test getAttributes() in a positive basic way
 * @target  const RCSResourceAttributes& getAttributes() const
 * @test_data None
 * @pre_condition None
 * @procedure Perform getAttributes() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RERepresentationTest_btc, RCSRepresentationGetAttributes_SRC_P)
{
	RCSRepresentation representation;

    try
    {
    	RCSResourceAttributes attributes = representation.getAttributes();
    }
    catch (exception& e)
    {
        SET_FAILURE("exception occurred inside RCSRepresentationGetAttributes_SRC_P: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-11
 * @see RCSResourceObject, RCRemoteResourceObject
 * @objective Test setAttributes() with positive basic way
 * @target  void setAttributes(const RCSResourceAttributes& attrs);
 * @test_data None
 * @pre_condition None
 * @procedure Perform setAttributes() API with Resource Attributes
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RERepresentationTest_btc, RCSRepresentationSetAttributes_SRC_P)
{
	RCSResourceAttributes resourceAttributes;
	RCSRepresentation representation;
	
    try
    {
    	representation.setAttributes(resourceAttributes);
    }
    catch (exception& e)
    {
        SET_FAILURE("exception occurred inside RCSRepresentationSetAttributes_SRC_P: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-11
 * @see RCSResourceObject, RCRemoteResourceObject
 * @objective Test getChildren() with positive basic way
 * @target  const std::vector< RCSRepresentation >& getChildren() const;
 * @test_data None
 * @pre_condition None
 * @procedure Perform getChildren() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RERepresentationTest_btc, RCSRepresentationGetChildren_SRC_P)
{
	RCSRepresentation representation;
	std::vector< RCSRepresentation > representationVector;
	
    try
    {
    	representationVector = representation.getChildren();
    }
    catch (exception& e)
    {
        SET_FAILURE("exception occurred inside RCSRepresentationGetChildren_SRC_P: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-11
 * @see RCSResourceObject, RCRemoteResourceObject
 * @objective Test addChild() in a positive basic way
 * @target  void addChild(RCSRepresentation child);
 * @test_data None
 * @pre_condition None
 * @procedure Perform addChild() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RERepresentationTest_btc, RCSRepresentationAddChild_SRC_P)
{
	RCSRepresentation representation;
	RCSRepresentation child;

    try
    {
    	representation.addChild(child);
    }
    catch (exception& e)
    {
        SET_FAILURE("exception occurred inside RCSRepresentationAddChild_SRC_P: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-11
 * @see RCSResourceObject, RCRemoteResourceObject
 * @objective Test setChildren() with positive basic way
 * @target  void setChildren(std::vector< RCSRepresentation > children);
 * @test_data None
 * @pre_condition None
 * @procedure Perform setChildren() API 
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RERepresentationTest_btc, RCSRepresentationSetChildren_SRC_P)
{
	RCSRepresentation representation;
	std::vector< RCSRepresentation > children;

    try
    {
    	representation.setChildren(children);
    }
    catch (exception& e)
    {
        SET_FAILURE("exception occurred inside RCSRepresentationSetChildren_SRC_P : " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-11
 * @see RCSResourceObject, RCRemoteResourceObject
 * @objective Test clearChildren() with positive basic way
 * @target  void clearChildren();
 * @test_data None
 * @pre_condition None
 * @procedure Perform clearChildren() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RERepresentationTest_btc, RCSRepresentationClearChildren_SRC_P)
{
	RCSRepresentation representation;

    try
    {
    	representation.clearChildren();
    }
    catch (exception& e)
    {
        SET_FAILURE("exception occurred inside RCSRepresentationClearChildren_SRC_P: " + std::string(e.what()));
    }
}
#endif
