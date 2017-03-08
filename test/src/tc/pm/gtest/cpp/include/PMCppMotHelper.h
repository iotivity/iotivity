/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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

#ifndef PMCppMOtHelper_H_
#define PMCppMOtHelper_H_

#if defined(__MOT__)
#include "PMCppUtilityHelper.h"

class PMCppMotHelper
{
private:
    std::string m_failureMessage;

public:
    static OCPersistentStorage s_pstMot;

    PMCppMotHelper();

    static FILE* fopenMotClient(const char*, const char*);

    bool initMotClient();

    bool discoverMultipleOwnerEnabledDevices(unsigned short timeout, DeviceList_t &list,
            OCStackResult expectedResult);

    bool discoverMultipleOwnedDevices(unsigned short timeout, DeviceList_t &list,
            OCStackResult expectedResult);

    bool discoverMultipleOwnerEnabledDevice(unsigned short timeout, const OicUuid_t* deviceID,
            std::shared_ptr< OCSecureResource > &foundDevice, OCStackResult expectedResult);

    bool doMultipleOwnershipTransfer(DeviceList_t &data, ResultCallBack resultCallback,
            OCStackResult expectedResult);

    bool addPreconfigPIN(DeviceList_t &data, const char* preconfPIN, size_t preconfPINLength,
            OCStackResult expectedResult);

    bool provisionPreconfPin(DeviceList_t &data, const char *preconfPin, size_t preconfPinLength,
            ResultCallBack resultCallback, OCStackResult expectedResult);

    bool changeMOTMode(DeviceList_t &data, const OicSecMomType_t momType,
            ResultCallBack resultCallback, OCStackResult expectedResult);

    bool selectMOTMethod(DeviceList_t &data, const OicSecOxm_t oxmSelVal,
            ResultCallBack resultCallback, OCStackResult expectedResult);


    bool isSubownerOfDevice(DeviceList_t &data, bool* subowner, OCStackResult expectedResult,
            bool expectedVal);

    bool getMOTMethod(DeviceList_t &data, OicSecOxm_t* oxm, OCStackResult expectedResult);

    bool isMOTSupported(DeviceList_t &data, bool expectedVal);

    bool isMOTEnabled(DeviceList_t &data, bool expectedVal);

    static void multipleOwnershipTransferCB(PMResultList_t *result, int hasError);

    static void provisionPreconfPinCB(PMResultList_t *result, int hasError);

    static void changeMOTModeCB(PMResultList_t *result, int hasError);

    static void selectMOTMethodCB(PMResultList_t *result, int hasError);

    /**
     * All Utility Methods for Provision Manager
     */
    std::string getFailureMessage();

    int waitCallbackRet(void);
};
#endif /*_MOT_*/

#endif /* PMCppMOtHelper_H_ */
