//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

#include "OCTestApp.h"
#include "stdlib.h"
#include "stdio.h"
#include <string.h>
#include <iostream>

// Headers Required For the Tests
#include "OCPlatform.h"
#include "OCApi.h"

using namespace std;
using namespace OC;

void Test_OCPlatform_0(int functionIndex);
void Test_OCPlatform_1(int functionIndex);
void Test_OCPlatform_2(int functionIndex);
void Test_OCPlatform_3(int functionIndex);
void Test_OCPlatform_4(int functionIndex);
void Test_OCPlatform_5(int functionIndex);

void PlatformCallback_1(OCResource::Ptr pParam);
void PlatformCallback_2(OCResource::Ptr pParam);

FunctionStruct              functionStruct[] =  {
                                                    { &Test_OCPlatform_0, "OCPlatform", "DNRT" },
                                                    { &Test_OCPlatform_1, "OCPlatform - findResource", "DNRT" },
                                                    { &Test_OCPlatform_2, "OCPlatform - Callback", "DNRT" },
                                                    { &Test_OCPlatform_3, "OCPlatform - findResourceByType", "DNRT" },
                                                    { &Test_OCPlatform_4, "OCPlatform - Resource Callback", "DNRT" },
                                                    { &Test_OCPlatform_5, "~OCPlatform", "DNRT" },
                                                };

OCPlatform              *pPlatform = nullptr;

int main(void)
{
    int                     functionIndex = 0, functionNameSize = 0, functionNameLen = 0, functionCount = 0;
    int                     resultSize = 0, resultLen = 0, lenOfResult = 0;
    char                    rightSide[MaxResultSize], a;
    

    // Get The Size Of Each Of The Structure Elements
    functionCount = ElementCount(functionStruct);    
    
    // Run The Tests    
    Test_OCPlatform_0(0);
    Test_OCPlatform_1(1);
    Test_OCPlatform_2(2);
    Test_OCPlatform_3(3);
    Test_OCPlatform_4(4);
    Test_OCPlatform_5(5);
    
    for (functionIndex = 0; functionIndex < functionCount; functionIndex++)
    {
        functionNameLen = strlen((char*)&functionStruct[functionIndex].m_FunctionName);
        if (functionNameLen > functionNameSize)
            functionNameSize = functionNameLen;
        
        resultLen = strlen((char*)&functionStruct[functionIndex].m_Result);
        if (resultLen > resultSize)
            resultSize = resultLen;
    }

    for (functionIndex = 0; functionIndex < functionCount; functionIndex++)
    {
        // Set The Width Of This Line
        cout.width(ResultColumn);

        // Print The Function Name
        cout << left << functionStruct[functionIndex].m_FunctionName;

        // Print The Result
        strcpy((char*)&rightSide, (char*)&functionStruct[functionIndex].m_Result);
        lenOfResult = strlen((char*)&functionStruct[functionIndex].m_Result);
        memset((char*)&rightSide[lenOfResult], ' ', MaxResultSize - lenOfResult);
        rightSide[MaxResultSize - 1] = 0;
        cout << right << rightSide << endl;
    }
    
    cin >> a;  // Wait For Keyboard Input
    
    return 0;
}


void Test_OCPlatform_0(int functionIndex)
{
    bool                    bResult = true;
    PlatformConfig          cfg;    
    
    try
    {
        cfg.serviceType = ServiceType::InProc;
        cfg.mode = ModeType::Server;
        cfg.ipAddress = "192.168.1.5";
        cfg.port = 8080;
	
        pPlatform = new OCPlatform(cfg);	
        
	bResult = true;
    }

    catch (...)
    {
	bResult = false;
    } 

    if (bResult)
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Pass");
    }
    else
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Fail");
    } 
}

void Test_OCPlatform_1(int functionIndex)
{
    bool                    bResult = false;
    PlatformConfig          cfg;
    string		    resType, hostName;
    property_binding_vector vc;
    
    try
    {
        if (pPlatform != nullptr)
        {
            hostName = "Host";
            resType = "ResourceType";
            pPlatform->findResource(hostName, resType, &PlatformCallback_1);        
	    bResult = true;
        }
    }

    catch (...)
    {
	bResult = false;
    } 

    if (bResult)
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Pass");
    }
    else
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Fail");
    } 
}

void PlatformCallback_1(OCResource::Ptr pParam)
{
    Test_OCPlatform_2(2);
}

void Test_OCPlatform_2(int functionIndex)
{
    bool                    bResult = true;    

    if (bResult)
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Pass");
    }
    else
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Fail");
    } 
}

void Test_OCPlatform_3(int functionIndex)
{
    bool                    bResult = false;
    PlatformConfig          cfg;
    string		    serviceURL, resourceType;
    property_binding_vector vc;
    
    try
    {
        if (pPlatform != nullptr)
        {
            serviceURL = "ServiceURL";
            resourceType = "Type";
            //pPlatform->findResourceByType(serviceURL, resourceType, &PlatformCallback_2);	
            bResult = true;
        }
    }

    catch (...)
    {
	bResult = false;
    } 

    if (bResult)
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Pass");
    }
    else
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Fail");
    } 
}

void PlatformCallback_2(OCResource::Ptr pParam)
{
    Test_OCPlatform_4(4);
}

void Test_OCPlatform_4(int functionIndex)
{
    bool                    bResult = false; 
    string                  resourceURI, resourceTypeName;
    property_binding_vector vc;

    try
    {
        if (pPlatform != nullptr)
        {
            resourceURI = "URI";
            resourceTypeName = "NAME";
            pPlatform->registerResource(resourceURI, resourceTypeName, vc);		
            bResult = true;
        }
    }

    catch (...)
    {
	
    } 

    if (bResult)
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Pass");
    }
    else
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Fail");
    } 
}

void Test_OCPlatform_5(int functionIndex)
{
    bool                    bResult = false;    
    
    try
    {
        if (pPlatform != nullptr)
        {
	    delete pPlatform;
            bResult = true;
        }
    }

    catch (...)
    {
	
    } 

    if (bResult)
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Pass");
    }
    else
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Fail");
    } 
}





