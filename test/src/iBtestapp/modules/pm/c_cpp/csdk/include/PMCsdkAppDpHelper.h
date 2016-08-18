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
 *      LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/

#ifndef PMCsdkAppDpHelper_H_
#define PMCsdkAppDpHelper_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <sstream>

#include "ocstack.h"
#include "logger.h"
#include "ocpayload.h"
#include "payload_logging.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "utlist.h"

#include "CommonUtil.h"
#include "IotivityTest_Logger.h"

#define JUSTWORKS_SERVER_DP "'./sampleserver_justworks 6'"
#define KILL_JUSTWORKS_SERVER_DP "./sampleserver_justworks"
#define JUSTWORKS_SERVER_DP_C_DAT "./justworks_server_dp_c.dat"
#define DIRECT_PAIRING_CLIENT_C_DAT "./direct_pairing_c.dat"
#define JUSTWORKS_SERVER_DP_DAT_C_BACKUP "../justworks_server_dp_c.dat"
#define DIRECT_PAIRING_CLIENT_DAT_C_BACKUP "../direct_pairing_c.dat"

#define DP_DEV_INSTANCE_ONE 1
#define DP_PIN_LENGTH 8 // 8 digit
#define DP_DEFAULT_PIN "00000000"
#define DP_WRONG_PIN "11111111"
#define PMSEL_UOBV 10
#define EXPECTED_DEV_COUNT_ZERO 0
#define EXPECTED_DEV_COUNT 2

#define DP_DISCOVERY_TIMEOUT_ZERO 0
#define DP_DISCOVERY_TIMEOUT 5

#define CALLBACK_INVOKED 0
#define CALLBACK_NOT_INVOKED -1
#define CALLBACK_RES_WRONG -2
#define CALLBACK_TIMEOUT 60
#define DELAY_SHORT 1
#define OC_INIT_FAIL "OCInit Failed"

FILE* clientFopenDP(const char *path, const char *mode);
int printDevList(const OCDPDev_t* pList);
OCDPDev_t* getDeviceInstance(const OCDPDev_t* pList, const uint32_t dev_num);
const char *getDPResult(OCStackResult result);
int waitDpCallbackRet();

/**
 * Helper Class for Provisioning Manager
 */
bool discoverDirectPairingDevices(unsigned short waittime);
bool doDirectPairing(void *ctx, int peerDevInstance, OCPrm_t pmSel, char *pinNumber,
        OCDirectPairingCB resultCallback, OCStackResult expectedApiResult,
        OCStackResult expectedCbResult);
bool getDirectPairedDevices();
void pairingReqCBF(void *ctx, OCDPDev_t* peer, OCStackResult result);

#endif
