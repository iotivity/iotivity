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
#include "OCResource.h"

using namespace std;
using namespace OC;

// Create A Function That Matches...
// std::function<void(OCResource::Ptr)> callback

void InProcClientResourceCallback(OCResource::Ptr pValue);
void OutProcClientResourceCallback(OCResource::Ptr pValue);

void Test_OCFunction_0(int functionIndex);
void Test_OCFunction_1(int functionIndex);
void Test_OCFunction_2(int functionIndex);
void Test_OCFunction_3(int functionIndex);
void Test_OCFunction_4(int functionIndex);
void Test_OCFunction_5(int functionIndex);
void Test_OCFunction_6(int functionIndex);
void Test_OCFunction_7(int functionIndex);
void Test_OCFunction_8(int functionIndex);
void Test_OCFunction_9(int functionIndex);
void Test_OCFunction_10(int functionIndex);
void Test_OCFunction_11(int functionIndex);
void Test_OCFunction_12(int functionIndex);
void Test_OCFunction_13(int functionIndex);
void Test_OCFunction_14(int functionIndex);
void Test_OCFunction_15(int functionIndex);
void Test_OCFunction_16(int functionIndex);
void Test_OCFunction_17(int functionIndex);

FunctionStruct              functionStruct[] =  {
                                                    { &Test_OCFunction_0, "InProcClient", "DNRT" },
                                                    { &Test_OCFunction_1, "InProcClient - ListenForResource", "DNRT" },
                                                    { &Test_OCFunction_2, "InProcClient - Callback Called", "DNRT" },
                                                    { &Test_OCFunction_3, "OCOutOfProcClient", "DNRT" },
                                                    { &Test_OCFunction_4, "OCOutOfProcClient - ListenForResource", "DNRT" },
                                                    { &Test_OCFunction_5, "OCOutOfProcClient - Callback Called", "DNRT" },
                                                    { &Test_OCFunction_6, "~InProcClientWrapper", "DNRT" },
                                                    { &Test_OCFunction_7, "~OCOutOfProcClient", "DNRT" },
                                                    { &Test_OCFunction_8, "OCResource", "DNRT" },
                                                    { &Test_OCFunction_9, "OCResource - host", "DNRT" },
                                                    { &Test_OCFunction_10, "OCResource - uri", "DNRT" },
                                                    { &Test_OCFunction_11, "OCResource - isObservable", "DNRT" },
                                                    { &Test_OCFunction_12, "OCResource - isCollection", "DNRT" },
                                                    { &Test_OCFunction_13, "OCResource - resourceTypes", "DNRT" },
                                                    { &Test_OCFunction_14, "OCResource - interfaces", "DNRT" },
                                                    { &Test_OCFunction_15, "OCResource - properties", "DNRT" },
                                                    { &Test_OCFunction_16, "OCResource - children", "DNRT" },
                                                    { &Test_OCFunction_17, "~OCResource", "DNRT" },
                                                };

InProcClientWrapper     *pInClient = nullptr;
OutOfProcClientWrapper  *pOutClient = nullptr;
OCResource              *pResource = nullptr;

int main(void)
{
    int                     functionIndex = 0, functionNameSize = 0, functionNameLen = 0, functionCount = 0;
    int                     resultSize = 0, resultLen = 0, lenOfResult = 0;
    char                    rightSide[MaxResultSize], a;
    

    // Get The Size Of Each Of The Structure Elements
    functionCount = ElementCount(functionStruct);    
    
    // Run The Tests    
    Test_OCFunction_0(0);
    Test_OCFunction_1(1);
    Test_OCFunction_3(3);
    Test_OCFunction_4(4);
    Test_OCFunction_5(5);
    Test_OCFunction_6(6);
    Test_OCFunction_7(7);
    Test_OCFunction_8(8);
    Test_OCFunction_9(9);
    Test_OCFunction_10(10);
    Test_OCFunction_11(11);
    Test_OCFunction_12(12);
    Test_OCFunction_13(13);
    Test_OCFunction_14(14);
    Test_OCFunction_15(15);
    Test_OCFunction_16(16);
    Test_OCFunction_17(17);    
    
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

void Test_OCFunction_0(int functionIndex)
{
    bool                    bResult = true;
    PlatformConfig          cfg;
    property_binding_vector vc;
    
    try
    {
        cfg.serviceType = ServiceType::InProc;
        cfg.mode = ModeType::Server;
        cfg.ipAddress = "192.168.1.5";
        cfg.port = 8080;

	pInClient = new InProcClientWrapper(cfg); 
       
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

void Test_OCFunction_1(int functionIndex)
{
    bool                    bResult = false;    
    string		    resName, resType; 
    int                     retVal = 0;
    
    try
    {
        if (pInClient != nullptr)
        {
            resName = "ResourceName";
            resType = "ResourceType";

	    retVal = pInClient->ListenForResource(resName, resType, &InProcClientResourceCallback);

            bResult = true;
        }        
    }

    catch (...)
    {
	bResult = false;
    } 

    if (bResult && retVal == 0)
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Pass");
    }
    else
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Fail");
    }
}

void InProcClientResourceCallback(OCResource::Ptr pValue)
{
    Test_OCFunction_2(2);
}

void Test_OCFunction_2(int functionIndex)
{
    bool                    bResult = false;    
    
    try
    {
        
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

void Test_OCFunction_3(int functionIndex)
{
    bool                    bResult = true;
    PlatformConfig          cfg;
    property_binding_vector vc;
    
    try
    {
        cfg.serviceType = ServiceType::InProc;
        cfg.mode = ModeType::Server;
        cfg.ipAddress = "192.168.1.5";
        cfg.port = 8080;

	pOutClient = new OutOfProcClientWrapper(cfg); 
       
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

void Test_OCFunction_4(int functionIndex)
{
    bool                    bResult = false;    
    string		    resName, resType; 
    int                     retVal = 0;
    
    try
    {
        if (pOutClient != nullptr)
        {
            resName = "ResourceName";
            resType = "ResourceType";

	    retVal = pOutClient->ListenForResource(resName, resType, &InProcClientResourceCallback);

            bResult = true;
        }        
    }

    catch (...)
    {
	bResult = false;
    } 

    if (bResult && retVal == 0)
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Pass");
    }
    else
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Fail");
    }
}

void OutProcClientResourceCallback(OCResource::Ptr pValue)
{
    Test_OCFunction_5(5);
}

void Test_OCFunction_5(int functionIndex)
{
    bool                    bResult = true;    
    
    try
    {
        
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

void Test_OCFunction_6(int functionIndex)
{
    bool                    bResult = false;    
    
    try
    {
        if (pInClient != NULL)
        {
            delete pInClient;
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

void Test_OCFunction_7(int functionIndex)
{
    bool                    bResult = true;    
    
    try
    {
        
	if (pOutClient != NULL)
        {
            delete pOutClient;
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

void Test_OCFunction_8(int functionIndex)
{
    bool                    bResult = true;
    string                  str, exceptionStr;
    boost::property_tree::ptree tree;

    try
    {
        str = "Host";
	pResource = new OCResource(str, tree); 
       
        bResult = true;
    }

    catch (ResourceInitException rie)
    {
        exceptionStr =rie.Reason()
        cout << exceptionStr.c_str() << endl;
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

void Test_OCFunction_9(int functionIndex)
{
    string                  str;

    if (pResource != nullptr)
        str = pResource->host();       
    

    if (strcmp(str.c_str(), "Host") == 0)
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Pass");
    }
    else
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Fail");
    } 
}

void Test_OCFunction_10(int functionIndex)
{
    string                  str;

    if (pResource != nullptr)
        str = pResource->uri();       
    

    if (strcmp(str.c_str(), "Host") == 0)
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Pass");
    }
    else
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Fail");
    }
}

void Test_OCFunction_11(int functionIndex)
{
    bool                    bResult = false;

    if (pResource != nullptr)
        bResult = pResource->isObservable();       
    

    if (bResult)
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Pass");
    }
    else
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Fail");
    }
}

void Test_OCFunction_12(int functionIndex)
{
    bool                    bResult = false;

    if (pResource != nullptr)
        bResult = pResource->isCollection();       
    

    if (bResult)
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Pass");
    }
    else
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Fail");
    }
}

void Test_OCFunction_13(int functionIndex)
{
    std::vector<std::string> vStr;

    if (pResource != nullptr)
        vStr = pResource->resourceTypes();       
    

    if (vStr.size() > 0)
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Pass");
    }
    else
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Fail");
    }
}

void Test_OCFunction_14(int functionIndex)
{
    std::vector<std::string> vStr;

    if (pResource != nullptr)
        vStr = pResource->interfaces();       
    

    if (vStr.size() > 0)
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Pass");
    }
    else
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Fail");
    }
}

void Test_OCFunction_15(int functionIndex)
{
    std::vector<std::string> vStr;

    if (pResource != nullptr)
        vStr = pResource->properties();       
    

    if (vStr.size() > 0)
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Pass");
    }
    else
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Fail");
    }
}

void Test_OCFunction_16(int functionIndex)
{
    std::vector<std::string> vStr;

    if (pResource != nullptr)
        vStr = pResource->children();       
    

    if (vStr.size() > 0)
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Pass");
    }
    else
    {
	strcpy((char*)&functionStruct[functionIndex].m_Result, "Fail");
    }
}

void Test_OCFunction_17(int functionIndex)
{
    bool                    bResult = false; 

    try
    {
        if (pResource != nullptr)
        {
            delete pResource;
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
