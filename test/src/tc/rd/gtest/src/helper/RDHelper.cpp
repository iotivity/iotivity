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

#include "RDHelper.h"

RDHelper* RDHelper::s_rdHelper = NULL;

OCResourceHandle g_curResource_t = NULL;
OCResourceHandle g_curResource_l = NULL;
bool g_flag=false;
char g_rdAddress[MAX_ADDR_STR_SIZE];
int g_quitFlag=0;
std::string rdAddress;
uint16_t g_rdPort=0;
bool g_foundRDResource = false;
OCHeaderOption options[1];
size_t numOptions = 0;;
OCResourceHandle handles[2];
#define TTL_TIMEOUT 30
#define NUM_HANDLES 10
using namespace OC;
#define DEFAULT_CONTEXT_VALUE 0x99
RDHelper::RDHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[RDHelper] IN");
}

RDHelper* RDHelper::getInstance(void)
{
    s_rdHelper = new RDHelper();
    return s_rdHelper;
}

RDHelper::~RDHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[RDHelper] OUT");
    delete s_rdHelper;
}

OCStackResult RDHelper::rdCheckStartServerStatus(bool isWithOCInit)
{
    try
    {
        OCStackResult result = OC_STACK_ERROR;
        if(isWithOCInit)
        {
            result = OCInit(NULL, 0, OC_CLIENT_SERVER);

            if (result != OC_STACK_OK)
            {
                IOTIVITYTEST_LOG(INFO,"OCInit initialization failed ...\n");
                return result;
            }
        }

        result = OCRDStart();
        return result;
    }
    catch (std::exception &ex)
    {
        IOTIVITYTEST_LOG(INFO, "Exception: %s in rdCheckStartServerStatus",ex.what());
        return OC_STACK_ERROR;
    }
}

OCStackResult RDHelper::rdStopServer()
{
    try
    {
        OCStackResult result = OC_STACK_ERROR;
        result = OCRDStop();

        return result;
    }
    catch (std::exception &ex)
    {
        IOTIVITYTEST_LOG(INFO, "Exception: %s in rdStopServer",ex.what());
        return OC_STACK_ERROR;
    }
}

void RDHelper::registerLocalResources()
{
    try
    {
        std::string resourceURI_thermostat = RD_RESOURCE_URL_THERMOSTAT;
        std::string resourceURI_light = RD_RESOURCE_URL_LIGHT;

        OCStackResult result = OCPlatform::registerResource(g_curResource_t,
                               resourceURI_thermostat,
                               RD_RESOURCE_TYPE_THERMOSTAT,
                               DEFAULT_INTERFACE,
                               NULL,
                               OC_DISCOVERABLE);

        if (OC_STACK_OK != result)
        {
            throw std::runtime_error(
                std::string("Device Resource failed to start") + std::to_string(result));
        }

        result = OCPlatform::registerResource(g_curResource_l,
                                              resourceURI_light,
                                              RD_RESOURCE_TYPE_LIGHT,
                                              DEFAULT_INTERFACE,
                                              NULL,
                                              OC_DISCOVERABLE);

        if (OC_STACK_OK != result)
        {
            throw std::runtime_error(
                std::string("Device Resource failed to start") + std::to_string(result));
        }
    }
    catch (std::exception &ex)
    {
        IOTIVITYTEST_LOG(INFO, "Exception: %s in registerLocalResources",ex.what());
    }
}

void RDHelper::unregisterLocalResources()
{
    try
    {
        OCStackResult result = OCPlatform::unregisterResource(g_curResource_t);

        if (OC_STACK_OK != result)
        {
            throw std::runtime_error(
                std::string("Device Resource failed to unregister") + std::to_string(result));
        }

        result = OCPlatform::unregisterResource(g_curResource_l);
        if (OC_STACK_OK != result)
        {
            throw std::runtime_error(
                std::string("Device Resource failed to start") + std::to_string(result));
        }
    }
    catch (std::exception &ex)
    {
        IOTIVITYTEST_LOG(INFO, "Exception: %s in unregisterLocalResources",ex.what());
    }
}

int RDHelper::biasFactorCB(void *ctx, OCDoHandle handle,OCClientResponse *clientResponse)
{
    std::ostringstream oss;
    oss << clientResponse->devAddr.addr << ":" << clientResponse->devAddr.port;
    rdAddress = oss.str();
    std::cout << "RD Address and Port is : "<<rdAddress<< std::endl;
    try
    {
        size_t pos = rdAddress.find(':');
        if (pos!=std::string::npos)
        {
            std::string rd_ipaddr = rdAddress.substr(0,pos);
            std::string rd_port = rdAddress.substr(pos+1);
            strcpy(g_rdAddress,rd_ipaddr.c_str());
            for (int i=0; i<MAX_ADDR_STR_SIZE; i++)
                std::cout << g_rdAddress[i];
            OICStrcpy(g_rdAddress, MAX_ADDR_STR_SIZE, g_rdAddress);
            int temp(std::stoi(rd_port));
            if (temp<=static_cast<int>(UINT16_MAX) && temp>=0)
            {
                temp = static_cast<uint16_t>(temp);
                g_rdPort = temp;
            }
        }
    }
    catch (std::exception &ex)
    {
        IOTIVITYTEST_LOG(INFO,"Invalid Port and Address");
    }
    return 0;
}
OCStackResult RDHelper::rdDiscover(bool isCheckedWithParam,bool isCheckedCallback)
{
    OCStackResult actualResult =  OC_STACK_ERROR;
    OCDoHandle *handle = NULL;
    OCConnectivityType connectivityType = (OCConnectivityType)CT_ADAPTER_IP;

    try
    {
        if(isCheckedWithParam)
        {
            OCCallbackData cbData;
            cbData.cb = &RDHelper::biasFactorCB;
            cbData.cd = NULL;
            cbData.context = (void*) DEFAULT_CONTEXT_VALUE;
            actualResult = OCRDDiscover(handle, CT_ADAPTER_IP, &cbData, OC_LOW_QOS);
        }
        else
        {
            actualResult = OCRDDiscover(NULL,(OCConnectivityType)NULL,NULL,(OCQualityOfService)NULL);
        }

        if(isCheckedCallback)
        {
            for(int i=0;i<RD_SLEEPTIME_MAX;i++)
            {
                if(g_rdPort==0)
                {
                    CommonUtil::waitInSecond(RD_SLEEPTIME_MAX);
                    IOTIVITYTEST_LOG(INFO, "waiting for biasFactorCB... ");

                }
                else
                {
                    IOTIVITYTEST_LOG(INFO, "rdDiscover Callback called");
                    break;
                }
            }

            if (actualResult == OC_STACK_OK)
            {
                if(g_rdPort==0)
                {
                    IOTIVITYTEST_LOG(INFO, "rdDiscover Callback is not invoked.");
                    return OC_STACK_ERROR;
                }
            }
        }

        return actualResult;
    }
    catch (std::exception &ex)
    {
        IOTIVITYTEST_LOG(INFO, "Exception: %s in rdDiscover",ex.what());
        return OC_STACK_ERROR;
    }
}

OCStackResult RDHelper::rdPublish()
{
    try
    {
        OCCallbackData cbData;
        cbData.cb = &RDHelper::biasFactorCB;
        cbData.cd = NULL;
        cbData.context = (void*) DEFAULT_CONTEXT_VALUE;
        OCStackResult actualResult =  OC_STACK_ERROR;
        OCDoHandle *handle = (OCDoHandle *) NULL;
        actualResult = OCRDPublish(handle,rdAddress.c_str(),CT_ADAPTER_IP, &g_curResource_t, NUM_HANDLES, TTL_TIMEOUT, &cbData,(OCQualityOfService)OC::QualityOfService::LowQos);
        return actualResult;
    }
    catch (std::exception &ex)
    {
        IOTIVITYTEST_LOG(INFO, "Exception: %s in OCRDPublish",ex.what());
        return OC_STACK_ERROR;
    }
}

OCStackResult RDHelper::rdPublish(char *addr,uint16_t port,int num)
{
    try
    {
        OCCallbackData cbData;
        cbData.cb = &RDHelper::biasFactorCB;
        cbData.cd = NULL;
        cbData.context = (void*) DEFAULT_CONTEXT_VALUE;
        OCStackResult actualResult;
        OCDoHandle *handle = (OCDoHandle *) NULL;

        if(addr=="default")
        {
            addr=rdAddress.c_str();
        }
        if(port==1)
        {
            port=g_rdPort;
        }

        actualResult = OCRDPublish(handle,addr,(OCConnectivityType)2, &g_curResource_t, num, TTL_TIMEOUT, &cbData,(OCQualityOfService)OC::QualityOfService::LowQos);

        return actualResult;
    }
    catch (std::exception &ex)
    {
        IOTIVITYTEST_LOG(INFO, "Exception: %s in rdPublish",ex.what());
        return OC_STACK_ERROR;
    }
}

void RDHelper::startClient()
{
    PlatformConfig config { OC::ServiceType::InProc, ModeType::Both, "0.0.0.0",0, OC::QualityOfService::LowQos };
    OCPlatform::Configure(config);
    registerLocalResources();
#ifndef __WINDOWS__
    pthread_create(&m_helperThread, NULL, &RDHelper::runThread, this);
#else
    pthread_create(&m_helperThread, NULL, (void *(*)(void *))(&RDHelper::runThread), this);
#endif
}

void RDHelper::joinThread()
{
#ifndef __WINDOWS__
    pthread_join(m_helperThread, NULL);
#endif
}

void RDHelper::runThread(RDHelper *arg)
{

    OCStackResult expectedResult = OC_STACK_OK;

    if(expectedResult!=arg->rdDiscover(RD_DISCOVER_WITH_PARAM, RD_DISCOVER_WITH_CALLBACK))
        IOTIVITYTEST_LOG(DEBUG, "Failed To Discover Resource Directory");

    if(expectedResult!=arg->rdPublish())
        IOTIVITYTEST_LOG(DEBUG, "Failed To Publish Resource Directory");

    g_quitFlag=1;
    IOTIVITYTEST_LOG(DEBUG, "New quitFlag value %d",g_quitFlag);

}
void RDHelper::rdFoundResourceCB(std::shared_ptr< OC::OCResource > resource)
{
    try
    {
        IOTIVITYTEST_LOG(INFO, "Found resource response.rdFoundResource callback is invoked.");
        if (resource)
        {
            if (resource->uri() == RD_RESOURCE_URL_LIGHT)
            {
                IOTIVITYTEST_LOG(INFO, "Light Resource Found at @ URI: %s \tHost Address: %s",resource->uri().c_str(),resource->host().c_str());
                g_foundRDResource = true;
            }

            if (resource->uri() == RD_RESOURCE_URL_THERMOSTAT)
            {
                IOTIVITYTEST_LOG(INFO, "Thermostat Resource Found at @ URI: %s \tHost Address: %s",resource->uri().c_str(),resource->host().c_str());
                g_foundRDResource = true;
            }
        }
        else
        {
            IOTIVITYTEST_LOG(INFO, "Resource is invalid: %s",resource->uri().c_str());
            g_foundRDResource = false;
        }
    }
    catch (std::exception &ex)
    {
        IOTIVITYTEST_LOG(INFO, "Exception: %s in rdFoundResource",ex.what());
    }
    return g_foundRDResource;
}

OCStackResult RDHelper::rdFindResourcesChecked(const std::string& host, const std::string& resourceURI, OCConnectivityType connectivityType)
{
    OCStackResult actualResult =  OC_STACK_ERROR;
    bool sendRequest = true;
    IOTIVITYTEST_LOG(INFO, "Created Platform...");

    while (!g_foundRDResource)
    {
        try
        {
            if (sendRequest)
            {
                sendRequest = false;
                IOTIVITYTEST_LOG(INFO, "Finding Resource : %s .....",resourceURI.c_str());

                actualResult = OCPlatform::findResource(host, resourceURI, connectivityType, &rdFoundResourceCB);
                return actualResult;
            }
        }
        catch (OC::OCException &ex)
        {
            sendRequest = true;
            IOTIVITYTEST_LOG(INFO, "Exception: %s in rdFoundResource",ex.reason().c_str());
        }
    }

    return actualResult;
}

OCStackResult RDHelper::rdBindResourceInsToResource(bool isResourcehandle, int64_t ins)
{
    OCStackResult actualResult =  OC_STACK_ERROR;
    try
    {
        if(isResourcehandle)
            actualResult = OCBindResourceInsToResource(g_curResource_t, ins);
        else
            actualResult = OCBindResourceInsToResource(NULL, ins);
        return actualResult;
    }
    catch (std::exception &ex)
    {
        IOTIVITYTEST_LOG(INFO, "Exception: %s in OCBindResourceInsToResource",ex.what());
        return OC_STACK_ERROR;
    }
}

OCStackResult RDHelper::rdGetResourceIns(bool isResourcehandle,int64_t ins)
{
    OCStackResult actualResult =  OC_STACK_ERROR;
    try
    {
        if(isResourcehandle)
            actualResult = OCGetResourceIns(g_curResource_t, &ins);
        else
            actualResult = OCGetResourceIns(NULL, &ins);
        return actualResult;
    }
    catch (std::exception &ex)
    {
        IOTIVITYTEST_LOG(INFO, "Exception: %s in OCBindResourceInsToResource",ex.what());
        return OC_STACK_ERROR;
    }
}

OCStackResult RDHelper::rdDatabaseSetStorageFilename(const char *databaseFilename)
{
    OCStackResult actualResult =  OC_STACK_ERROR;
    actualResult = OCRDDatabaseSetStorageFilename(databaseFilename);
    return actualResult;
}