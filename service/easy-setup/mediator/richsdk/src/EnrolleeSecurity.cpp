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

        // TODO : Currently discovery timeout for owned and unowned devices is fixed as 5
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
            OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "ownershipTransferCb IN");
            if (hasError)
            {
                OIC_LOG_V(ERROR, ENROLEE_SECURITY_TAG, "OwnershipTransfer is failed with code(%d)", hasError);
                OTMResult = false;
            }
            else
            {
                OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "OwnershipTransfer is succeeded");
                for (unsigned int i = 0; i < result->size(); i++)
                {
                    OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "Result is = %d for device", result->at(i).res);
                }
                delete result;
                OTMResult = true;
            }
            m_cond.notify_all();
        }

        ESResult EnrolleeSecurity::provisionOwnership()
        {
            OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "provisionOwnership IN");

            ESResult res = ESResult::ES_ERROR;

            OCStackResult result = OC_STACK_ERROR;
            OicUuid_t uuid;
            if(OC_STACK_OK != ConvertStrToUuid(m_ocResource->sid().c_str(), &uuid))
            {
                OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "Convert to uuid from deviceID failed.");
                return res;
            }

            result = OCSecure::discoverSingleDevice(ES_SEC_DISCOVERY_TIMEOUT,
                                                    &uuid,
                                                    m_securedResource);
            if (result != OC_STACK_OK)
            {
                OIC_LOG(ERROR, ENROLEE_SECURITY_TAG, "Secure Resource Discovery failed.");
                res = ESResult:: ES_SECURE_RESOURCE_DISCOVERY_FAILURE;
                return res;
            }
            else if (m_securedResource)
            {
                OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "Secured resource is found.");
                OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "HOST: %s", m_securedResource->getDevAddr().c_str());
                OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "SID: %s", m_securedResource->getDeviceID().c_str());
                OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "Owned status: %d", m_securedResource->getOwnedStatus());

                if (m_securedResource->getOwnedStatus()) // owned check logic
                {
                    if(isOwnedDeviceRegisteredInSVRDB())
                    {
                        OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG,
                                "The found device is already owned by Mediator.(SUCCESS)");
                        res = ESResult::ES_OK;
                    }
                    else
                    {
                        OIC_LOG(ERROR, ENROLEE_SECURITY_TAG, "The found device is not one in SVR DB");
                        res = ESResult::ES_ERROR;
                    }
                    return res;
                }
                else // unowned check logic
                {
                    if(isOwnedDeviceRegisteredInSVRDB())
                    {
                        OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG,
                            "Found Unowned device's DevID at DB of ownedDevices list");

                        OC::ResultCallBack removeDeviceWithUuidCB = std::bind(
                                &EnrolleeSecurity::removeDeviceWithUuidCB,
                                this, std::placeholders::_1, std::placeholders::_2);

                        result = OCSecure::removeDeviceWithUuid(ES_SEC_DISCOVERY_TIMEOUT,
                                                                m_ocResource->sid(),
                                                                removeDeviceWithUuidCB);
                        if(result != OC_STACK_OK)
                        {
                            OIC_LOG_V(ERROR, ENROLEE_SECURITY_TAG, "removeDeviceWithUuid failed. (%d)", result);
                            res = ESResult::ES_OWNERSHIP_TRANSFER_FAILURE;
                            return res;
                        }

                        std::unique_lock<std::mutex> lck(m_mtx);
                        m_cond.wait_for(lck, std::chrono::seconds(ES_SEC_DISCOVERY_TIMEOUT));

                        if(!removeDeviceResult)
                        {
                            OIC_LOG(ERROR, ENROLEE_SECURITY_TAG, "Removing device is failed.");
                            res = ESResult::ES_OWNERSHIP_TRANSFER_FAILURE;
                            return res;
                        }
                        OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "Removing device is succeeded.");
                    }

                    res = performOwnershipTransfer();

                    if(res != ESResult::ES_OK)
                    {
                        OIC_LOG_V(ERROR, ENROLEE_SECURITY_TAG, "Ownership-Transfer failed. (%d)", res);
                        res = ESResult::ES_OWNERSHIP_TRANSFER_FAILURE;
                        return res;
                    }

                    std::unique_lock<std::mutex> lck(m_mtx);
                    m_cond.wait(lck);

                    if(!OTMResult)
                    {
                        OIC_LOG(ERROR, ENROLEE_SECURITY_TAG, "Ownership-Transfer failed.");
                        res = ESResult::ES_OWNERSHIP_TRANSFER_FAILURE;
                    }
                }
            }
            else
            {
                OIC_LOG(ERROR, ENROLEE_SECURITY_TAG, "No secure resource is found.");
                res = ESResult:: ES_SECURE_RESOURCE_DISCOVERY_FAILURE;
            }
            return res;
        }

        ESResult EnrolleeSecurity::performOwnershipTransfer()
        {
            OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "performOwnershipTransfer IN.");

            OCStackResult result = OC_STACK_ERROR;

            OTMCallbackData_t justWorksCBData;
            justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
            justWorksCBData.createSecureSessionCB = CreateSecureSessionJustWorksCallback;
            justWorksCBData.createSelectOxmPayloadCB = CreateJustWorksSelectOxmPayload;
            justWorksCBData.createOwnerTransferPayloadCB =
                    CreateJustWorksOwnerTransferPayload;
            OCSecure::setOwnerTransferCallbackData(OIC_JUST_WORKS, &justWorksCBData, NULL);

            OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "Transfering ownership for : %s ",
                    m_securedResource->getDeviceID().c_str());

            OC::ResultCallBack ownershipTransferCb = std::bind(
                    &EnrolleeSecurity::ownershipTransferCb, this, std::placeholders::_1,
                    std::placeholders::_2);

            result = m_securedResource->doOwnershipTransfer(ownershipTransferCb);
            if (result != OC_STACK_OK)
            {
                OIC_LOG(ERROR, ENROLEE_SECURITY_TAG, "doOwnershipTransfer is failed");
                return ESResult::ES_ERROR;
            }
            return ESResult::ES_OK;
        }

        void EnrolleeSecurity::removeDeviceWithUuidCB(OC::PMResultList_t *result, int hasError)
        {
            OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "removeDeviceWithUuidCB IN");

            if (hasError)
            {
               OIC_LOG_V(ERROR, ENROLEE_SECURITY_TAG, "removeDeviceWithUuid is failed with code (%d)", hasError);
               removeDeviceResult = false;
            }

            else
            {
               OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "removeDeviceWithUuid is succeeded");

               for (unsigned int i = 0; i < result->size(); i++)
               {
                    std::string uuid;
                    convertUUIDToString(result->at(i).deviceId.id, uuid);

                    OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG,
                        "Result is = %d for device %s",  result->at(i).res, uuid.c_str());
               }
               removeDeviceResult = true;
            }
            m_cond.notify_all();
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
                    OICFree(uuidList);
                    return true;
                }
                pUuidList = pUuidList->next;
            }
            OICFree(uuidList);
            return false;
        }


        std::string EnrolleeSecurity::getUUID() const
        {
            return m_ocResource->sid();
        };

#if defined(__WITH_DTLS__) && defined(__WITH_TLS__)
        ESResult EnrolleeSecurity::provisionSecurityForCloudServer(
            std::string cloudUuid, int credId)
        {
            OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "provisionSecurityForCloudServer IN");

            ESResult res = ESResult::ES_ERROR;

            // Need to discover Owned device in a given network, again
            std::shared_ptr< OC::OCSecureResource > ownedDevice = NULL;

            OCStackResult result;
            OicUuid_t uuid;
            if(OC_STACK_OK != ConvertStrToUuid(m_ocResource->sid().c_str(), &uuid))
            {
                OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "Convert to uuid from deviceID failed.");
                return res;
            }


            result = OCSecure::discoverSingleDevice(ES_SEC_DISCOVERY_TIMEOUT,
                                                    &uuid,
                                                    ownedDevice);
            if (result != OC_STACK_OK)
            {
                OIC_LOG(ERROR, ENROLEE_SECURITY_TAG, "secureResource Discovery failed.");
                res = ESResult::ES_SECURE_RESOURCE_DISCOVERY_FAILURE;
                return res;
            }
            else if (ownedDevice)
            {
                OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "Secured resource is found.");
                OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "HOST: %s", ownedDevice->getDevAddr().c_str());
                OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "SID: %s", ownedDevice->getDeviceID().c_str());
                OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "Owned status: %d", ownedDevice->getOwnedStatus());

                if (ownedDevice->getOwnedStatus())
                {
                    if(!isOwnedDeviceRegisteredInSVRDB())
                    {
                        OIC_LOG(ERROR, ENROLEE_SECURITY_TAG, "The found device is not one in SVR DB");
                        res = ESResult::ES_SECURE_RESOURCE_DISCOVERY_FAILURE;
                        return res;
                    }
                }
                else
                {
                    OIC_LOG(ERROR, ENROLEE_SECURITY_TAG, "The found device is unowned.");
                    OIC_LOG(ERROR, ENROLEE_SECURITY_TAG, "Ownerthip transfer is required.");

                    res = ESResult::ES_SECURE_RESOURCE_DISCOVERY_FAILURE;
                    return res;
                }
            }
            else
            {
                OIC_LOG(ERROR, ENROLEE_SECURITY_TAG, "No secure resource is found");
                res = ESResult::ES_SECURE_RESOURCE_DISCOVERY_FAILURE;
                return res;
            }

            if(cloudUuid.empty())
            {
                OIC_LOG(ERROR, ENROLEE_SECURITY_TAG,
                         "ACL provisioning is skipped due to empty UUID of cloud server");
            }
            else
            {
                res = performACLProvisioningForCloudServer(ownedDevice, cloudUuid);
                if(res != ESResult::ES_OK)
                {
                    OIC_LOG(ERROR, ENROLEE_SECURITY_TAG, "error performACLProvisioningForCloudServer");
                    return res;
                }
            }

            if(credId < 1)
            {
                OIC_LOG(ERROR, ENROLEE_SECURITY_TAG,
                         "Cert. provisioning is skipped due to wrong cred ID (<1)");
            }
            else
            {
                res = performCertProvisioningForCloudServer(ownedDevice, credId);
                if(res != ESResult::ES_OK)
                {
                    OIC_LOG(ERROR, ENROLEE_SECURITY_TAG, "error performCertProvisioningForCloudServer");
                    return res;
                }
            }

            return res;
        }

        ESResult EnrolleeSecurity::performCertProvisioningForCloudServer(
            std::shared_ptr< OC::OCSecureResource > ownedDevice, int credId)
        {
            OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "performCertProvisioningForCloudServer IN");

            ESResult res = ESResult::ES_CERT_PROVISIONING_FAILURE;

            if(!ownedDevice)
            {
                OIC_LOG(ERROR, ENROLEE_SECURITY_TAG, "Given ownedDevice is null");
                return res;
            }

            OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "Given CredId: %d", credId);

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
            m_cond.wait(lck);

            if(certResult)
            {
                res = ESResult::ES_OK;
            }

            return res;
        }

        ESResult EnrolleeSecurity::performACLProvisioningForCloudServer(
            std::shared_ptr< OC::OCSecureResource > ownedDevice, std::string& cloudUuid)
        {
            OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "performACLProvisioningForCloudServer IN");

            ESResult res = ESResult::ES_ACL_PROVISIONING_FAILURE;

            if(!ownedDevice)
            {
                OIC_LOG(ERROR, ENROLEE_SECURITY_TAG, "Given ownedDevice is null");
                return res;
            }

            OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "Given cloudUuid: %s", cloudUuid.c_str());

            OicUuid_t uuid;
            if(OC_STACK_OK != ConvertStrToUuid(cloudUuid.c_str(), &uuid))
            {
                OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "Convert to uuid from deviceID failed.");
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
            m_cond.wait(lck);

            if(aclResult)
            {
                res = ESResult::ES_OK;
            }

            return res;
        }

        OicSecAcl_t* EnrolleeSecurity::createAcl(const OicUuid_t cloudUuid)
        {
            OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "creatAcl IN");

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

            OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "creatAcl OUT");

            return acl;
        }

        void EnrolleeSecurity::ACLProvisioningCb(PMResultList_t *result, int hasError)
        {
            OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "ACLProvisioningCb IN");

            if (hasError)
            {
               OIC_LOG_V(ERROR, ENROLEE_SECURITY_TAG, "ACL provisioning is failed with code (%d)", hasError);
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
            OIC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "CertProvisioningCb IN");

            if (hasError)
            {
               OIC_LOG_V(ERROR, ENROLEE_SECURITY_TAG, "Cert provisioning is failed with code (%d)", hasError);
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
