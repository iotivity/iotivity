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

#ifndef PMCppHelper_H_
#define PMCppHelper_H_

#include "PMCppUtilityHelper.h"

class PMCppHelper
{
private:
    std::string m_failureMessage;

public:
    static OCPersistentStorage s_ps;

    PMCppHelper();

    static FILE* clientOpen(const char *UNUSED_PARAM, const char *mode);

    bool provisionInit(const std::string& dbPath = CLIENT_DATABASE);

    bool setRandomPinPolicy(size_t pinSize, OicSecPinType_t pinType, OCStackResult expectedResult);

    bool setDisplayPinCB(GeneratePinCallback displayPin, OCStackResult expectedResult);

    bool unsetDisplayPinCB(OCStackResult expectedResult);

    bool registerInputPinCallback(InputPinCB inputPinCB,
            InputPinCallbackHandle* inputPinCallbackHandle, OCStackResult expectedResult);

    bool deregisterInputPinCallback(InputPinCallbackHandle inputPinCallbackHandle,
            OCStackResult expectedResult);

    bool registerDisplayPinCallback(DisplayPinCB displayPinCB,
            DisplayPinCallbackHandle* displayPinCallbackHandle, OCStackResult expectedResult);

    bool deregisterDisplayPinCallback(DisplayPinCallbackHandle displayPinCallbackHandle,
            OCStackResult expectedResult);

    bool setInputPinCallback(InputPinCallback inputPin, OCStackResult expectedResult);

    bool unsetInputPinCallback(OCStackResult expectedResult);

    bool registerDisplayNumCallback(DisplayNumCB displayNumCB, OCStackResult expectedResult);

    bool deregisterDisplayNumCallback(OCStackResult expectedResult);

    bool registerUserConfirmCallback(UserConfirmNumCB userConfirmCB, OCStackResult expectedResult);

    bool deregisterUserConfirmCallback(OCStackResult expectedResult);

    bool discoverUnownedDevices(int time, DeviceList_t& data, OCStackResult expectedResult);

    bool discoverSingleDevice(unsigned short timeout, const OicUuid_t* deviceID,
            std::shared_ptr< OCSecureResource > &foundDevice, OCStackResult expectedResult);

    bool discoverSingleDeviceInUnicast(unsigned short timeout, const OicUuid_t* deviceID,
            const std::string& hostAddress, OCConnectivityType connType,
            std::shared_ptr< OCSecureResource > &foundDevice, OCStackResult expectedResult);

    bool discoverOwnedDevices(int time, DeviceList_t& data, OCStackResult expectedResult);

    bool getDevInfoFromNetwork(unsigned short time, DeviceList_t& ownedDevList,
            DeviceList_t &unownedDevList, OCStackResult expectedResult);

    bool setOwnerTransferCallbackData(int num, OTMCallbackData_t& data, InputPinCallback inputPin,
            OCStackResult expectedResult);

    bool doOwnershipTransfer(DeviceList_t &data, ResultCallBack resultCallback,
            OCStackResult expectedResult);

    bool provisionACL(DeviceList_t& deviceList, const OicSecAcl_t* acl,
            ResultCallBack resultCallback, OCStackResult expectedResult);

    bool provisionCredentials(DeviceList_t& deviceList, const Credential &cred,
            const OCSecureResource &device2, ResultCallBack resultCallback,
            OCStackResult expectedResult);

    bool provisionPairwiseDevices(DeviceList_t& deviceList, const Credential &cred,
            const OicSecAcl_t* acl1, const OCSecureResource &device2, const OicSecAcl_t* acl2,
            ResultCallBack resultCallback, OCStackResult expectedResult);

    bool getLinkedDevices(DeviceList_t& deviceList, UuidList_t &uuidList,
            OCStackResult expectedResult);

    bool getOTMethod(DeviceList_t& deviceList, OicSecOxm_t* oxm, OCStackResult expectedResult);

    bool getSelectedOwnershipTransferMethod(DeviceList_t& deviceList, OicSecOxm_t expectedResult);

    bool unlinkDevices(DeviceList_t& deviceList, const OCSecureResource &device2,
            ResultCallBack resultCallback, OCStackResult expectedResult);

    bool removeDevice(DeviceList_t& deviceList, unsigned short waitTimeForOwnedDeviceDiscovery,
            ResultCallBack resultCallback, OCStackResult expectedResult);

    bool removeDeviceWithUuid(unsigned short waitTimeForOwnedDeviceDiscovery, std::string uuid,
            ResultCallBack resultCallback, OCStackResult expectedResult);

    bool saveACL(const OicSecAcl_t* acl, OCStackResult expectedResult);

    bool provisionTrustCertChain(DeviceList_t& deviceList, OicSecCredType_t type, uint16_t credId,
            ResultCallBack resultCallback, OCStackResult expectedResult);

    static void createAcl(OicSecAcl_t *acl, const int dev_num, int nPermission,
            DeviceList_t &m_OwnedDevList);

    static OicSecAcl_t* createAclForLEDAccess(const OicUuid_t* subject);

    static void ownershipTransferCB(PMResultList_t *result, int hasError);

    static void provisionCB(PMResultList_t *result, int hasError);

    static void inputPinCB(char* pinBuf, size_t bufSize);

    static void OnInputPinCB(OicUuid_t deviceId, char* pinBuf, size_t bufSize);

    static void OnDisplayPinCB(char* pinData, size_t pinSize);

    static OCStackResult displayMutualVerifNumCB(uint8_t mutualVerifNum[MUTUAL_VERIF_NUM_LEN]);

    static OCStackResult confirmMutualVerifNumCB(void);

    static OicSecPdAcl_t* createPdAcl(int nPermission);

    bool deleteACLList(OicSecAcl_t* pAcl);

    OCProvisionDev_t* getDevInst(const OCProvisionDev_t* dev_lst, const int dev_num);

    std::string getFailureMessage();

    int waitCallbackRet();

    bool convertStrToUuid(std::string uuid, OicUuid_t* deviceID, OCStackResult expectedResult);
};

#endif
