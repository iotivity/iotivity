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

#ifndef PMCppHelper_H_
#define PMCppHelper_H_

#include "PMCppUtilityHelper.h"

class PMCppHelper
{
private:
    std::string m_failureMessage;

public:

    PMCppHelper();bool provisionInit();bool discoverUnownedDevices(int time, DeviceList_t& data,
            OCStackResult expectedResult);bool discoverOwnedDevices(int time, DeviceList_t& data,
            OCStackResult expectedResult);bool getDevInfoFromNetwork(unsigned short time,
            DeviceList_t& ownedDevList, DeviceList_t &unownedDevList, OCStackResult expectedResult);bool setOwnerTransferCallbackData(
            int num, OTMCallbackData_t& data, InputPinCallback inputPin,
            OCStackResult expectedResult);bool doOwnershipTransfer(DeviceList_t &data,
            ResultCallBack resultCallback, OCStackResult expectedResult);bool provisionACL(
            DeviceList_t& deviceList, const OicSecAcl_t* acl, ResultCallBack resultCallback,
            OCStackResult expectedResult);bool provisionCredentials(DeviceList_t& deviceList,
            const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback,
            OCStackResult expectedResult);bool provisionPairwiseDevices(DeviceList_t& deviceList,
            const Credential &cred, const OicSecAcl_t* acl1, const OCSecureResource &device2,
            const OicSecAcl_t* acl2, ResultCallBack resultCallback, OCStackResult expectedResult);bool getLinkedDevices(DeviceList_t& deviceList,
            UuidList_t &uuidList, OCStackResult expectedResult);bool unlinkDevices(
            DeviceList_t& deviceList, const OCSecureResource &device2,
            ResultCallBack resultCallback, OCStackResult expectedResult);bool removeDevice(
            DeviceList_t& deviceList, unsigned short waitTimeForOwnedDeviceDiscovery,
            ResultCallBack resultCallback, OCStackResult expectedResult);bool removeDeviceWithUuid(
            unsigned short waitTimeForOwnedDeviceDiscovery, std::string uuid,
            ResultCallBack resultCallback, OCStackResult expectedResult);bool provisionTrustCertChain(
            DeviceList_t& deviceList, OicSecCredType_t type, uint16_t credId,
            ResultCallBack resultCallback, OCStackResult expectedResult);

    static void ownershipTransferCB(PMResultList_t *result, int hasError);
    static void provisionCB(PMResultList_t *result, int hasError);

    static void InputPinCB(char* pinBuf, size_t bufSize);
    static OicSecAcl_t* createAcl(const int dev_num, int nPermission, DeviceList_t &m_OwnedDevList);
    static OicSecPdAcl_t* createPdAcl(int nPermission);
    OCProvisionDev_t* getDevInst(const OCProvisionDev_t* dev_lst, const int dev_num);

    std::string getFailureMessage();
    int waitCallbackRet();
};
#endif
