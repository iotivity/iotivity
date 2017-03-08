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
#if defined(__MOT__)
#include "PMCsdkUtilityHelper.h"

#define ctxProvisionPreconfPin "ProvisionPreconfPin Context"
#define ctxChangeMOTMode "OCChangeMOTMode Context"
#define ctxSelectMOTMethod "OCSelectMOTMethod Context"

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

    bool discoverMultipleOwnerEnabledSingleDevice(unsigned short timeoutSeconds,
            const OicUuid_t *deviceID, OCProvisionDev_t **motdev_list, OCStackResult expectedResult);

    bool discoverMultipleOwnedDevices(int nTime, OCProvisionDev_t** motOnwedDev_list,
            OCStackResult expectedResult);

    bool addPreconfigPIN(const OCProvisionDev_t *targetDeviceInfo, const char* preconfPIN,
            size_t preconfPINLen, OCStackResult expectedResult);

    bool doMultipleOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices,
            OCProvisionResultCB resultCallback, OCStackResult expectedResult);

    bool provisionPreconfPin(void* ctx, OCProvisionDev_t *targetDeviceInfo, const char * preconfPin, size_t preconfPinLen, OCProvisionResultCB resultCallback, OCStackResult expectedResult);

    bool changeMOTMode(void *ctx, const OCProvisionDev_t *targetDeviceInfo, const OicSecMomType_t momType, OCProvisionResultCB resultCallback, OCStackResult expectedResult);

    bool selectMOTMethod(void *ctx, const OCProvisionDev_t *targetDeviceInfo, const OicSecOxm_t oxmSelValue, OCProvisionResultCB resultCallback, OCStackResult expectedResult);

    static void provisionPreconfPinCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr, bool hasError);

    static void changeMOTModeCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr, bool hasError);

    static void selectMOTMethodCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr, bool hasError);

    /**
     * Callback Methods
     */
    static void multipleOwnershipTransferCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr,
            bool hasError);

    /**
     * All Utility Methods for Provision Manager
     */
    std::string getFailureMessage();

    int waitCallbackRet(void);
};
#endif /*_MOT_*/
#endif /*PMCsdkMotHelper_H_*/
