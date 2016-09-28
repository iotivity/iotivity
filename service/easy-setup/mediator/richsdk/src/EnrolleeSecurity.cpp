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
#include "provisioningdatabasemanager.h"
#include "oic_string.h"
#include "utlist.h"
#include "srmutility.h"

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
        #define DISCOVERY_TIMEOUT (10)

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

        std::shared_ptr< OC::OCSecureResource > EnrolleeSecurity::findEnrolleeSecurityResource(
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

        void EnrolleeSecurity::convertUUIDToString(const uint8_t uuid[UUID_SIZE],
                                                              std::string& uuidString)
        {
            char uuidArray[UUID_STRING_SIZE] = {'\0',};
            int ret = snprintf(uuidArray, UUID_STRING_SIZE,
                    "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                    uuid[0], uuid[1], uuid[2], uuid[3],
                    uuid[4], uuid[5], uuid[6], uuid[7],
                    uuid[8], uuid[9], uuid[10], uuid[11],
                    uuid[12], uuid[13], uuid[14], uuid[15]
                    );

            if (ret != UUID_STRING_SIZE - 1)
            {
                return;
            }

            uuidString = uuidArray;
        }

        void EnrolleeSecurity::ownershipTransferCb(OC::PMResultList_t *result, int hasError)
        {
            if (hasError)
            {
                OIC_LOG(ERROR, ENROLEE_SECURITY_TAG,"Error!!! in OwnershipTransfer");

                std::string uuid;
                convertUUIDToString(result->at(0).deviceId.id, uuid);
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
                    convertUUIDToString(result->at(0).deviceId.id, uuid);

                    OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "UUID : %s",uuid.c_str());
                    std::shared_ptr< SecProvisioningStatus > securityProvisioningStatus =
                            std::make_shared< SecProvisioningStatus >(uuid, ES_OK);
                    m_securityProvStatusCb(securityProvisioningStatus);
                    return;
                }

                delete result;
            }
        }

        void EnrolleeSecurity::provisionOwnership()
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
                std::shared_ptr< OC::OCSecureResource > ownedDevice =
                    findEnrolleeSecurityResource(pOwnedDevList);

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

                m_unownedDevice = findEnrolleeSecurityResource(pUnownedDevList);
                if (m_unownedDevice)
                {
                    if(isOwnedDeviceRegisteredInSVRDB())
                    {
                        OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG,
                            "Found Unowned device's DevID at DB of ownedDevices list");

                        OC::ResultCallBack removeDeviceWithUuidCB = std::bind(
                                &EnrolleeSecurity::removeDeviceWithUuidCB,
                                this, std::placeholders::_1, std::placeholders::_2);

                        OCSecure::removeDeviceWithUuid(DISCOVERY_TIMEOUT,
                                                       m_ocResource->sid(),
                                                       removeDeviceWithUuidCB);
                    }
                    else
                    {
                        performOwnershipTransfer();
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

        void EnrolleeSecurity::performOwnershipTransfer()
        {
            OCStackResult result = OC_STACK_ERROR;

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
                OIC_LOG(ERROR, ENROLEE_SECURITY_TAG, "doOwnershipTransfer is failed");
                throw ESPlatformException(result);
            }
        }

        void EnrolleeSecurity::removeDeviceWithUuidCB(OC::PMResultList_t *result, int hasError)
        {
            if (hasError)
            {
               OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "Error in removeDeviceWithUuid operation!");
               throw ESException("removeDeviceWithUuid Error");
            }

            else
            {
               OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "Received provisioning results: ");

               for (unsigned int i = 0; i < result->size(); i++)
               {
                    std::string uuid;
                    convertUUIDToString(result->at(i).deviceId.id, uuid);

                    OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG,
                        "Result is = %d for device %s",  result->at(i).res, uuid.c_str());
               }
               performOwnershipTransfer();
            }
        }

        bool EnrolleeSecurity::isOwnedDeviceRegisteredInSVRDB()
        {
            OCStackResult res = OC_STACK_ERROR;

            OCUuidList_t *uuidList = NULL;
            size_t numOfDevices = 0;

            res = PDMGetOwnedDevices(&uuidList, &numOfDevices);
            if (OC_STACK_OK != res)
            {
                OIC_LOG(ERROR, ENROLEE_SECURITY_TAG, "Error while getting info from DB");
                return false;
            }

            OCUuidList_t *pUuidList = uuidList;

            while (pUuidList)
            {
                std::string uuid;
                convertUUIDToString(pUuidList->dev.id, uuid);
                OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG,
                    "m_ocResource->sid(): %s, cur DB UUID %s",
                    m_ocResource->sid().c_str(), uuid.c_str());
                if(m_ocResource->sid() == uuid.c_str())
                {
                    return true;
                }
                pUuidList = pUuidList->next;
            }
            return false;
        }


        std::string EnrolleeSecurity::getUUID() const
        {
            return m_ocResource->sid();
        };

#if defined(__WITH_DTLS__) && defined(__WITH_TLS__)
        void EnrolleeSecurity::provisionSecurityForCloudServer(
            std::string cloudUuid, int credId)
        {
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
                ownedDevice = findEnrolleeSecurityResource(pOwnedDevList);

                if (!ownedDevice)
                {
                    OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "Not found matched owned device.");
                    throw ESException("Not found matched owned device.");
                }
            }
            else
            {
                OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "Not found owned devices.");
                throw ESException("Not found owned devices.");
            }

            if(!cloudUuid.empty()
                && performACLProvisioningForCloudServer(ownedDevice, cloudUuid) != ESResult::ES_OK)
            {
                OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "error performACLProvisioningForCloudServer");
                throw ESException("error performACLProvisioningForCloudServer");
            }

            if(credId != -1
                && performCertProvisioningForCloudServer(ownedDevice, credId) != ESResult::ES_OK)
            {
                OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "error performCertProvisioningForCloudServer");
                throw ESException("error performCertProvisioningForCloudServer");
            }
        }

        ESResult EnrolleeSecurity::performCertProvisioningForCloudServer(
            std::shared_ptr< OC::OCSecureResource > ownedDevice, int credId)
        {
            ESResult res = ESResult::ES_ERROR;

            if(!ownedDevice)
            {
                OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "Invalid param");
                return res;
            }
            OC::ResultCallBack CertProvisioningCb = std::bind(
                            &EnrolleeSecurity::CertProvisioningCb, this, std::placeholders::_1,
                            std::placeholders::_2);
            OCStackResult rst = ownedDevice->provisionTrustCertChain(SIGNED_ASYMMETRIC_KEY,
                                                                    static_cast<uint16_t>(credId),
                                                                    CertProvisioningCb);
            if(OC_STACK_OK != rst)
            {
                OIC_LOG_V(ERROR, ENROLEE_SECURITY_TAG, "provisionTrustCertChain error: %d", rst);
                return res;
            }

            std::unique_lock<std::mutex> lck(m_mtx);
            m_cond.wait_for(lck, std::chrono::seconds(ES_SEC_DISCOVERY_TIMEOUT));

            if(certResult)
            {
                res = ESResult::ES_OK;
            }

            return res;
        }

        ESResult EnrolleeSecurity::performACLProvisioningForCloudServer(
            std::shared_ptr< OC::OCSecureResource > ownedDevice, std::string& cloudUuid)
        {
            ESResult res = ESResult::ES_ERROR;

            if(!ownedDevice)
            {
                OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "Invalid param");
                return res;
            }
            if(cloudUuid.empty())
            {
                OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "Invalid param");
                return res;
            }

            OicUuid_t uuid;
            ConvertStrToUuid(cloudUuid.c_str(), &uuid);

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
            rsrc->interfaceLen = 1;
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
               OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "Error in ACL provisioning operation!");
               aclResult = false;
            }
            else
            {
               OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "Received ACL provisioning results: ");

               std::string devUuid;
               for (unsigned int i = 0; i < result->size(); i++)
               {
                   convertUUIDToString(result->at(i).deviceId.id, devUuid);
                   OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "Result is = %d  for device %s",
                                                           result->at(i).res, devUuid.c_str());
               }
               delete result;
               aclResult = true;
            }
            m_cond.notify_all();
        }

        void EnrolleeSecurity::CertProvisioningCb(PMResultList_t *result, int hasError)
        {
            if (hasError)
            {
               OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "Error in Cert. provisioning operation!");
               certResult = false;
            }
            else
            {
               OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "Received Cert. provisioning results: ");

               std::string devUuid;
               for (unsigned int i = 0; i < result->size(); i++)
               {
                   convertUUIDToString(result->at(i).deviceId.id, devUuid);
                   OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "Result is = %d  for device %s",
                                                           result->at(i).res, devUuid.c_str());
               }
               delete result;
               certResult= true;
            }
            m_cond.notify_all();
        }
#endif //defined(__WITH_DTLS__) && defined(__WITH_TLS__)
    }
}
