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
uint16_t g_rdPort=0;
bool g_foundRDResource = false;

using namespace OC;

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

int RDHelper::biasFactorCB(char addr[MAX_ADDR_STR_SIZE], uint16_t port)
{
	IOTIVITYTEST_LOG(INFO, "biasFactorCB Callback called");
    OICStrcpy(g_rdAddress, MAX_ADDR_STR_SIZE, addr);
    g_rdPort = port;
    return 0;
}

OCStackResult RDHelper::rdDiscover(bool isCheckedWithParam,bool isCheckedCallback)
{
	g_rdPort=0;
	OCStackResult actualResult =  OC_STACK_ERROR;
	
	try
	{
		if(isCheckedWithParam)
		{
			actualResult = OCRDDiscover(RDHelper::biasFactorCB);
		}
		else
		{
			actualResult = OCRDDiscover(NULL);
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
		
		OCStackResult actualResult =  OC_STACK_ERROR;

		actualResult = OCRDPublish(g_rdAddress, g_rdPort, 2, g_curResource_t, g_curResource_l);
		IOTIVITYTEST_LOG(INFO, "RdPublished called..\n");
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
		OCStackResult actualResult;

		if(addr=="default")
		{
			addr=g_rdAddress;
		}
		if(port==1)
		{
			port=g_rdPort;
		}
		if(num==0)
		{
			actualResult = OCRDPublish(addr,port,num);
		}
		else if(num==1)
		{
			actualResult =OCRDPublish(addr,port,num,g_curResource_t);
		}
		else
		{
			actualResult =OCRDPublish(addr,port,num,g_curResource_t, g_curResource_l);
		}

		return actualResult;
	}
	catch (std::exception &ex)
	{
		IOTIVITYTEST_LOG(INFO, "Exception: %s in rdPublish",ex.what());
		return OC_STACK_ERROR;
	}
}

OCStackResult RDHelper::rdCheckPublishedResource(const char *interfaceType, const char *resourceType)
{
	try
	{
		OCResourceCollectionPayload *payload=NULL;
		OCDevAddr rdAddr1 = { 0 };

		OCStackResult actualResult = OCRDCheckPublishedResource(interfaceType,resourceType,&payload,&rdAddr1);
		IOTIVITYTEST_LOG(DEBUG, "OCRDCheckPublishedResource result is: %s \n",CommonUtil::s_OCStackResultString.at(actualResult).c_str());
		return actualResult;
	}
	catch (std::exception &ex)
	{
		IOTIVITYTEST_LOG(INFO, "Exception: %s in rdCheckPublishedResource",ex.what());
		return OC_STACK_ERROR;
	}
}

void RDHelper::startClient()
{
	PlatformConfig config { OC::ServiceType::InProc, ModeType::Both, "0.0.0.0",0, OC::QualityOfService::LowQos };
	OCPlatform::Configure(config);
	registerLocalResources();

	pthread_create(&m_helperThread, NULL, &RDHelper::runThread, this);
}

void RDHelper::joinThread()
{
	pthread_join(m_helperThread, NULL);
}

static void RDHelper::runThread(RDHelper *arg)
{

	OCStackResult expectedResult = OC_STACK_OK;

	if(expectedResult!=arg->rdDiscover(RD_DISCOVER_WITH_PARAM,RD_DISCOVER_WITH_CALLBACK))
		IOTIVITYTEST_LOG(DEBUG, "Failed To Discover Resource Directory");

	if(expectedResult!=arg->rdPublish())
		IOTIVITYTEST_LOG(DEBUG, "Failed To Publish Resource Directory");

	g_quitFlag=1;
	IOTIVITYTEST_LOG(DEBUG, "New quitFlag value %d",g_quitFlag);

}

static void RDHelper::rdFoundResourceCB(std::shared_ptr< OC::OCResource > resource)
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
}

OCStackResult RDHelper::rdFindResourcesChecked(const std::string& host, const std::string& resourceURI,OCConnectivityType connectivityType)
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

	            actualResult = OCPlatform::findResource(host,  resourceURI, connectivityType, &rdFoundResourceCB);
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
