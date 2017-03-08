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
#ifndef PMCsdkHelper_H_
#define PMCsdkHelper_H_

#include "PMCsdkUtilityHelper.h"

#define g_ctx "Provision Manager Client Application Context"
#define ctxProvCreadential "ProvisionCredentials Context"
#define ctxProvPairwise "ctxProvPairwise"
#define ctxProvDirectPairing "ctxProvDirectPairing"
#define ctxUnlinkDevice "ctxUnlinkDevice"
#define ctxRemoveDevice "ctxRemoveDevice"
#define ctxRemoveDeviceWithUuid "ctxRemoveDeviceWithUuid"
#define ctxResetDevice "ctxResetDevice"
#define RANDOM_PIN_TEXT_FILE "server_pincode.txt"
#define PIN_MAX_SIZE 9
#define MAX_TIME_INPUT_PROMPT_FOR_PASSWORD 5

/*
 * Ownership Transfer Related Resource
 */
#define OTM_INVALID_LOBV -1
#define OTM_INVALID_UOBV 10
#define OTM_NONE 0
#define OTM_JUSTWORK 1
#define OTM_RANDOMPIN 2
#define OTM_ALL 3

/*
 * Credential Representative value
 */
#define SYM_PAIR_WISE_KEY 1
#define CRED_TYPE_UOBV 30
#define CRED_TYPE_LOBV -3

// function declaration(s) for calling them before implementing
FILE* fopenProvManager(const char*, const char*);

OicSecAcl_t* createAcl(const int dev_num, int permission, OCProvisionDev_t** m_own_list,
        bool multiple_dev = true);

OicSecPdAcl_t* createPdAcl(int nPermission);

OTMCallbackData_t otmCbRegister(int otmType);

int waitCallbackRet(void);

/**
 * Helper Class for Provisioning Manager
 */
class PMCsdkHelper
{
private:
    std::string m_failureMessage;

public:

    PMCsdkHelper();

    bool initProvisionClient(int clientOTMType = OTM_JUSTWORK, char* chDBPath =
            (char*) PRVN_DB_FILE_NAME);

    bool setOxmAllowStatus(const OicSecOxm_t oxm, const bool allowStatus,
            OCStackResult expectedResult);

    bool discoverAllDevices(int nTime, OCProvisionDev_t** own_list, OCProvisionDev_t** unown_list,
            OCStackResult expectedResult);

    bool discoverSingleDevice(unsigned short nTime, const OicUuid_t* deviceID,
            OCProvisionDev_t **ppFoundDevice, OCStackResult expectedResult);

    bool discoverSingleDeviceInUnicast(unsigned short timeout, const OicUuid_t* deviceID,
            const char* hostAddress, OCConnectivityType connType, OCProvisionDev_t **ppFoundDevice,
            OCStackResult expectedResult);

    bool discoverUnownedDevices(int nTime, OCProvisionDev_t** unown_list,
            OCStackResult expectedResult);

    bool discoverOwnedDevices(int nTime, OCProvisionDev_t** own_list, OCStackResult expectedResult);

    bool doOwnerShipTransfer(void *ctx, OCProvisionDev_t** unown_list,
            OCProvisionResultCB resultCallback, OCStackResult expectedResult, bool checkCallback =
            true);

    bool provisionACL(void* ctx, const OCProvisionDev_t* selectedDeviceInfo, OicSecAcl_t *acl,
            OCProvisionResultCB resultCallback, OCStackResult expectedResult);

    bool saveACL(const OicSecAcl_t* acl, OCStackResult expectedResult);

    bool provisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize,
            const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2,
            OCProvisionResultCB resultCallback, OCStackResult expectedResult);

    bool provisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize,
            const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2,
            OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback,
            OCStackResult expectedResult);

    bool getCredResource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo,
            OCProvisionResultCB resultCallback, OCStackResult expectedResult);

    bool getACLResource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo,
            OCProvisionResultCB resultCallback, OCStackResult expectedResult);

    bool unlinkDevices(void* ctx, const OCProvisionDev_t* pTargetDev1,
            const OCProvisionDev_t* pTargetDev2, OCProvisionResultCB resultCallback,
            OCStackResult expectedResult);

    bool removeDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery,
            const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback,
            OCStackResult expectedResult);

    bool removeDeviceWithUuid(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery,
            const OicUuid_t* pTargetUuid, OCProvisionResultCB resultCallback,
            OCStackResult expectedResult);

    bool resetDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery,
            const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback,
            OCStackResult expectedResult);

    bool resetSVRDB(OCStackResult expectedResult);

    bool deleteDiscoveredDevices(OCProvisionDev_t *pList);

    bool deleteUuidList(OCUuidList_t* pList);

    bool deleteACLList(OicSecAcl_t* pList);

    bool getLinkedStatus(const OicUuid_t* uuidOfDevice, OCUuidList_t** uuidList,
            size_t* numOfDevices, OCStackResult expectedResult);

    bool selectOwnershipTransferMethod(const OicSecOxm_t *supportedMethods,
            size_t numberOfMethods, OicSecOxm_t *selectedMethod, OwnerType_t ownerType,
            OCStackResult expectedResult, OicSecOxm_t expectedOxm);

    /**
     * All Callback Methods for Provision Manager
     */
    static void ownershipTransferCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr,
            bool hasError);

    static void provisionPairwiseCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr,
            bool hasError);

    static void provisionCredCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr, bool hasError);

    static void provisionAclCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr, bool hasError);

    static void getAclCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr, bool hasError);

    static void getCredCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr, bool hasError);

    static void unlinkDevicesCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr, bool hasError);

    static void removeDeviceCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr, bool hasError);

    static void syncDeviceCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr, bool hasError);

    static ByteArray getTrustCertChainArray();

    bool provisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId,

    const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback,
            OCStackResult expectedResult);

    bool saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize,
            OicEncodingType_t encodingType, uint16_t *credId, OCStackResult expectedResult);
    /**
     * All Utility Methods for Provision Manager
     */
    std::string getFailureMessage();

    bool convertStrToUuid(std::string uuid, OicUuid_t* deviceID, OCStackResult expectedResult);
};
#endif
