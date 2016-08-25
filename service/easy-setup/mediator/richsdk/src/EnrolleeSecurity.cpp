//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

#include "base64.h"

#include "EnrolleeSecurity.h"
#include "oxmjustworks.h"
#include "oxmrandompin.h"
#include "EnrolleeResource.h"
#include "logger.h"
#include "ESException.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "utlist.h"

namespace OIC
{
    namespace Service
    {
        #define MAX_PERMISSION_LENGTH (5)
        #define CREATE (1)
        #define READ (2)
        #define UPDATE (4)
        #define DELETE (8)
        #define NOTIFY (16)
        #define DASH '-'

        //TODO : Currently discovery timeout for owned and unowned devices is fixed as 5
        // The value should be accepted from the application as a parameter during ocplatform
        // config call
        #define ES_SEC_DISCOVERY_TIMEOUT 5

        EnrolleeSecurity::EnrolleeSecurity(
            std::shared_ptr< OC::OCResource > resource,
            const std::string secDbPath)
        {
            (void) secDbPath;
            m_ocResource = resource;
        }

        void EnrolleeSecurity::registerCallbackHandler(
            const SecurityProvStatusCb securityProvStatusCb,
            const SecurityPinCb securityPinCb,
            const SecProvisioningDbPathCb secProvisioningDbPathCb)
        {
            m_securityProvStatusCb = securityProvStatusCb;
            m_securityPinCb = securityPinCb;
            m_secProvisioningDbPathCb = secProvisioningDbPathCb;
        }

        std::shared_ptr< OC::OCSecureResource > EnrolleeSecurity::getEnrollee(
            DeviceList_t &list)
        {
            for (unsigned int i = 0; i < list.size(); i++)
            {
                if(m_ocResource->sid() == list[i]->getDeviceID().c_str())
                {
                    OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "Device %d ID %s ", i + 1,
                            list[i]->getDeviceID().c_str());
                    OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "From IP :%s",
                                                            list[i]->getDevAddr().c_str());
                    return list[i];
                }
            }
            OIC_LOG(ERROR, ENROLEE_SECURITY_TAG,"Error!!! DeviceList_t is NULL");
            return NULL;
        }

        void EnrolleeSecurity::convertUUIDToString(const OicUuid_t uuid,
                                                              std::string& uuidString)
        {
            char base64Buff[B64ENCODE_OUT_SAFESIZE(sizeof(((OicUuid_t*) 0)->id)) + 1] =
            { 0, };
            uint32_t outLen = 0;
            B64Result b64Ret = B64_OK;
            std::ostringstream deviceId("");

            b64Ret = b64Encode(uuid.id, sizeof(uuid.id),
                    base64Buff, sizeof(base64Buff), &outLen);

            if (B64_OK == b64Ret)
            {
                deviceId << base64Buff;
            }
            uuidString =  deviceId.str();
        }

        void EnrolleeSecurity::convertStringToUUID(OicUuid_t& uuid,
                                                              const std::string uuidString)
        {
            size_t outBufSize = B64DECODE_OUT_SAFESIZE((uuidString.length() + 1));
            uint8_t* outKey = (uint8_t*)OICCalloc(1, outBufSize);
            uint32_t outKeySize = 0;
            if(NULL == outKey)
            {
                OIC_LOG (ERROR, ENROLEE_SECURITY_TAG, "Failed to memoray allocation.");
                throw ESBadRequestException ("Failed to memoray allocation.");
            }

            if(B64_OK == b64Decode((char*)uuidString.c_str(),
                                    uuidString.length(),
                                    outKey,
                                    outBufSize,
                                    &outKeySize))
            {
                memcpy(uuid.id, outKey, outKeySize);
            }
            else
            {
                OIC_LOG (ERROR, ENROLEE_SECURITY_TAG, "Failed to base64 decoding.");
                throw ESBadRequestException ("Failed to base64 decoding.");
            }

            OICFree(outKey);
        }

        void EnrolleeSecurity::ownershipTransferCb(OC::PMResultList_t *result, int hasError)
        {
            if (hasError)
            {
                OIC_LOG(ERROR, ENROLEE_SECURITY_TAG,"Error!!! in OwnershipTransfer");

                std::string uuid;
                convertUUIDToString(result->at(0).deviceId, uuid);
                std::shared_ptr< SecProvisioningStatus > securityProvisioningStatus =
                        std::make_shared< SecProvisioningStatus >(uuid, ES_ERROR);
                m_securityProvStatusCb(securityProvisioningStatus);
                return;
            }
            else
            {
                OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "ownershipTransferCb : Received provisioning results: ");
                for (unsigned int i = 0; i < result->size(); i++)
                {
                    OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "Result is = %d for device",result->at(i).res);
                    std::string uuid;
                    convertUUIDToString(result->at(0).deviceId, uuid);

                    OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "UUID : %s",uuid.c_str());
                    std::shared_ptr< SecProvisioningStatus > securityProvisioningStatus =
                            std::make_shared< SecProvisioningStatus >(uuid, ES_OK);
                    m_securityProvStatusCb(securityProvisioningStatus);
                    return;
                }

                delete result;
            }
        }

        void EnrolleeSecurity::performOwnershipTransfer()
        {
            OC::DeviceList_t pUnownedDevList, pOwnedDevList;

            pOwnedDevList.clear();
            pUnownedDevList.clear();

            OCStackResult result = OC_STACK_ERROR;

            result = OCSecure::discoverOwnedDevices(ES_SEC_DISCOVERY_TIMEOUT,
                    pOwnedDevList);
            if (result != OC_STACK_OK)
            {
                OIC_LOG(ERROR, ENROLEE_SECURITY_TAG, "Owned Discovery failed.");
                //Throw exception
                throw ESPlatformException(result);
            }
            else if (pOwnedDevList.size())
            {
                OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "Found owned devices. Count =%d",
                        pOwnedDevList.size());
                std::shared_ptr< OC::OCSecureResource > ownedDevice = getEnrollee(pOwnedDevList);

                if (ownedDevice)
                {
                    std::shared_ptr< SecProvisioningStatus > securityProvisioningStatus =
                            std::make_shared< SecProvisioningStatus >(ownedDevice->getDeviceID(), ES_OK);
                    m_securityProvStatusCb(securityProvisioningStatus);
                    return;
                }
            }

            result = OCSecure::discoverUnownedDevices(ES_SEC_DISCOVERY_TIMEOUT, pUnownedDevList);
            if (result != OC_STACK_OK)
            {
                OIC_LOG(ERROR, ENROLEE_SECURITY_TAG, "UnOwned Discovery failed.");
                //Throw exception
                throw ESPlatformException(result);
            }
            else if (pUnownedDevList.size())
            {
                OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "Found Unowned devices. Count =%d",
                        pUnownedDevList.size());

                m_unownedDevice = getEnrollee(pUnownedDevList);
                if (m_unownedDevice)
                {
                    OTMCallbackData_t justWorksCBData;
                    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
                    justWorksCBData.createSecureSessionCB = CreateSecureSessionJustWorksCallback;
                    justWorksCBData.createSelectOxmPayloadCB = CreateJustWorksSelectOxmPayload;
                    justWorksCBData.createOwnerTransferPayloadCB =
                            CreateJustWorksOwnerTransferPayload;
                    OCSecure::setOwnerTransferCallbackData(OIC_JUST_WORKS, &justWorksCBData, NULL);

                    OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "Transfering ownership for : %s ",
                            m_unownedDevice->getDeviceID().c_str());

                    OC::ResultCallBack ownershipTransferCb = std::bind(
                            &EnrolleeSecurity::ownershipTransferCb, this, std::placeholders::_1,
                            std::placeholders::_2);

                    result = m_unownedDevice->doOwnershipTransfer(ownershipTransferCb);
                    if (result != OC_STACK_OK)
                    {
                        OIC_LOG(ERROR, ENROLEE_SECURITY_TAG, "OwnershipTransferCallback is failed");
                        throw ESPlatformException(result);
                    }
                }
                else
                {
                    OIC_LOG(ERROR, ENROLEE_SECURITY_TAG, "No matched unowned devices found.");
                    throw ESException("No matched unowned devices found.");
                }
            }
            else
            {
                OIC_LOG(ERROR, ENROLEE_SECURITY_TAG, "No unowned devices found.");
                throw ESException("No unowned devices found.");
            }
        }

        ESResult EnrolleeSecurity::performACLProvisioningForCloudServer(std::string cloudUuid)
        {
            ESResult res = ESResult::ES_ERROR;

            OicUuid_t uuid;
            convertStringToUUID(uuid, cloudUuid);

            // Need to discover Owned device in a given network, again
            OC::DeviceList_t pOwnedDevList;
            std::shared_ptr< OC::OCSecureResource > ownedDevice = NULL;

            pOwnedDevList.clear();

            OCStackResult result;

            result = OCSecure::discoverOwnedDevices(ES_SEC_DISCOVERY_TIMEOUT,
                    pOwnedDevList);
            if (result != OC_STACK_OK)
            {
                OIC_LOG(ERROR, ENROLEE_SECURITY_TAG, "Owned Discovery failed.");
                //Throw exception
                throw ESPlatformException(result);
            }
            else if (pOwnedDevList.size())
            {
                OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "Found owned devices. Count =%d",
                        pOwnedDevList.size());
                ownedDevice = getEnrollee(pOwnedDevList);

                if (!ownedDevice)
                {
                    OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "Not found owned devices.");
                    return res;
                }
            }
            else
            {
                OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "Not found owned devices.");
                return res;
            }

            // Create Acl for Cloud Server to be provisioned to Enrollee
            OicSecAcl_t* acl = createAcl(uuid);
            if(!acl)
            {
                OIC_LOG(ERROR, ENROLEE_SECURITY_TAG, "createAcl error return");
                return res;
            }

            OC::ResultCallBack aclProvisioningCb = std::bind(
                            &EnrolleeSecurity::ACLProvisioningCb, this, std::placeholders::_1,
                            std::placeholders::_2);
            // ACL provisioning to Enrollee
            OCStackResult rst = ownedDevice->provisionACL(acl, aclProvisioningCb);
            if(OC_STACK_OK != rst)
            {
                OIC_LOG_V(ERROR, ENROLEE_SECURITY_TAG, "OCProvisionACL API error: %d", rst);
                return res;
            }

            std::unique_lock<std::mutex> lck(m_mtx);
            m_cond.wait_for(lck, std::chrono::seconds(ES_SEC_DISCOVERY_TIMEOUT));

            if(aclResult)
            {
                res = ESResult::ES_OK;
            }

            return res;
        }

        std::string EnrolleeSecurity::getUUID() const
        {
            return m_ocResource->sid();
        };

        OicSecAcl_t* EnrolleeSecurity::createAcl(const OicUuid_t cloudUuid)
        {
            // allocate memory for |acl| struct
            OicSecAcl_t* acl = (OicSecAcl_t*) OICCalloc(1, sizeof(OicSecAcl_t));
            if(!acl)
            {
                OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "createAcl: OICCalloc error return");
                return NULL;  // not need to 'goto' |ERROR| before allocating |acl|
            }
            OicSecAce_t* ace = (OicSecAce_t*) OICCalloc(1, sizeof(OicSecAce_t));
            if(!ace)
            {
                OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG,  "createAcl: OICCalloc error return");
                return NULL;  // not need to 'goto' |ERROR| before allocating |acl|
            }
            LL_APPEND(acl->aces, ace);

            memcpy(&ace->subjectuuid, &cloudUuid, UUID_LENGTH);

            OicSecRsrc_t* rsrc = (OicSecRsrc_t*)OICCalloc(1, sizeof(OicSecRsrc_t));
            if(!rsrc)
            {
                OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "createAcl: OICCalloc error return");
                OCDeleteACLList(acl);
                return NULL;
            }

            char href[] = "*";
            size_t len = strlen(href)+1;  // '1' for null termination
            rsrc->href = (char*) OICCalloc(len, sizeof(char));
            if(!rsrc->href)
            {
                OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG,  "createAcl: OICCalloc error return");
                OCDeleteACLList(acl);
                return NULL;
            }
            OICStrcpy(rsrc->href, len, href);

            size_t arrLen = 1;
            rsrc->typeLen = arrLen;
            rsrc->types = (char**)OICCalloc(arrLen, sizeof(char*));
            rsrc->interfaces = (char**)OICCalloc(arrLen, sizeof(char*));
            rsrc->types[0] = OICStrdup("rt");   // ignore
            rsrc->interfaces[0] = OICStrdup("if");  // ignore

            LL_APPEND(ace->resources, rsrc);

            ace->permission = 31;   // R/W/U/D

            return acl;
        }

        void EnrolleeSecurity::ACLProvisioningCb(PMResultList_t *result, int hasError)
        {
            if (hasError)
            {
               OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "Error in provisioning operation!");
               aclResult = false;
            }
            else
            {
               OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "Received provisioning results: ");

               std::string devUuid;
               for (unsigned int i = 0; i < result->size(); i++)
               {
                   convertUUIDToString(result->at(i).deviceId, devUuid);
                   OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "Result is = %d  for device %s",
                                                           result->at(i).res, devUuid.c_str());
               }
               delete result;
               aclResult = true;
            }
            m_cond.notify_all();
        }
    }
}
