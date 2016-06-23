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

#include "PHYHelper.h"

#define SSID  "EasySetup123"
#define PASSWORD  "EasySetup123"

#define WRONG_SSID  "negative"
#define WRONG_PASSWORD  "negative"

#define EMPTY_SSID  ""
#define EMPTY_PASSWORD  ""

#define NULL_SSID  "NULL"
#define NULL_PASSWORD  "NULL"

static bool isSecured=false;


void eventCallbackInApp(ESResult esResult, EnrolleeState enrolleeState)
{
    Serial.println("callback!!! in app");
}

//The setup function is called once at startup of the sketch
void setup()
{

    Serial.begin(115200);

    Serial.println("==========================");
    Serial.println("TestSuite: PhyEasySetupTest");
    Serial.println("==========================");

    Serial.println("------------------------------------BEGIN------------------------------------");

    PHYHelper::addFilter();
    PHYHelper phyHelper;
    if (OCProcess() != OC_STACK_OK)
	{
		return;
	}

}

// The loop function is called in an endless loop
void loop()
{
    Test::run();
}

/**
 * @since 2015-09-18
 * @see None
 * @objective Test ESInitEnrollee api
 * @target ESInitEnrollee
 * @test_data networkType SSID PASSWORD isSecured eventCallback
 * @pre_condition Run Android Sample App and Press Start Setup Button
 * @procedure Call ESInitEnrollee
 * @post_condition none
 * @expected It will return success
 */
test(InitializePhy_P)
{
	ESResult expectedResult=ES_OK;
	assertEqual(phyHelper.ESInitializeEnrollee(CT_ADAPTER_IP, SSID, PASSWORD, isSecured, expectedResult), true, "initialize EasySetup"); 
}

/**
 * @since 2015-09-18
 * @see none
 * @objective Test ESInitEnrollee api with wrong SSID and PASSWORD
 * @target ESInitEnrollee
 * @test_data networkType Wrong_SSID Wrong_PASSWORD isSecured eventCallback
 * @pre_condition Run Android Sample App and Press Start Setup Button
 * @procedure Call ESInitEnrollee negatively with wrong parameters
 * @post_condition none
 * @expected It will not return success
 */
test(InitializePhy_N)
{	
	ESResult expectedResult=ES_ERROR;
	assertEqual(phyHelper.ESInitializeEnrollee(CT_ADAPTER_IP, WRONG_SSID, WRONG_PASSWORD, isSecured, expectedResult), true, "initialize EasySetup");
}

/**
 * @since 2015-09-18
 * @see none
 * @objective Test ESInitEnrollee api with empty string for SSID
 * @target ESInitEnrollee
 * @test_data networkType Empty_SSID PASSWORD isSecured eventCallback
 * @pre_condition Run Android Sample App and Press Start Setup Button
 * @procedure Call ESInitEnrollee negatively with empty string as ssid
 * @post_condition none
 * @expected It will return ES_ERROR
 * covered-ESV
 */
test(InitializePhySsid_ESV_N)
{
	ESResult expectedResult=ES_ERROR;
	assertEqual(phyHelper.ESInitializeEnrollee(CT_ADAPTER_IP, EMPTY_SSID, PASSWORD, isSecured, expectedResult), true, "initialize EasySetup");
}

/**
 * @since 2015-09-18
 * @see none
 * @objective Test ESInitEnrollee api with empty string for Password
 * @target ESInitEnrollee
 * @test_data networkType SSID Empty_PASSWORD isSecured eventCallback
 * @pre_condition Run Android Sample App and Press Start Setup Button
 * @procedure Call ESInitEnrollee negatively with empty string as password
 * @post_condition none
 * @expected It will return ES_ERROR
 * covered-ESV
 */
test(InitializePhyPassword_ESV_N)
{
	ESResult expectedResult=ES_ERROR;
	assertEqual(phyHelper.ESInitializeEnrollee(CT_ADAPTER_IP, SSID, EMPTY_PASSWORD, isSecured, expectedResult), true, "initialize EasySetup");
}

/**
 * @since 2015-09-18
 * @see none
 * @objective Test ESInitEnrollee api with null value for SSID
 * @target ESInitEnrollee
 * @test_data networkType NULL_SSID PASSWORD isSecured eventCallback
 * @pre_condition Run Android Sample App and Press Start Setup Button
 * @procedure Call ESInitEnrollee negatively with null value as ssid
 * @post_condition none
 * @expected It will return ES_ERROR
 * covered-NV
 */
test(InitializePhySsid_NV_N)
{
	ESResult expectedResult=ES_ERROR;
	assertEqual(phyHelper.ESInitializeEnrollee(CT_ADAPTER_IP, NULL_SSID, PASSWORD, isSecured, expectedResult), true, "initialize EasySetup");
}

/**
 * @since 2015-09-18
 * @see none
 * @objective Test ESInitEnrollee api with null value for Password
 * @target ESInitEnrollee
 * @test_data networkType SSID NULL_PASSWORD isSecured eventCallback
 * @pre_condition Run Android Sample App and Press Start Setup Button
 * @procedure Call ESInitEnrollee negatively with null value as password
 * @post_condition none
 * @expected It will return ES_ERROR
 * covered-NV
 */
test(InitializePhyPassword_NV_N)
{
	ESResult expectedResult=ES_ERROR;
	assertEqual(phyHelper.ESInitializeEnrollee(CT_ADAPTER_IP, SSID, NULL_PASSWORD, isSecured, expectedResult), true, "initialize EasySetup");
}

/**
 * @since 2015-09-18
 * @see none
 * @objective Test ESInitEnrollee api with wrong SSID
 * @target ESInitEnrollee
 * @test_data networkType Wrong_SSID PASSWORD isSecured eventCallback
 * @pre_condition Run Android Sample App and Press Start Setup Button
 * @procedure Call ESInitEnrollee with wrong SSID
 * @post_condition none
 * @expected It will not return success
 */
test(InitializePhyWrongSsid_N)
{
	ESResult expectedResult=ES_ERROR;
	assertEqual(phyHelper.ESInitializeEnrollee(CT_ADAPTER_IP, WRONG_SSID, PASSWORD, isSecured, expectedResult), true, "initialize EasySetup");

}

/**
 * @since 2015-09-18
 * @see none
 * @objective Test ESInitEnrollee api with wrong Password
 * @target ESInitEnrollee
 * @test_data networkType SSID Wrong_PASSWORD isSecured eventCallback
 * @pre_condition Run Android Sample App and Press Start Setup Button
 * @procedure Call ESInitEnrollee with wrong Password
 * @post_condition none
 * @expected It will not return success
 */
test(InitializePhyWrongPassword_N)
{
	ESResult expectedResult=ES_ERROR;
	assertEqual(phyHelper.ESInitializeEnrollee(CT_ADAPTER_IP, SSID, WRONG_PASSWORD, isSecured, expectedResult), true, "initialize EasySetup");

}

/**
 * @since 2015-09-18
 * @see none
 * @objective Test ESInitProvisioning api
 * @target ESInitProvisioning
 * @test_data none
 * @pre_condition 1.Run Android Sample App and Press Start Setup Button
 *                2. Call ESInitEnrollee
 * @procedure Call ESInitProvisioning
 * @post_condition none
 * @expected It will return success
 */
test(InitProvision_P)
{
	phyHelper.ESInitializeEnrollee(CT_ADAPTER_IP, SSID, PASSWORD, isSecured, expectedResult);	

	expectedResult=ES_RESOURCECREATED;
    assertEqual(phyHelper.ESInititializeProvisioning(expectedResult), true, "initialize Provisioning");
}



/**
 * @since 2015-09-18
 * @see none
 * @objective Test ESTerminateEnrollee api positively
 * @target ESTerminateEnrollee
 * @test_data none
 * @pre_condition 1.Run Android Sample App and Press Start Setup Button
 *                2. Call ESInitEnrollee
 * @procedure Call ESTerminateEnrollee
 * @post_condition Test
 * @expected It will return success
 */
test(Terminate_P)
{
	ESResult expectedResult=ES_OK;
	phyHelper.ESInitializeEnrollee(CT_ADAPTER_IP, SSID, PASSWORD, isSecured, expectedResult);
    assertEqual(phyHelper.ESTerminate(expectedResult), true, "terminate EasySetup");
}
