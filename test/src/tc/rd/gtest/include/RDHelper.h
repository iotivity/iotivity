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


#ifndef INCLUDE_TESTCASE_RDHELPER_H_
#define INCLUDE_TESTCASE_RDHELPER_H_

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "OCPlatform.h"
#include "OCApi.h"
#include "rd_server.h"
#include "rd_client.h"
#include "oic_string.h"
#include "CommonUtil.h"
#include "IotivityTest_Logger.h"

#define SERVER 								"./rd_server"
#define RD_START_SERVER_WITH_INIT 			true
#define RD_START_SERVER_WITHOUT_INIT 		false
#define RD_DISCOVER_WITH_PARAM 				true
#define RD_DISCOVER_WITH_INVALID_PARAM 		false
#define RD_DISCOVER_WITH_CALLBACK			true
#define RD_DISCOVER_WITHOUT_CALLBACK 		false
#define RD_NUMBER_OF_REGISTER_RESOURCE 		2
#define RD_NUMBER_OF_REGISTER_RESOURCE_ZERO 0
#define RD_UNIT16_PORT 						1
#define RD_UNIT16_PORT_NULL 				NULL
#define RD_MAX_LOOP_COUNT_VLCC				5
#define RD_RESOURCE_TYPE_LIGHT 				"core.light"
#define RD_RESOURCE_URL_LIGHT 				"/a/light"
#define RD_RESOURCE_QUERY_URL_LIGHT 		"/oic/res?rt=core.light"
#define RD_RESOURCE_TYPE_THERMOSTAT 		"core.thermostat"
#define RD_RESOURCE_URL_THERMOSTAT 			"/a/thermostat"
#define RD_RESOURCE_QUERY_URL_THERMOSTAT 	"/oic/res?rt=core.thermostat"
#define RD_RESOURCE_TYPE_INVALID 			"core.abcdefg"
#define RD_RESOURCE_TYPE_EMPTY 				""
#define RD_RESOURCE_HOST_EMPTY 				""
#define RD_RESOURCE_INTERFACE_TYPE_EMPTY 	""
#define RD_RESOURCE_TYPE_NULL 				NULL
#define RD_RESOURCE_INTERFACE_TYPE_NULL 	NULL
#define RD_SLEEPTIME_MAX 					5
#define RD_SLEEPTIME_MIN 					2

extern int g_quitFlag;
extern bool g_foundRDResource;
class RDHelper
{
private:
	static RDHelper* s_rdHelper;
    pthread_t m_helperThread;

public:
	RDHelper(void);
	~RDHelper(void);
	/**
	 * @brief 	function for getting helper instance
	 * @return	helper instance
	 */
	static RDHelper* getInstance(void);
	
	/**
	* @brief 				This function will be registered resources that will be published by RD client
	*/
	void registerLocalResources();

	/**
	* @brief 				This function will be removed all registered rd resources
	*/
    void unregisterLocalResources();

	/**
	* @brief 				cbBiasFactor callback function invoked when bias factor is returned by the Resource Directory Server
	*/
    static int biasFactorCB(char addr[MAX_ADDR_STR_SIZE], uint16_t port);

	/**
	* @brief   				This function will create a client process by thread which will perform discover and publish client
	*/
    void startClient();

	/**
	* @brief 				The pthread_join() subroutine blocks the calling thread until the specified thread id thread terminates.
	*/
    void joinThread();

	/**
	* @brief   				runThread is a C++ routine that the thread will execute once it is created
	*/
    static void runThread(RDHelper * arg);

	/**
	* @brief   				Function is for getting status of start RD server
    * @param isWithOCInit 	Define OCInit API will be used or not
    * @return  				OCStackResult
    */
    OCStackResult rdCheckStartServerStatus(bool isWithOCInit);

    /**
    * @brief   				This function will stop the resource directory and removes all published
    * 			 			resources information
    * @return  				::OC_STACK_OK upon success, ::OC_STACK_ERROR is returned except
    *			   			the case that OC_STACK_SUCCESS is returned
    */
    OCStackResult rdStopServer();

    /**
    * @brief   				This function searches a RD server and obtain the bias factor
    * @param cbBiasFactor 	Callback function invoked when bias factor is returned by
    *          				the Resource Directory Server
    * @return 				::OC_STACK_OK upon success, ::OC_STACK_ERROR in case of error
    */
    OCStackResult rdDiscover(bool isCheckedWithParam,bool isCheckedCallback);

    /**
    * @brief   				Publish resource on the RD
    * @param addr 			The IP address of the RD, it could be either retrieved via OCRDDiscover()
    * @param port 			The port of the RD
    * @param num 			This denotes the number of registered resource handles being passed
    * @returns 				::OC_STACK_OK when successful and OC_STACK_ERROR when query failed
    */
    OCStackResult rdPublish(char *addr,uint16_t port,int num);

    /**
    * @brief   				Checks based on the resource type if the entity exists in the resource directory
    * @param interfaceType 	A interface type that is being queried
    * @param resourceType 	A resource type that is being queried
    * @param payload 		A payload of the matching resource type or interface type or NULL
    * @param addr 			A device address
    * @return 				::OC_STACK_OK upon success, ::OC_STACK_ERROR is returned except the case that OC_STACK_SUCCESS is returned
    */
    OCStackResult rdCheckPublishedResource(const char *interfaceType, const char *resourceType);

    /**
    * @brief   				Publish resource on the RD
    * @returns 				::OC_STACK_OK when successful and OC_STACK_ERROR when query failed
    */
    OCStackResult rdPublish();

	/**
	* @brief 				foundResource callback function invoked when bias factor is returned by the Resource Directory Server
	*/
    static void rdFoundResourceCB(std::shared_ptr< OC::OCResource > resource);

    /**
    * @brief   					API for Service and Resource Discovery
    * @param host 				Host IP Address of a service to direct resource discovery query
    * @param resourceURI 		Name of the resource. If null or empty, performs search for all resource names
    * @param connectivityType 	::OCConnectivityType type of connectivity indicating the interface. Example: OC_WIFI, OC_ETHERNET, OC_ALL
    * @param resourceHandler	Handles callbacks, success states and failure states
    * @returns 					::OC_STACK_OK if success
    */
    OCStackResult rdFindResourcesChecked(const std::string& host, const std::string& resourceURI,OCConnectivityType connectivityType);
};

#endif
