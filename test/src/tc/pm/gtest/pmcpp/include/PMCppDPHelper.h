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

#ifndef PMCppDPHelper_H_
#define PMCppDPHelper_H_
#if defined(__DIRECTPAIRING__)

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <map>
#include <cstdlib>
#include <pthread.h>
#include <mutex>
#include <condition_variable>

#include "logger.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "OCPlatform.h"
#include "OCApi.h"

#include "CommonUtil.h"
#include "IotivityTest_Logger.h"

using namespace OC;

#define JUSTWORKS_SERVER_DP "'./sampleserver_justworks 5'"
#define KILL_JUSTWORKS_SERVER_DP "./sampleserver_justworks"
#define JUSTWORKS_SERVER_DP_DAT "./justworks_server_dp_cpp.dat"
#define DIRECT_PAIRING_CLIENT_DAT "./direct_pairing_cpp.dat"
#define JUSTWORKS_SERVER_DP_DAT_BACKUP "../justworks_server_dp_cpp.dat"
#define DIRECT_PAIRING_CLIENT_DAT_BACKUP "../direct_pairing_cpp.dat"

#define ALL_IPV4_ADDRESS "0.0.0.0"
#define DP_DISCOVERY_TIMEOUT 5
#define DP_DISCOVERY_TIMEOUT_ZERO 0
#define DP_PIN_LENGTH 8
#define DP_PRECONFIG_PIN "00000000"
#define DP_WRONG_PIN "11111111"

#define CALLBACK_INVOKED 0
#define CALLBACK_NOT_INVOKED -1
#define CALLBACK_RES_WRONG -2
#define CALLBACK_TIMEOUT 60
#define DELAY_SHORT 1
#define OC_INIT_FAIL "OCInit Failed"

#define DP_DEV_INSTANCE_ONE 0
#define EMPTY_STRING ""

FILE* clientOpen(const char* /*fileName*/, const char *mode);

class PMCppDPHelper
{
private:
    std::string m_failureMessage;

public:

    PMCppDPHelper();

    //Helper for APIs from OCPlatform.h
    bool findDirectPairingDevices(unsigned short waittime, GetDirectPairedCallback callback,
            OCStackResult expectedResult);
    bool doDirectPairing(int nPeerDevInstance, OCPrm_t pmSel, const std::string& pinNumber,
            DirectPairingCallback resultCallback, OCStackResult expectedResult,
            OCStackResult expectedCbResult = OC_STACK_OK);
    bool getDirectPairedDevices(GetDirectPairedCallback callback, OCStackResult expectedResult);

    //Helper for APIs from OCDirectPairing.h
    bool getHost(int nDevInstance);
    bool getDeviceID(int nDevInstance);
    bool getPairingMethods(int nDevInstance);
    bool getConnType(int nDevInstance);
    bool getDev(int nDevInstance);

    // Callback functions
    static void findCallback(PairedDevices discoveredDevList);
    static void resultCallback(std::shared_ptr< OCDirectPairing > ptr, OCStackResult result);
    static void pairedDevListCB(PairedDevices pairedDevList);

    std::string getPairingMethodsType(int connectionType);
    std::string getConnectivityTypeName(OCConnectivityType connType);
    static std::string getDpOCStackResult(OCStackResult ocstackresult);
    std::string getFailureMessage();
    std::string setFailureMessage(OCStackResult expectedResult, OCStackResult actualResult);
    std::string setFailureMessage(std::string errorMessage);
    int waitCallbackRet();

};
#endif

#endif /*#if defined(__DIRECTPAIRING__)*/
