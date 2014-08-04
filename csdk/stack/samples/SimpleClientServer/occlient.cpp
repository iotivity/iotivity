//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <ocstack.h>
#include <iostream>
#include <sstream>

char *getResult(OCStackResult result);
std::string getIPAddrTBServer(OCClientResponse * clientResponse);
std::string getPortTBServer(OCClientResponse * clientResponse);
std::string getQueryStrForGetPut(unsigned  const char * responsePayload);

#define TAG PCF("occlient")
#define CTX_VAL 0x99
#ifndef MAX_LENGTH_IPv4_ADDR
#define MAX_LENGTH_IPv4_ADDR 16
#endif

static std::string putPayload = "{\"state\":\"off\",\"power\":\"0\"}";

int gQuitFlag = 0;
/* SIGINT handler: set gQuitFlag to 1 for graceful termination */
void handleSigInt(int signum) {
	if (signum == SIGINT) {
		gQuitFlag = 1;
	}
}

OCStackApplicationResult clientApplicationGETCb(void* ctx, OCClientResponse * clientResponse);

OCStackApplicationResult clientApplicationPUTCb(void* ctx, OCClientResponse * clientResponse) {	
	if(clientResponse) {}
	if(ctx == (void*)CTX_VAL) {
		OC_LOG_V(INFO, TAG, "Callback Context for PUT query recvd successfully");
		OC_LOG_V(INFO, TAG, "JSON = %s =============> Discovered", clientResponse->resJSONPayload);	
	}

	return OC_STACK_KEEP_TRANSACTION;
}

OCStackApplicationResult clientApplicationGETCb(void* ctx, OCClientResponse * clientResponse) {
	if(clientResponse) {}
	if(ctx == (void*)CTX_VAL) {
		OC_LOG_V(INFO, TAG, "Callback Context for GET query recvd successfully");
		OC_LOG_V(INFO, TAG, "JSON = %s =============> Discovered", clientResponse->resJSONPayload);
	//* Make a GET query*/
		std::ostringstream getQuery;
		getQuery << "coap://" << getIPAddrTBServer(clientResponse) << ":" << getPortTBServer(clientResponse) << getQueryStrForGetPut(clientResponse->resJSONPayload);
		OCCallbackData cbData;
		cbData.cb = clientApplicationPUTCb;
		cbData.context = (void*)CTX_VAL;
		//OCDoHandle handle;

		OC_LOG_V(INFO, TAG, "PUT payload from client = %s ", putPayload.c_str());	
		if (OCDoResource(OC_REST_PUT, getQuery.str().c_str(), 0, putPayload.c_str(), OC_NON_CONFIRMABLE, &cbData)
				!= OC_STACK_OK) {
			OC_LOG_V(ERROR, TAG, "OCStack resource error");
			//reOC_LOG_Vturn 0;
		}
	}
	return OC_STACK_KEEP_TRANSACTION;
}


// This is a function called back when a device is discovered
OCStackApplicationResult clientApplicationCB(void* ctx,
        OCClientResponse * clientResponse) {
    uint8_t remoteIpAddr[4];
    uint16_t remotePortNu;

    OC_LOG(INFO, TAG,
            "Entering clientApplicationCB (Application Layer CB)");
    OC_LOG_V(INFO, TAG, "StackResult: %s",
            getResult(clientResponse->result));

    if (ctx == (void*) CTX_VAL) {
        OC_LOG_V(INFO, TAG, "Callback Context recvd successfully");
    }

    OCDevAddrToIPv4Addr((OCDevAddr *) clientResponse->addr, remoteIpAddr,
            remoteIpAddr + 1, remoteIpAddr + 2, remoteIpAddr + 3);
    OCDevAddrToPort((OCDevAddr *) clientResponse->addr, &remotePortNu);

    OC_LOG_V(INFO, TAG,
            "Device =============> Discovered %s @ %d.%d.%d.%d:%d",
            clientResponse->resJSONPayload, remoteIpAddr[0], remoteIpAddr[1],
            remoteIpAddr[2], remoteIpAddr[3], remotePortNu);

	//* Make a GET query*/
	std::ostringstream getQuery;
	getQuery << "coap://" << getIPAddrTBServer(clientResponse) << ":" << getPortTBServer(clientResponse) << getQueryStrForGetPut(clientResponse->resJSONPayload);
	OCCallbackData cbData;
	cbData.cb = clientApplicationGETCb;
	cbData.context = (void*)CTX_VAL;
	if (OCDoResource(OC_REST_GET, getQuery.str().c_str(), 0, 0, OC_NON_CONFIRMABLE, &cbData)
			!= OC_STACK_OK) {
		OC_LOG(ERROR, TAG, "OCStack resource error");
		//return 0;
	}

	return OC_STACK_KEEP_TRANSACTION;
}
//This function is called back when a resource is discovered.

int main() {
	uint8_t addr[20] = {0};
	uint8_t* paddr = NULL;
	uint16_t port = USE_RANDOM_PORT;
	uint8_t ifname[] = "eth0";
	OCCallbackData cbData;

	/*Get Ip address on defined interface and initialize coap on it with random port number
	 * this port number will be used as a source port in all coap communications*/
    if ( OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr,
                               sizeof(addr)) == ERR_SUCCESS)
    {
    	OC_LOG_V(INFO, TAG, "Starting occlient on address %s",addr);
        paddr = addr;
    }

	/* Initialize OCStack*/
	if (OCInit((char *) paddr, port, OC_CLIENT) != OC_STACK_OK) {
		OC_LOG(ERROR, TAG, "OCStack init error");
		return 0;
	}

	/* Start a discovery query*/
	char szQueryUri[64] = { 0 };
	strcpy(szQueryUri, OC_WELL_KNOWN_QUERY);
	cbData.cb = clientApplicationCB;
	cbData.context = (void*)CTX_VAL;
	if (OCDoResource(OC_REST_GET, szQueryUri, 0, 0, OC_NON_CONFIRMABLE, &cbData)
			!= OC_STACK_OK) {
		OC_LOG(ERROR, TAG, "OCStack resource error");
		return 0;
	}

	// Break from loop with Ctrl+C
	OC_LOG(INFO, TAG, "Entering occlient main loop...");
	signal(SIGINT, handleSigInt);
	while (!gQuitFlag) {

		if (OCProcess() != OC_STACK_OK) {
			OC_LOG(ERROR, TAG, "OCStack process error");
			return 0;
		}

		sleep(1);
	}
	OC_LOG(INFO, TAG, "Exiting occlient main loop...");

	if (OCStop() != OC_STACK_OK) {
		OC_LOG(ERROR, TAG, "OCStack stop error");
	}

	return 0;
}

char *getResult(OCStackResult result) {
	char *resString = new char[40];
	memset(resString, 0, 40);
    strcpy(resString, "Result: ");
    switch (result) {
    case OC_STACK_OK:
        strcat(resString, "OC_STACK_OK");
        break;
    case OC_STACK_INVALID_URI:
        strcat(resString, "OC_STACK_INVALID_URI");
        break;
    case OC_STACK_INVALID_QUERY:
        strcat(resString, "OC_STACK_INVALID_QUERY");
        break;
    case OC_STACK_INVALID_IP:
        strcat(resString, "OC_STACK_INVALID_IP");
        break;
    case OC_STACK_INVALID_PORT:
        strcat(resString, "OC_STACK_INVALID_PORT");
        break;
    case OC_STACK_INVALID_CALLBACK:
        strcat(resString, "OC_STACK_INVALID_CALLBACK");
        break;
    case OC_STACK_INVALID_METHOD:
        strcat(resString, "OC_STACK_INVALID_METHOD");
        break;
    case OC_STACK_NO_MEMORY:
        strcat(resString, "OC_STACK_NO_MEMORY");
        break;
    case OC_STACK_COMM_ERROR:
        strcat(resString, "OC_STACK_COMM_ERROR");
        break;
    case OC_STACK_INVALID_PARAM:
        strcat(resString, "OC_STACK_INVALID_PARAM");
        break;
    case OC_STACK_NOTIMPL:
        strcat(resString, "OC_STACK_NOTIMPL");
        break;
    case OC_STACK_NO_RESOURCE:
        strcat(resString, "OC_STACK_NO_RESOURCE");
        break;
    case OC_STACK_RESOURCE_ERROR:
        strcat(resString, "OC_STACK_RESOURCE_ERROR");
        break;
    case OC_STACK_SLOW_RESOURCE:
        strcat(resString, "OC_STACK_SLOW_RESOURCE");
        break;
    case OC_STACK_ERROR:
        strcat(resString, "OC_STACK_ERROR");
        break;
    default:
        strcat(resString, "UNKNOWN");
    }
    return resString;
}


std::string getIPAddrTBServer(OCClientResponse * clientResponse) {
	if(!clientResponse) return "";
	if(!clientResponse->addr) return "";
	uint8_t a, b, c, d = 0;
	if(0 != OCDevAddrToIPv4Addr(clientResponse->addr, &a, &b, &c, &d) ) return "";

	char ipaddr[16] = {'\0'};
	snprintf(ipaddr,  sizeof(ipaddr), "%d.%d.%d.%d", a,b,c,d); // ostringstream not working correctly here, hence snprintf
	//printf("IP address string of the TB server = %s\n", *out_ipaddr);
	return std::string (ipaddr);
}


std::string getPortTBServer(OCClientResponse * clientResponse){
	if(!clientResponse) return "";
	if(!clientResponse->addr) return "";
	uint16_t p = 0;
	if(0 != OCDevAddrToPort(clientResponse->addr, &p) ) return "";
	std::ostringstream ss;
	ss << p;
	return ss.str();
}

std::string getQueryStrForGetPut(unsigned  const char * responsePayload){
	//  JSON = {"oc":{"payload":[{"href":"/a/led","rt":["core.led"],"if":["core.rw"],"obs":1}]}}

	//std::string jsonPayload(responsePayload, responsePayload + sizeof responsePayload / sizeof responsePayload[0]);
	std::string jsonPayload(reinterpret_cast<char*>(const_cast<unsigned char*>(responsePayload)));
	//std::cout << jsonPayload << std::endl;

	return "/a/led";
}
