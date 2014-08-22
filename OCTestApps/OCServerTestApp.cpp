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
#include "OCReflect.h"


using namespace std;
using namespace OC;
using namespace OC::OCReflect;

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

FunctionStruct              functionStruct[] =  {
                                                    { &Test_OCFunction_0, "OCInProcServer", "DNRT" },
                                                    { &Test_OCFunction_1, "OCOutOfProcServer", "DNRT" },
                                                    { &Test_OCFunction_2, "OCInProcServer - registerResource", "DNRT" },
                                                    { &Test_OCFunction_3, "OCOutOfProcServer - registerResource", "DNRT" },
                                                    { &Test_OCFunction_4, "remote_resource", "DNRT" },
                                                    { &Test_OCFunction_5, "remoteResource - operator", "DNRT" },
                                                    { &Test_OCFunction_6, "method", "DNRT" },
                                                    { &Test_OCFunction_7, "method - operator", "DNRT" },
                                                    { &Test_OCFunction_8, "~OCInProcServer", "DNRT" },
                                                    { &Test_OCFunction_9, "~OCOutOfProcServer", "DNRT" },
                                                    { &Test_OCFunction_10, "~remote_resource", "DNRT" },
                                                    { &Test_OCFunction_11, "~OCResource", "DNRT" },
                                                    { &Test_OCFunction_12, "~method", "DNRT" },                                                    
                                                };

InProcServerWrapper        *pInProcServer = nullptr;
OutOfProcServerWrapper     *pOutProcServer = nullptr;
remote_resource            *pRemoteResource = nullptr;
OCResource                 *pResource = nullptr;
method                     *pMethod = nullptr;

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
    Test_OCFunction_2(2);
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
    
    try
    {
        cfg.serviceType = ServiceType::InProc;
        cfg.mode = ModeType::Server;
        cfg.ipAddress = "192.168.1.5";
        cfg.port = 8080;
	
        pInProcServer = new InProcServerWrapper(cfg);	
        
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
    bool                    bResult = true;
    PlatformConfig          cfg;
    
    try
    {
        cfg.serviceType = ServiceType::InProc;
        cfg.mode = ModeType::Server;
        cfg.ipAddress = "192.168.1.5";
        cfg.port = 8080;
	
        pOutProcServer = new OutOfProcServerWrapper(cfg);	
        
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

void Test_OCFunction_2(int functionIndex)
{
    bool                    bResult = true;
    string		    resName, resType;
    property_binding_vector vc;
    
    try
    {
        if (pInProcServer != nullptr)
        {
            resName = "ResourceName";
	    resType = "ResourceType";
	    pInProcServer->registerResource(resName, resType, vc);        
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

void Test_OCFunction_3(int functionIndex)
{
    bool                    bResult = true;
    string	        	    resName, resType;
    property_binding_vector vc;
    
    try
    {
        if (pOutProcServer != nullptr)
        {
            resName = "ResourceName";
	    resType = "ResourceType";
	    pOutProcServer->registerResource(resName, resType, vc);        
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

void Test_OCFunction_4(int functionIndex)
{
    bool                    bResult = false;
    string                  host, loc;
    boost::property_tree::ptree   tree;
    
    try
    {
        host = "Host";
        loc = "Nowhere Special";
        pResource = new OCResource(host, tree);
        pRemoteResource = new remote_resource(*pResource, loc);
        bResult = true;
    }

    catch (...)
    {
cout << "Ouch!" << endl;
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

void Test_OCFunction_5(int functionIndex)
{
    bool                    bResult = false; 
    OC::OCReflect::entity   localEntity; 
    string                  str;
    
    try
    {
        if (pRemoteResource)
        {
            str = "SomeEntity";
            //localEntity = pRemoteResource;//(str);
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

void Test_OCFunction_6(int functionIndex)
{
    bool                    bResult = false;
    string                  str;
    
    try
    {
        if (pResource != nullptr)
        {
            str = "SomeName";
            pMethod = new method(*pResource, str);
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
    bool                    bResult = false;
    OC::OCReflect::tagged_property prop; 
    
    try
    {
        if (pMethod != nullptr)
        {
            //prop = pMethod(TS ...xs);
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
    bool                    bResult = false;    
    
    try
    {
        if (pInProcServer != nullptr)
        {
            delete pInProcServer;
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

void Test_OCFunction_9(int functionIndex)
{
    bool                    bResult = false;    
    
    try
    {
        if (pOutProcServer != nullptr)
        {
            delete pOutProcServer;
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

void Test_OCFunction_10(int functionIndex)
{
    bool                    bResult = false;
     
    try
    {
        if (pRemoteResource != nullptr)
        {
            delete pRemoteResource;
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


void Test_OCFunction_11(int functionIndex)
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

void Test_OCFunction_12(int functionIndex)
{
    bool                    bResult = false;
    
    try
    {
        if (pMethod != nullptr)
        {
            delete pMethod;
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
