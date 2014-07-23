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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <ocstack.h>

static const char * LEVEL[] __attribute__ ((unused)) = {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};
char *getResult(OCStackResult result);
void printToScreen(LogLevel level, const char * tag, const char * logStr);
void printToScreenV(LogLevel level,  const char * tag, const char * format, ...);

#define TAG PCF("ocserver")

int gQuitFlag = 0;
void createLEDResource();
typedef struct LEDRESOURCE{
	OCResourceHandle handle;
	bool power;
} LEDResource;

static LEDResource LED;

void MyOCEntityHandler (OCEntityHandlerFlag flag, const char *request, char *response, uint16_t maxResponseSize, OCResourceHandle resource) {
	char* typeOfMessage;
	switch (flag) {
		case OC_INIT_FLAG:
			typeOfMessage = "OC_INIT_FLAG";
			break;
		case OC_REQUEST_FLAG:
			typeOfMessage = "OC_REQUEST_FLAG";
			break;
		case OC_OBSERVE_FLAG:
			typeOfMessage = "OC_OBSERVE_FLAG";
			break;
		default:
			typeOfMessage = "UNKNOWN";
	}
	printToScreenV(INFO, TAG, "/nReceiving message type:/n/t %s. /n/nWith request:/n/t %s", typeOfMessage, request);
}

/* SIGINT handler: set gQuitFlag to 1 for graceful termination */
void handleSigInt(int signum) {
    if (signum == SIGINT) {
        gQuitFlag = 1;
    }
}

int main() {
	printToScreen(DEBUG, TAG, "OCServer is starting...");
	LED = {0};
    uint8_t addr[20] = {0};
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    printToScreenV(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    if (OCInit((char *) addr, port, OC_SERVER) != OC_STACK_OK) {
        printToScreen(ERROR, TAG, "OCStack init error");
        return 0;
    }

	/*
	 * Declare and create the example resource: LED
	 */
    createLEDResource();

    // Break from loop with Ctrl-C
    printToScreen(INFO, TAG, "Entering ocserver main loop...");
    signal(SIGINT, handleSigInt);
    while (!gQuitFlag) {
        if (OCProcess() != OC_STACK_OK) {
            printToScreen(ERROR, TAG, "OCStack process error");
            return 0;
        }
        sleep(1);
    }

   printToScreen(INFO, TAG, "Exiting ocserver main loop...");

    if (OCStop() != OC_STACK_OK) {
        printToScreen(ERROR, TAG, "OCStack process error");
    }

    return 0;
}
void createLEDResource() {
	LED.power = false;
	OCStackResult res = OCCreateResource(&LED.handle,
					"core.led",
					"state:oc.bt.b;power:oc.bt.i",
					"core.rw",
					OC_REST_GET|OC_REST_PUT,
					"/a/led",
					0,
					OC_DISCOVERABLE|OC_OBSERVABLE);
	printToScreenV(INFO, TAG, "Created LED resource with result: %s", getResult(res));
}
char *getResult(OCStackResult result) {
	char resString[40] = {0};
	strcpy(resString, "Result: ");
	switch(result) {
		case OC_STACK_OK:
			strcat(resString, "OC_STACK_OK");
			break;
		case OC_STACK_INVALID_URI:
			strcat(resString, "OC_STACK_INVALID_URI");
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
		case OC_STACK_ERROR:
			strcat(resString, "OC_STACK_ERROR");
			break;
		default:
			strcat(resString, "UNKNOWN");
	}
	return resString;
}
void printToScreen(LogLevel level, const char * tag, const char * logStr) {
	printf("%s: %s: %s\n", LEVEL[level], tag, logStr);
}
void printToScreenV(LogLevel level,  const char * tag, const char * format, ...) {
	if (!format || !tag) {
	        return;
	    }
	    char buffer[MAX_LOG_V_BUFFER_SIZE];
	    memset(buffer, 0, sizeof buffer);
	    va_list args;
	    va_start(args, format);
	    vsnprintf(buffer, sizeof buffer - 1, format, args);
	    va_end(args);
	    printToScreen(level, tag, buffer);
}

