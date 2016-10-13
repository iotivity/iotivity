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
#ifndef PMCsdkMotHelper_H_
#define PMCsdkMotHelper_H_

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include "ocprovisioningmanager.h"
#include "oxmjustworks.h"
#include "oxmrandompin.h"
#include "securevirtualresourcetypes.h"
#include "srmutility.h"
#include "pmtypes.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "utlist.h"

#include "CommonUtil.h"
#include "IotivityTest_Logger.h"

/**
 *  Server and Client Resources
 */

#define JUSTWORKS_SERVER "./sampleserver_justworks"
#define JUSTWORKS_SERVER7 "./sampleserver_justworks 7"
#define JUSTWORKS_SERVER2 "./sampleserver_justworks 2"
#define JUSTWORKS_SERVER7_CBOR "./mot_preconfig_pin_server.dat"
#define JUSTWORKS_SERVER2_CBOR "./oic_svr_db_server_justworks.dat"
#define JUSTWORKS_SERVER7_CBOR_BACKUP "../mot_preconfig_pin_server.dat"
#define JUSTWORKS_SERVER2_CBOR_BACKUP "../oic_svr_db_server_justworks.dat"
#define RANDOMPIN_SERVER "./sampleserver_randompin"

class PMCsdkMotHelper
{
private:
    std::string m_failureMessage;

public:

    PMCsdkMotHelper();
    static FILE* fopenMotClient(const char*, const char*);

    bool initMotClient();
    bool discoverMultipleOwnerEnabledDevices(int nTime, OCProvisionDev_t** motdev_list,
            OCStackResult expectedResult);
    bool discoverMultipleOwnedDevices(int nTime, OCProvisionDev_t** motOnwedDev_list,
            OCStackResult expectedResult);
    bool addPreconfigPIN(const OCProvisionDev_t *targetDeviceInfo, const char* preconfPIN,
            size_t preconfPINLen, OCStackResult expectedResult);
    bool doMultipleOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices,
            OCProvisionResultCB resultCallback, OCStackResult expectedResult);

    /**
     * Callback Methods
     */
    static void multipleOwnershipTransferCB(void* ctx, int nOfRes, OCProvisionResult_t* arr,
            bool hasError);

    /**
     * All Utility Methods for Provision Manager
     */
    std::string getFailureMessage();
    std::string setFailureMessage(OCStackResult actualResult, OCStackResult expectedResult);
    std::string setFailureMessage(std::string errorMessage);
    int waitCallbackRet(void);

};
#endif
