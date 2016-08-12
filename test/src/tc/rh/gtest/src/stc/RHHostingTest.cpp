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


#include "RHCommon.h"

#define OC_WELL_KNOWN_COORDINATING_QUERY "/oic/res?rt=oic.r.resourcehosting"

class RHHostingTest_stc: public ::testing::Test
{

protected:

    RHCommon m_RHCommon;
    OCStackResult m_OCStackResult;
    std::string m_errorMessage;

    virtual void SetUp()
    {
        KILL_ALL();
        m_OCStackResult = OC_STACK_ERROR;
        m_errorMessage = "";
        m_RHCommon.m_curResource = NULL;
        m_RHCommon.m_curResource2 = NULL;

        m_RHCommon.resourceOneTemp = 0;
        m_RHCommon.resourceTwoTemp = 0;
        m_RHCommon.resourceOneHum = 0;
        m_RHCommon.resourceTwoHum = 0;
    }

    virtual void TearDown()
    {
        KILL_ALL();
    }
};


/**
 * @since			2015-04-15
 * @see 			none
 * @objective 		Call OICStopCoordinate API after successfully calling OICStartCoordinate & OICStopCoordinate API once
 * @target 			OICStopCoordinate
 * @test_data 		none
 * @pre_condition 	none
 * @procedure 		1. Call OICStartCoordinate
	 *            	2. Call OICStopCoordinate
	 *            	3. Clear Resource
	 *            	4. Call OICStopCoordinate
 * @post_condition 	none
 * @expected 		OICStopCoordinate will stop & will not crash.
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RHHostingTest_stc, StartStopAndStopCoordinateWithoutInitialize_DSCC_N)
{
    ASSERT_EQ(m_RHCommon.startOCResourceHost(), true) << m_RHCommon.m_errorMessage ;

    EXPECT_EQ(OICStartCoordinate(), OC_STACK_OK) << START_ERR_MSG;

    EXPECT_EQ(OICStopCoordinate(), OC_STACK_OK)<< STOP_ERR_MSG;

    ASSERT_EQ(m_RHCommon.stopOCResourceHost(), true) << m_RHCommon.m_errorMessage ;


    EXPECT_EQ(OICStopCoordinate(), OC_STACK_OK)<< STOP_ERR_MSG;
}
#endif

 /**
  * @since			2016-05-03
  * @see 			none
  * @objective 		Call OICStartCoordinate API after successfully calling OICStartCoordinate & OICStopCoordinate API once
  * @target 		OICStartCoordinate
  * @test_data 		none
  * @pre_condition 	none
  * @procedure 		1. Call OICStartCoordinate
  *            		2. Call OICStopCoordinate
  *            		3. Call OICStartCoordinate
  * @post_condition none
  * @expected 		OICStartCoordinate will start successfully.
  */
 #if defined(__LINUX__) || defined(__TIZEN__)
 TEST_F(RHHostingTest_stc, StartStopAndStartCoordinate_SRC_P)
 {

     ASSERT_EQ(OICStartCoordinate(), OC_STACK_OK) << START_ERR_MSG;

     ASSERT_EQ(OICStopCoordinate(), OC_STACK_OK)<< STOP_ERR_MSG;

     ASSERT_EQ(OICStartCoordinate(), OC_STACK_OK) << "FAILED to start Coordinate after start & Stop Once";
 }
 #endif

/**
 * @since			2015-04-15
 * @see 			none
 * @objective 		Call OICStartCoordinate & OICStopCoordinate without Initialize Resource
 * @target 			OICStartCoordinate
 * @target 			OICStopCoordinate
 * @test_data 		none
 * @pre_condition 	none
 * @procedure 		1. Call OICStartCoordinate
 *            		2. Call OICStopCoordinate
 * @post_condition 	none
 * @expected 		OICStartCoordinate & OICStopCoordinate will function successfully.
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RHHostingTest_stc, StartStopCoordinateWithoutInitializing_DSCC_N)
{
    EXPECT_EQ(OICStartCoordinate(), OC_STACK_OK) << START_ERR_MSG;

    EXPECT_EQ(OICStopCoordinate(), OC_STACK_OK)<< STOP_ERR_MSG;
}
#endif

/**
 * @since 			2015-04-15
 * @see 			none
 * @objective 		test if resource host can host resource
 * @target 			OCPlatform::findResource()
 * @test_data 		none
 * @pre_condition 	1. Run resource
 *                	2. Run resource host
 * @procedure 		Call findResource API to check if resource is found
 * @post_condition 	Stop resource & resource host
 * @expected 		Resource should be found successfully.
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RHHostingTest_stc, FindHostableResource_SRC_P)
{

    RUN_HOSTABLE_RESOURCE_ONE();
    RUN_RESOURCE_HOST();
    CommonUtil::waitInSecond(20);

    OCPlatform::findResource("" , OC_WELL_KNOWN_COORDINATING_QUERY , CT_DEFAULT, &m_RHCommon.foundResource);
    CommonUtil::waitInSecond(15);

    ASSERT_EQ(m_RHCommon.verifyTempHumSensorOne(m_errorMessage),true) << m_errorMessage;
}
#endif

/**
 * @since 			2015-04-15
 * @see 			none
 * @objective 		test if resource host can host multiple resources
 * @target 			OCPlatform::findResource()
 * @test_data 		none
 * @pre_condition 	1. Run two resources
 *                	2. Run resource host
 * @procedure 		call findResource API to check if resource is found
 * @post_condition 	Stop resources & resource host
 * @expected 		Both resources should be found successfully
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RHHostingTest_stc, FindMultipleHostableResource_SRC_P)
{
    RUN_HOSTABLE_RESOURCE_ONE();
    RUN_HOSTABLE_RESOURCE_TWO();
    CommonUtil::waitInSecond(10);
    RUN_RESOURCE_HOST();
    CommonUtil::waitInSecond(30);

    OCPlatform::findResource("" , OC_WELL_KNOWN_COORDINATING_QUERY , CT_DEFAULT, &m_RHCommon.foundResource);
    CommonUtil::waitInSecond(35);

    EXPECT_EQ(m_RHCommon.verifyTempHumSensorOne(m_errorMessage),true) << m_errorMessage;
    EXPECT_EQ(m_RHCommon.verifyTempHumSensorTwo(m_errorMessage),true) << m_errorMessage;
}
#endif

/**
 * @since 			2015-04-15
 * @see 			none
 * @objective 		test if resource host can handle observe request
 * @target 			observe(ObserveType, QueryParamsMap, onObsreve)
 * @test_data 		none
 * @pre_condition 	1. Run resource
 *                	2. Run resource host
 * @procedure 		1. Call find resource
 *            		2. Check if resource is found
 *            		3. Send observe request
 * @post_condition 	Stop resources & resource host
 * @expected 		Data should successfully come from resource
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RHHostingTest_stc, SendObserveRequest_SRC_P)
{
    RUN_HOSTABLE_RESOURCE_ONE();
    CommonUtil::waitInSecond(10);
    RUN_RESOURCE_HOST();
    CommonUtil::waitInSecond(15);

    OCPlatform::findResource("" , OC_WELL_KNOWN_COORDINATING_QUERY , CT_DEFAULT, &m_RHCommon.foundResource);
    CommonUtil::waitInSecond(15);
    ASSERT_EQ(m_RHCommon.verifyTempHumSensorOne(m_errorMessage),true) << m_errorMessage;

    QueryParamsMap test;

    EXPECT_EQ(OC_STACK_OK , m_RHCommon.m_curResource->observe(ObserveType::Observe , test , std::bind(&RHCommon::onObserve, &m_RHCommon,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4))) << FAIL_RES_OBSERVE;
    CommonUtil::waitInSecond(25);

    ASSERT_EQ(m_RHCommon.verifyObserveDataFromSensorOne(m_errorMessage),true) << m_errorMessage;
}
#endif

/**
 * @since 			2015-04-15
 * @see 			none
 * @objective 		test if resource host can handle observe request to multiple resources
 * @target 			observe(ObserveType, QueryParamsMap, onObsreve)
 * @test_data 		none
 * @pre_condition 	1. Run two resource
 *                	2. Run resource host
 * @procedure 		1. Call find resource
 *            		2. Check if resources are found
 *            		3. Send observe request to both resources
 * @post_condition 	Stop resources & resource host
 * @expected 		Data should come from both resources
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RHHostingTest_stc, SendObserveRequestToMultipleResource_SRC_P)
{
    RUN_HOSTABLE_RESOURCE_ONE();
    RUN_HOSTABLE_RESOURCE_TWO();
    CommonUtil::waitInSecond(10);
    RUN_RESOURCE_HOST();
    CommonUtil::waitInSecond(30);

    OCPlatform::findResource("" , OC_WELL_KNOWN_COORDINATING_QUERY , CT_DEFAULT, &m_RHCommon.foundResource);
    CommonUtil::waitInSecond(35);

    EXPECT_EQ(m_RHCommon.verifyTempHumSensorOne(m_errorMessage),true) << m_errorMessage;
    EXPECT_EQ(m_RHCommon.verifyTempHumSensorTwo(m_errorMessage),true) << m_errorMessage;

    QueryParamsMap test;
    EXPECT_EQ(OC_STACK_OK, m_RHCommon.m_curResource->observe(ObserveType::Observe , test , std::bind(&RHCommon::onObserve, &m_RHCommon,
                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4))) << FAIL_RES_OBSERVE;

    CommonUtil::waitInSecond(25);

    EXPECT_EQ(OC_STACK_OK,  m_RHCommon.m_curResource2->observe(ObserveType::Observe , test , std::bind(&RHCommon::onObserve, &m_RHCommon,
                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4))) << FAIL_RES_OBSERVE;
    CommonUtil::waitInSecond(25);

    EXPECT_EQ(m_RHCommon.verifyObserveDataFromSensorOne(m_errorMessage),true) << m_errorMessage;
    ASSERT_EQ(m_RHCommon.verifyObserveDataFromSensorTwo(m_errorMessage),true) << m_errorMessage;
}
#endif

/**
 * @since 			2015-04-15
 * @see 			none
 * @objective 		test if resource host can handle get request
 * @target 			get(QueryParamsMap, onGet)
 * @test_data 		none
 * @pre_condition 	1. Run resource
 *                	2. Run resource host
 * @procedure 		1. Call find resource
 *            		2. Check if resource is found
 *            		3. Send get request to resource
 * @post_condition 	stop resources & resource host
 * @expected 		Data should come from resource
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RHHostingTest_stc, SendGetRequest_SRC_P)
{
    RUN_HOSTABLE_RESOURCE_ONE();
    RUN_RESOURCE_HOST();
    CommonUtil::waitInSecond(15);

    OCPlatform::findResource("" , OC_WELL_KNOWN_COORDINATING_QUERY , CT_DEFAULT, &m_RHCommon.foundResource);
    CommonUtil::waitInSecond(10);

    QueryParamsMap test;
    EXPECT_EQ(OC_STACK_OK, m_RHCommon.m_curResource->get(test, std::bind(&RHCommon::onGet, &m_RHCommon,
                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))) << GET_FAIL_MSG;

    CommonUtil::waitInSecond(10);
    ASSERT_EQ(m_RHCommon.verifyObserveDataFromSensorOne(m_errorMessage),true) << m_errorMessage;
}
#endif

/**
 * @since 			2015-04-15
 * @see 			none
 * @objective 		test if resource host can handle get request to multiple resources
 * @target 			get(QueryParamsMap, onGet)
 * @test_data 		none
 * @pre_condition 	1. Run two resource
 *                	2. Run resource host
 * @procedure 		1. Call find resource
 *            		2. Check if resources are found
 *            		3. Send get request to both resource
 * @post_condition 	stop resources & resource host
 * @expected 		Data should come from both resource
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RHHostingTest_stc, SendGetRequestToMultipleResource_SRC_P)
{
    RUN_HOSTABLE_RESOURCE_ONE();
    RUN_HOSTABLE_RESOURCE_TWO();
    RUN_RESOURCE_HOST();
    CommonUtil::waitInSecond(30);

    OCPlatform::findResource("" , OC_WELL_KNOWN_COORDINATING_QUERY , CT_DEFAULT, &m_RHCommon.foundResource);
    CommonUtil::waitInSecond(35);

    QueryParamsMap test;
    EXPECT_EQ(OC_STACK_OK ,m_RHCommon.m_curResource->get(test, std::bind(&RHCommon::onGet, &m_RHCommon,
                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))) << GET_FAIL_MSG;
    CommonUtil::waitInSecond(10);

    EXPECT_EQ(OC_STACK_OK , m_RHCommon.m_curResource2->get(test, std::bind(&RHCommon::onGet, &m_RHCommon,
                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))) << GET_FAIL_MSG;
    CommonUtil::waitInSecond(10);

    EXPECT_EQ(m_RHCommon.verifyObserveDataFromSensorOne(m_errorMessage),true) << m_errorMessage;
    ASSERT_EQ(m_RHCommon.verifyObserveDataFromSensorTwo(m_errorMessage),true) << m_errorMessage;
}
#endif

/**
 * @since 			2015-04-15
 * @see 			none
 * @objective 		test if resource host can handle get request
 * @target 			post(OCRepresentation, QueryParamsMap, onGet)
 * @test_data 		none
 * @pre_condition 	1. Run resource
 *                	2. Run resource host
 * @procedure 		1. Call find resource
 *            		2. Check if resource is found
 *            		3. Send post request to resource
 *            		4. send get request to resource to verify data
 * @post_condition 	Stop resources & resource host
 * @expected 		Resource parameter should be changed
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RHHostingTest_stc, SendPostRequest_SRC_P)
{
    RUN_HOSTABLE_RESOURCE_ONE();
    RUN_RESOURCE_HOST();
    CommonUtil::waitInSecond(15);

    OCPlatform::findResource("" , OC_WELL_KNOWN_COORDINATING_QUERY , CT_DEFAULT, &m_RHCommon.foundResource);
    CommonUtil::waitInSecond(10);

    OCRepresentation rep;
    rep.setValue("temperature", 0);
    rep.setValue("humidity", 0);

    QueryParamsMap test;
    EXPECT_EQ(OC_STACK_OK , m_RHCommon.m_curResource->post(rep, test, std::bind(&RHCommon::onPost, &m_RHCommon,
                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))) << POST_FAIL_MSG;
    CommonUtil::waitInSecond(5);

    EXPECT_EQ(OC_STACK_OK , m_RHCommon.m_curResource->get(test, std::bind(&RHCommon::onGet, &m_RHCommon,
                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))) << GET_FAIL_MSG;
    CommonUtil::waitInSecond(10);

    ASSERT_EQ(m_RHCommon.verifyObserveDataFromSensorOne(m_errorMessage,0, 0),true) << m_errorMessage;
}
#endif

/**
 * @since 			2015-04-15
 * @see 			none
 * @objective 		test if resource host can handle get request to multiple resources
 * @target 			post(OCRepresentation, QueryParamsMap, onGet)
 * @test_data 		none
 * @pre_condition 	1. Run two resource
 *                	2. Run resource host
 * @procedure 		1. Call find resource
 *            		2. Check if resources are found
 *            		3. Send post request to both resource
 *            		4. send get request to both resource to verify data
 * @post_condition 	Stop resources & resource host
 * @expected 		Resource parameter should be changed for both resources
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RHHostingTest_stc, SendPostRequestToMultipleResource_SRC_P)
{
    RUN_HOSTABLE_RESOURCE_ONE();
    RUN_HOSTABLE_RESOURCE_TWO();
    RUN_RESOURCE_HOST();
    CommonUtil::waitInSecond(15);

    OCPlatform::findResource("" , OC_WELL_KNOWN_COORDINATING_QUERY , CT_DEFAULT, &m_RHCommon.foundResource);
    CommonUtil::waitInSecond(10);

    OCRepresentation rep;
    rep.setValue("temperature", 0);
    rep.setValue("humidity", 0);

    QueryParamsMap test;
    EXPECT_EQ(OC_STACK_OK , m_RHCommon.m_curResource->post(rep, test, std::bind(&RHCommon::onPost, &m_RHCommon,
                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))) << POST_FAIL_MSG;
    CommonUtil::waitInSecond(5);

    EXPECT_EQ(OC_STACK_OK , m_RHCommon.m_curResource2->post(rep, test, std::bind(&RHCommon::onPost, &m_RHCommon,
                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))) << POST_FAIL_MSG;
    CommonUtil::waitInSecond(5);

    EXPECT_EQ(OC_STACK_OK , m_RHCommon.m_curResource->get(test, std::bind(&RHCommon::onGet, &m_RHCommon,
                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))) << GET_FAIL_MSG;

    EXPECT_EQ(OC_STACK_OK , m_RHCommon.m_curResource2->get(test, std::bind(&RHCommon::onGet, &m_RHCommon,
                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))) << GET_FAIL_MSG;
    CommonUtil::waitInSecond(10);

    EXPECT_EQ(m_RHCommon.verifyObserveDataFromSensorOne(m_errorMessage, 0, 0), true) << m_errorMessage;
    EXPECT_EQ(m_RHCommon.verifyObserveDataFromSensorTwo(m_errorMessage, 0, 0), true) << m_errorMessage;

}
#endif

/**
 * @since 			2015-04-15
 * @see 			none
 * @objective 		test if resource host can handle resource turn off
 * @target 			OCPlatform::findResource()
 * @test_data 		none
 * @pre_condition 	1. Run resource
 *                	2. Run resource host
 * @procedure 		1. Call find resource
 *            		2. Check if resources are found
 *            		3. Stop resource
 *            		4. Call find resource
 * @post_condition 	Stop resource host
 * @expected 		Resource should not be found
 */
#if defined(__LINUX__)
TEST_F(RHHostingTest_stc, ResourceHostHandleResourceTurnOff_DSCC_N)
{
    RUN_HOSTABLE_RESOURCE_ONE();
    RUN_RESOURCE_HOST();
    CommonUtil::waitInSecond(15);

    OCPlatform::findResource("" , OC_WELL_KNOWN_COORDINATING_QUERY , CT_DEFAULT, &m_RHCommon.foundResource);
    CommonUtil::waitInSecond(10);

    ASSERT_EQ(m_RHCommon.verifyTempHumSensorOne(m_errorMessage) , true) << m_errorMessage;

    KILL_HOSTABLE_RESOURCE_ONE();
    m_RHCommon.m_curResource = NULL;

    OCPlatform::findResource("" , OC_WELL_KNOWN_COORDINATING_QUERY , CT_DEFAULT, &m_RHCommon.foundResource);
    CommonUtil::waitInSecond(10);

    ASSERT_EQ (m_RHCommon.verifyTempHumSensorOne(m_errorMessage),  true) << "Resource Host Does not Handle Resource Turn Off";

}
#endif

/**
 * @since 			2015-04-15
 * @see 			none
 * @objective 		test if resource host do not host non hostable resource
 * @target 			OCPlatform::findResource()
 * @test_data 		none
 * @pre_condition 	1. Run non hostable resource
 *                	2. Run resource host
 * @procedure 		1. Call find resource
 *            		2. Check if non hostable resource are found
 * @post_condition 	Stop resource & resource host
 * @expected 		Non hostable resource should not be found
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RHHostingTest_stc, ResourceHostDoNotHostNonHostableResource_SRC_N)
{
    RUN_HOSTABLE_RESOURCE_ONE();
    RUN_NON_HOSTABLE_RESOURCE();
    RUN_RESOURCE_HOST();
    CommonUtil::waitInSecond(15);

    OCPlatform::findResource("" , OC_WELL_KNOWN_COORDINATING_QUERY , CT_DEFAULT, &m_RHCommon.foundResource);
    CommonUtil::waitInSecond(10);

    ASSERT_EQ(m_RHCommon.verifySensor(m_RHCommon.m_curResourceNonHostable, "/a/TempHumSensor3", m_errorMessage) , false) << "Non Hostable resource is hosted";
}
#endif

/**
 * @since 			2015-04-15
 * @see 			none
 * @objective 		test if resource host find new resource
 * @target 			OCPlatform::findResource()
 * @test_data 		none
 * @pre_condition 	1. Run resource
 *                	2. Run resource host
 * @procedure 		1. Call find resource
 *            		2. Check if resource are found
 *            		3. Stop Resource
 *            		4. Run resource Again and Call find resource
 *            		2. Check if new resource are found
 * @post_condition 	stop resource host
 * @expected 		new resource should be found
 */
#if defined(__LINUX__)
TEST_F(RHHostingTest_stc, ResourceHostHostNewResource_SRC_N)
{
    RUN_HOSTABLE_RESOURCE_ONE();
    RUN_RESOURCE_HOST();
    CommonUtil::waitInSecond(15);

    OCPlatform::findResource("" , OC_WELL_KNOWN_COORDINATING_QUERY , CT_DEFAULT, &m_RHCommon.foundResource);
    CommonUtil::waitInSecond(10);

    ASSERT_EQ(m_RHCommon.verifyTempHumSensorOne(m_errorMessage) , true) << m_errorMessage;

    KILL_HOSTABLE_RESOURCE_ONE();
    RUN_HOSTABLE_RESOURCE_ONE();
    CommonUtil::waitInSecond(10);

    OCPlatform::findResource("" , OC_WELL_KNOWN_COORDINATING_QUERY , CT_DEFAULT, &m_RHCommon.foundResource);
    CommonUtil::waitInSecond(10);

    ASSERT_EQ(m_RHCommon.verifyTempHumSensorOne(m_errorMessage) , true) << m_errorMessage;
}
#endif

/**
 * @since 			2015-04-15
 * @see 			none
 * @objective 		test if resource host handle observe request after resource is turn off
 * @target 			observe(ObserveType, QueryParamsMap, onObserve)
 * @test_data 		none
 * @pre_condition 	1. Run resource
 *               	2. Run resource host
 * @procedure 		1. Call find resource
 *           		2. Check if resource are found
 *            		3. Stop resource
 *            		4. Send  observe request to resource
 * @post_condition 	Stop resource host
 * @expected 		No data will come from resource
 */
#if defined(__LINUX__)
TEST_F(RHHostingTest_stc, ResourceHostHandleObserveRequestAfterResourceIsTurnOff_SRC_N)
{
    RUN_HOSTABLE_RESOURCE_ONE();
    RUN_RESOURCE_HOST();
    CommonUtil::waitInSecond(15);

    OCPlatform::findResource("" , OC_WELL_KNOWN_COORDINATING_QUERY , CT_DEFAULT, &m_RHCommon.foundResource);
    CommonUtil::waitInSecond(10);

    ASSERT_EQ(m_RHCommon.verifyTempHumSensorOne(m_errorMessage) , true) << m_errorMessage;

    KILL_HOSTABLE_RESOURCE_ONE();

    QueryParamsMap test;
    ASSERT_EQ (OC_STACK_OK , m_RHCommon.m_curResource->observe(ObserveType::Observe , test , std::bind(&RHCommon::onObserve, &m_RHCommon,
                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4))) << FAIL_RES_OBSERVE;
}
#endif

/**
 * @since 			2015-04-15
 * @see 			none
 * @objective 		test if resource host handle get request after resource is turn off
 * @target 			get(onGet)
 * @test_data 		none
 * @pre_condition 	1. Run resource
 *                	2. Run resource host
 * @procedure 		1. Call find resource
 *            		2. Check if resource are found
 *           	 	3. Stop resource
 *            		4. Send  get request to resource
 * @post_condition 	Stop resource host
 * @expected 		No data will come from resource
 */
#if defined(__LINUX__)
TEST_F(RHHostingTest_stc, ResourceHostHandleGetRequestAfterResourceIsTurnOff_DSCC_N)
{
    RUN_HOSTABLE_RESOURCE_ONE();
    RUN_RESOURCE_HOST();
    CommonUtil::waitInSecond(15);

    OCPlatform::findResource("" , OC_WELL_KNOWN_COORDINATING_QUERY , CT_DEFAULT, &m_RHCommon.foundResource);
    CommonUtil::waitInSecond(10);

    ASSERT_EQ(m_RHCommon.verifyTempHumSensorOne(m_errorMessage) , true) << m_errorMessage;

    KILL_HOSTABLE_RESOURCE_ONE();

    QueryParamsMap test;
    ASSERT_EQ(OC_STACK_OK , m_RHCommon.m_curResource->get(test, std::bind(&RHCommon::onGet, &m_RHCommon,
                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))) << FAIL_RES_GET;
}
#endif

/**
 * @since 			2015-04-15
 * @see 			none
 * @objective 		test if resource host handle post request after resource is turn off
 * @target 			post(OCRepresentation, QueryParamsMap, onPost)
 * @test_data 		none
 * @pre_condition 	1. Run resource
 *                	2. Run resource host
 * @procedure 		1. Call find resource
 *            		2. Check if resource are found
 *            		3. Stop resource
 *           		4. Send post request to resource
 * @post_condition 	Stop resource host
 * @expected 		Post request will be handled successfully without any crash
 */
#if defined(__LINUX__)
TEST_F(RHHostingTest_stc, ResourceHostHandlePostRequestAfterResourceIsTurnOff_DSCC_N)
{
    RUN_HOSTABLE_RESOURCE_ONE();
    RUN_RESOURCE_HOST();
    CommonUtil::waitInSecond(15);

    OCPlatform::findResource("" , OC_WELL_KNOWN_COORDINATING_QUERY , CT_DEFAULT, &m_RHCommon.foundResource);
    CommonUtil::waitInSecond(10);

    ASSERT_EQ(m_RHCommon.verifyTempHumSensorOne(m_errorMessage) , true) << m_errorMessage;

    KILL_HOSTABLE_RESOURCE_ONE();

    OCRepresentation rep;
    rep.setValue("temperature", 25);
    rep.setValue("humidity", 10);
    QueryParamsMap test;
    ASSERT_EQ(OC_STACK_OK , m_RHCommon.m_curResource->post(rep, test, std::bind(&RHCommon::onPost, &m_RHCommon,
                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))) << FAIL_RES_POST;
}
#endif
