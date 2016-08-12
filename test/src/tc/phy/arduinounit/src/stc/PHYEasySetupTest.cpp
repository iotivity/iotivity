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

#define INIT 0
#define MAX 2

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

    if (OCProcess() != OC_STACK_OK)
	{
		return;
	}

    PHYHelper::addFilter();

}

// The loop function is called in an endless loop
void loop()
{
    Test::run();
}

/**
 * @since 2015-09-18
 * @see none
 * @objective Test ESInitEnrollee api and check if callback is called
 * @target ESInitEnrollee
 * @test_data networkType SSID PASSWORD isSecured eventCallback
 * @pre_condition Run Android Sample App and Press Start Setup Button
 * @procedure 1. Call ESInitEnrollee
 *            2. Check callback 
 * @post_condition none
 * @expected It will return ES_OK and callback will be called
 * covered-CV
 */
test(InitializePhy_CV_P)
{
	
	ESResult expectedResult=ES_OK;
	assertEqual(phyHelper.ESInitializeEnrollee(CT_ADAPTER_IP, SSID, PASSWORD, isSecured, expectedResult), true, "initialize EasySetup");
	assertEqual(phyHelper.checkIfCallbackCalled(), true, "Callback is not called");
}

/**
 * @since 2015-09-18
 * @see none
 * @objective Test ESInitEnrollee api with an invalid callback
 * @target ESInitEnrollee
 * @test_data networkType SSID PASSWORD isSecured eventCallback
 * @pre_condition Run Android Sample App and Press Start Setup Button
 * @procedure Call ESInitEnrollee negatively with invalid callback
 * @post_condition none
 * @expected It will return ES_ERROR
 * covered-ICCC
 */
test(InitializePhy_ICCC_N)
{	
    ESEnrolleeEventCallback invalidCallback;
	ESResult expectedResult=ES_ERROR;
	
	assertEqual(ESInitEnrollee(CT_ADAPTER_IP, SSID, PASSWORD, isSecured, invalidCallback), expectedResult, "should not return ES_OK with invalid callback");
}

/**
 * @since 2015-09-18
 * @see none
 * @objective Test ESInitEnrollee api without starting Android Sample App
 * @target ESInitEnrollee
 * @test_data networkType SSID PASSWORD isSecured eventCallback
 * @pre_condition none
 * @procedure Call ESInitEnrollee without starting mediator app
 * @post_condition none
 * @expected It will not return success
 */
test(InitializePhyWithoutMediator_N)
{
	ESResult expectedResult=ES_ERROR;
	assertEqual(phyHelper.ESInitializeEnrollee(CT_ADAPTER_IP, SSID, PASSWORD, isSecured, expectedResult), true, "initialize EasySetup");

}

/**
 * @since 2015-09-18
 * @see none
 * @objective Test ESInitProvisioning api without initializing
 * @target ESInitProvisioning
 * @test_data none
 * @pre_condition Run Android Sample App and Press Start Setup Button
 * @procedure Call ESInitProvisioning without initializing
 * @post_condition none
 * @expected It will not return success
 */
test(InitProvisionWithoutInitialize_N)
{
	ESResult expectedResult=ES_ERROR;
	assertEqual(phyHelper.ESInititializeProvisioning(expectedResult), true, "initialize Provisioning");

}

/**
 * @since 2015-09-18
 * @see none
 * @objective Test ESTerminateEnrollee api without initializing
 * @target ESTerminateEnrollee
 * @test_data none
 * @pre_condition Run Android Sample App and Press Start Setup Button
 * @procedure Call ESTerminateEnrollee
 * @post_condition none
 * @expected It will not return success
 */
test(TerminateWithoutInitialize_N)
{
	ESResult expectedResult=ES_ERROR;
	assertEqual(phyHelper.ESTerminate(expectedResult), true, "Terminate EasySetup");
}

/**
 * @since 2015-09-18
 * @see void
 * @objective Test ESTerminateEnrollee api after provisioning
 * @target ESTerminateEnrollee
 * @test_data networkType SSID PASSWORD isSecured eventCallback
 * @pre_condition 1.Run Android Sample App and Press Start Setup Button
 * 2. Call ESInitEnrollee
 * 3. Call ESInitProvisioning
 * @procedure Call ESTerminateEnrollee
 * @post_condition none
 * @expected It will return success
 */
test(TerminateAfterInitAndProvision_P)
{
	ESResult expectedResult=ES_OK;
	assertEqual(phyHelper.ESInitializeEnrollee(CT_ADAPTER_IP, SSID, PASSWORD, isSecured, expectedResult), true, "initialize EasySetup");

	expectedResult=ES_RESOURCECREATED;
	assertEqual(phyHelper.ESInititializeProvisioning(expectedResult), true, "initialize Provisioning");

	expectedResult=ES_OK;
    assertEqual(phyHelper.ESTerminate(expectedResult), true, "Terminate EasySetup");
}
