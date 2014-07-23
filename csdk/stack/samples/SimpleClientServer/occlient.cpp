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

static const char * LEVEL[] __attribute__ ((unused)) = {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};
char *getResult(OCStackResult result);
void printToScreen(LogLevel level, const char * tag, const char * logStr);
void printToScreenV(LogLevel level,  const char * tag, const char * format, ...);

#define TAG PCF("occlient")
#define CTX_VAL 0x99

int gQuitFlag = 0;
/* SIGINT handler: set gQuitFlag to 1 for graceful termination */
void handleSigInt(int signum) {
    if (signum == SIGINT) {
        gQuitFlag = 1;
    }
}

// This is a function called back when a device is discovered
//OC_STACK_DELETE_TRANSACTION = 0,
 //OC_STACK_KEEP_TRANSACTION,
OCStackApplicationResult clientApplicationCB(void* ctx, OCClientResponse * clientResponse) {
	printToScreen(INFO, TAG, "Entering clientApplicationCB (Application Layer CB)");

    printToScreenV(INFO, TAG, "StackResult: %s", getResult(clientResponse->result));

    printToScreenV(INFO, TAG, "JSON = %s =============> Discovered", clientResponse->resJSONPayload);

    if(ctx == (void*)CTX_VAL) {
        printToScreenV(INFO, TAG, "Callback Context recvd successfully");
    }


    return OC_STACK_KEEP_TRANSACTION;
}
//This function is called back when a resource is discovered.

int main() {
    uint8_t addr[20] = {0};
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";
    OCCallbackData cbData;

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));
    printToScreenV(INFO, TAG, "Starting occlient on address %s",addr);

    /* Initialize OCStack*/
    if (OCInit((char *) addr, port, OC_CLIENT) != OC_STACK_OK) {
        printToScreen(ERROR, TAG, "OCStack init error");
        return 0;
    }

    /* Start a discovery query*/
    char szQueryUri[64] = { 0 };
    strcpy(szQueryUri, OC_EXPLICIT_DEVICE_DISCOVERY_URI);
    cbData.cb = clientApplicationCB;
    cbData.context = (void*)CTX_VAL;
    if (OCDoResource(OC_REST_GET, szQueryUri, 0, 0, OC_NON_CONFIRMABLE, &cbData)
            != OC_STACK_OK) {
        printToScreen(ERROR, TAG, "OCStack resource error");
        return 0;
    }

    // Break from loop with Ctrl+C
    printToScreen(INFO, TAG, "Entering occlient main loop...");
    signal(SIGINT, handleSigInt);
    while (!gQuitFlag) {

        if (OCProcess() != OC_STACK_OK) {
            printToScreen(ERROR, TAG, "OCStack process error");
            return 0;
        }

        sleep(1);
    }
    printToScreen(INFO, TAG, "Exiting occlient main loop...");

    if (OCStop() != OC_STACK_OK) {
        printToScreen(ERROR, TAG, "OCStack stop error");
    }

    return 0;
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
