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
#define INIT 0
#define MAX 6

PHYHelper::PHYHelper()
{

}

void PHYHelper::addFilter()
{
    Test::exclude("*");

    char buffer[MAX_BUF_LEN] = {0};
    size_t len;

    Serial.println("Input filter (provide all to include all)");

    getData(buffer, sizeof(buffer), &len);

    Serial.flush();

    Test::include(buffer);

    Serial.print("Included Filter: ");

    Serial.println(buffer);
}

void PHYHelper::getData(char *readInput, size_t bufferLength, size_t *dataLength)
{
    if (!readInput || bufferLength == 0 || !dataLength)
    {
        Serial.println("Invalid buffer");
        return;
    }

    while (!Serial.available())
    {
        delay(500);
    }
    int len = 0;
    while (Serial.available())
    {
        delay(100);
        char c = Serial.read();
        if ('\n' != c && '\r' != c && len < bufferLength - 1)
        {
            readInput[len++] = c;
        }
        else
        {
            break;
        }
    }

    readInput[len] = '\0';
    Serial.flush();
    Serial.print("PD:");
    Serial.println(readInput);
    (*dataLength) = len;
}

static void PHYHelper::eventCallbackInApp(ESResult esResult, EnrolleeState enrolleeState)
{
	Serial.println("callback!!! in app");
	Serial.print("The value of g_OnBoardingSucceded is: ");
	Serial.print(g_OnBoardingSucceeded);
	Serial.println("");

	if(esResult == ES_OK)
	{
		if(!g_OnBoardingSucceeded){
			Serial.println("Device is successfully OnBoarded");
			g_OnBoardingSucceeded = true;
		}
		else if(g_OnBoardingSucceeded & enrolleeState == ES_ON_BOARDED_STATE){
			Serial.println("Device is successfully OnBoared with SoftAP");
			g_ProvisioningSucceeded = true;
		}
	}
	else if (esResult == ES_ERROR)
	{
		if(g_OnBoardingSucceeded)
		{
			Serial.println("Failure in Provisioning");
			g_OnBoardingSucceeded = false;
		}
		else if(g_ProvisioningSucceeded)
		{
			Serial.println("Failure in connect to target network.");
			//IOTIVITYTEST_LOG(DEBUG,"Failure in connect to target network. \
										Current Enrollee State: %d",enrolleeState);
			g_ProvisioningSucceeded = false;
		}
	}
	Serial.print("The value of g_OnBoardingSucceded is: ");
	Serial.print(g_OnBoardingSucceeded);
	Serial.println("");
}

bool PHYHelper::checkIfCallbackCalled()
{
	for(int i=INIT; i<MAX; i++)
	{		
		if(g_OnBoardingSucceeded == false)
		{
			return false;
		}
		delay(500);
	}
	

	return true;
}

bool PHYHelper::ESInitializeEnrollee(OCConnectivityType networkType, const char *ssid,
        const char *passwd,bool isSecured,ESResult expectedResult)
{
	ESResult actualResult = ESInitEnrollee(networkType,ssid,passwd,isSecured,eventCallbackInApp);

	if(actualResult != expectedResult)
	{
		Serial.println("ESInitEnrollee didnt return expected value");
		return false;
	}

    return true;
}

bool PHYHelper::ESInititializeProvisioning(ESResult expectedResult)
{
	ESResult actualResult = ESInitProvisioning();

    if(actualResult != expectedResult)
    {
    	Serial.println("ESInitProvisioning didnt return expected value");
        return false;
    }
    return true;
}

bool PHYHelper::ESTerminate(ESResult expectedResult)
{
	ESResult actualResult = ESTerminateEnrollee();

    if(actualResult != expectedResult)
    {
    	Serial.println("ESTerminateEnrollee didnt return expected value");
        return false;
    }
    return true;
}
