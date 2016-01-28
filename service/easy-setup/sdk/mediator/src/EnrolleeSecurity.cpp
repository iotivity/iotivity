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
#include "RemoteEnrolleeResource.h"
#include "logger.h"
#include "ESException.h"
#include "oic_malloc.h"
#include "oic_string.h"

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
        std::shared_ptr< RemoteEnrolleeResource > remoteEnrolleeResource,
        std::string secDbPath)
        {
            m_enrolleeSecState = EnrolleeSecState::ES_SEC_UNKNOWN;
            m_remoteEnrolleeResource = remoteEnrolleeResource;

            //Initializing the provisioning client stack using the db path provided by the
            // application.
            // Note : If the path is NULL or empty, the PDM.db should be present in the same path.
            OCStackResult result = OCSecure::provisionInit(secDbPath);

            if (result != OC_STACK_OK)
            {
                throw ESPlatformException(result);
            }
        }

        ESResult EnrolleeSecurity::registerCallbackHandler(EnrolleeSecStatusCb enrolleeSecStatusCb,
                SecurityPinCb securityPinCb, SecProvisioningDbPathCb secProvisioningDbPathCb)

        {
            m_enrolleeSecStatusCb = enrolleeSecStatusCb;
            m_securityPinCb = securityPinCb;
            m_secProvisioningDbPathCb = secProvisioningDbPathCb;

            return ES_ERROR;
        }

        std::shared_ptr< OC::OCSecureResource > EnrolleeSecurity::findEnrollee(std::string host,
                DeviceList_t &list)
        {
            for (unsigned int i = 0; i < list.size(); i++)
            {
                OC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "Device %d ID %s ", i + 1,
                        list[i]->getDeviceID().c_str());OC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "From IP :%s", list[i]->getDevAddr().c_str());

                if (list[i]->getDevAddr() == host)
                {
                    return list[i];
                }
            }

            return nullptr;
        }

        void EnrolleeSecurity::convertUUIDToString(OicUuid_t uuid, std::string& uuidString)
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

        void EnrolleeSecurity::ownershipTransferCb(OC::PMResultList_t *result, int hasError)
        {
            if (hasError)
            {
                OC_LOG(ERROR, ENROLEE_SECURITY_TAG,"Error!!! in OwnershipTransfer");

                std::shared_ptr< SecProvisioningResult > securityProvisioningStatus = nullptr;
                std::string uuid;
                convertUUIDToString(result->at(0).deviceId, uuid);
                securityProvisioningStatus = std::make_shared< SecProvisioningResult >(uuid,
                        ES_ERROR);

                m_enrolleeSecStatusCb(securityProvisioningStatus);
                return;
            }
            else
            {
                OC_LOG(DEBUG, ENROLEE_SECURITY_TAG,"Transferred Ownership successfuly for device : ");
                std::string uuid;
                convertUUIDToString(result->at(0).deviceId, uuid);
                OC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "UUID : %s",uuid.c_str());

                //TODO : Decide if we have to manage the owned/unowned devices.
                //pOwnedDevList.push_back(pUnownedDevList[transferDevIdx]);
                //pUnownedDevList.erase(pUnownedDevList.begin() + transferDevIdx);

                OC_LOG(DEBUG, ENROLEE_SECURITY_TAG,
                        "Ownership transfer success success. Continuing with provisioning ACL");

                if (provisionAcl() == ES_ERROR)
                {
                    OC_LOG(ERROR, ENROLEE_SECURITY_TAG,"Error!!! in provisionAcl");
                    std::shared_ptr< SecProvisioningResult > securityProvisioningStatus = nullptr;
                    std::string uuid;
                    convertUUIDToString(result->at(0).deviceId, uuid);
                    securityProvisioningStatus = std::make_shared< SecProvisioningResult >(uuid,
                            ES_ERROR);
                }

                delete result;
            }
        }

        /**
         * Callback function for provisioning ACL, Credentials.
         *
         * @param[in]    result Result list
         * @param[in] hasError indicates if the result has error
         */
        void EnrolleeSecurity::provisionCb(OC::PMResultList_t *result, int hasError)
        {
            if (hasError)
            {
                OC_LOG(ERROR, ENROLEE_SECURITY_TAG, "provisionCb : Error in provisioning operation!");
            }
            else
            {
                OC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "provisionCb : Received provisioning results: ");
                for (unsigned int i = 0; i < result->size(); i++)
                {
                    OC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "Result is = %d for device",result->at(i).res);
                    std::string uuid;
                    convertUUIDToString(result->at(0).deviceId, uuid);

                    OC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "UUID : %s",uuid.c_str());
                    std::shared_ptr< SecProvisioningResult > securityProvisioningStatus = nullptr;
                    securityProvisioningStatus = std::make_shared< SecProvisioningResult >(uuid,
                            ES_OK);

                    m_enrolleeSecStatusCb(securityProvisioningStatus);
                    return;
                }

                delete result;
            }
        }

        /**
         * Calculate ACL permission from string to bit
         *
         * @param[in] temp_psm    Input data of ACL permission string
         * @param[in,out] pms    The pointer of ACL permission value
         * @return  0 on success otherwise -1.
         */
        int EnrolleeSecurity::CalculateAclPermission(const char *temp_pms, uint16_t *pms)
        {
            int i = 0;

            if (NULL == temp_pms || NULL == pms)
            {
                return -1;
            }
            *pms = 0;
            while (temp_pms[i] != '\0')
            {
                switch (temp_pms[i])
                {
                    case 'C':
                        {
                            *pms += CREATE;
                            i++;
                            break;
                        }
                    case 'R':
                        {
                            *pms += READ;
                            i++;
                            break;
                        }
                    case 'U':
                        {
                            *pms += UPDATE;
                            i++;
                            break;
                        }
                    case 'D':
                        {
                            *pms += DELETE;
                            i++;
                            break;
                        }
                    case 'N':
                        {
                            *pms += NOTIFY;
                            i++;
                            break;
                        }
                    case '_':
                        {
                            i++;
                            break;
                        }
                    default:
                        {
                            return -1;
                        }
                }
            }
            return 0;
        }

        EasySetupState EnrolleeSecurity::performOwnershipTransfer()
        {
            EasySetupState ownershipStatus = DEVICE_NOT_OWNED;

            OC::DeviceList_t pUnownedDevList, pOwnedDevList;

            pOwnedDevList.clear();
            pUnownedDevList.clear();

            OCStackResult result;

            result = OCSecure::discoverOwnedDevices(ES_SEC_DISCOVERY_TIMEOUT,
                    pOwnedDevList);
            if (result != OC_STACK_OK)
            {
                OC_LOG(ERROR, ENROLEE_SECURITY_TAG, "Owned Discovery failed.");
                ownershipStatus = DEVICE_NOT_OWNED;
                //Throw exception
                throw ESPlatformException(result);
                return ownershipStatus;
            }
            else if (pOwnedDevList.size())
            {
                OC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "Found owned devices. Count =%d",
                        pOwnedDevList.size());
                std::shared_ptr< OC::OCSecureResource > ownedDevice =
                        findEnrollee(
                                std::string(
                                        m_remoteEnrolleeResource->m_enrolleeNWProvInfo.netAddressInfo.WIFI.ipAddress),
                                pOwnedDevList);
                if (ownedDevice)
                {
                    ownershipStatus = DEVICE_OWNED;
                    return ownershipStatus;
                }
            }
            else
            {
                OC_LOG(ERROR, ENROLEE_SECURITY_TAG, "No owned devices found.");
                ownershipStatus = DEVICE_NOT_OWNED;
            }

            result = OCSecure::discoverUnownedDevices(ES_SEC_DISCOVERY_TIMEOUT, pUnownedDevList);
            if (result != OC_STACK_OK)
            {
                OC_LOG(ERROR, ENROLEE_SECURITY_TAG, "UnOwned Discovery failed.");
                ownershipStatus = DEVICE_NOT_OWNED;
                //Throw exception
                throw ESPlatformException(result);
                return ownershipStatus;
            }
            else if (pUnownedDevList.size())
            {
                OC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "Found Unowned devices. Count =%d",
                        pUnownedDevList.size());

                m_unownedDevice =
                        findEnrollee(
                                m_remoteEnrolleeResource->m_enrolleeNWProvInfo.netAddressInfo.WIFI.ipAddress,
                                pUnownedDevList);
                if (m_unownedDevice)
                {
                    OTMCallbackData_t justWorksCBData;
                    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
                    justWorksCBData.createSecureSessionCB = CreateSecureSessionJustWorksCallback;
                    justWorksCBData.createSelectOxmPayloadCB = CreateJustWorksSelectOxmPayload;
                    justWorksCBData.createOwnerTransferPayloadCB =
                            CreateJustWorksOwnerTransferPayload;
                    OCSecure::setOwnerTransferCallbackData(OIC_JUST_WORKS, &justWorksCBData, NULL);

                    OC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "Transfering ownership for : %s ",
                            m_unownedDevice->getDeviceID().c_str());

                    OC::ResultCallBack ownershipTransferCb = std::bind(
                            &EnrolleeSecurity::ownershipTransferCb, this, std::placeholders::_1,
                            std::placeholders::_2);

                    if (m_unownedDevice->doOwnershipTransfer(ownershipTransferCb) != OC_STACK_OK)
                    {
                        OC_LOG(ERROR, ENROLEE_SECURITY_TAG, "OwnershipTransferCallback is failed");
                        ownershipStatus = DEVICE_NOT_OWNED;
                        //Throw exception
                        throw ESPlatformException(result);
                    }
                    ownershipStatus = DEVICE_NOT_OWNED;
                }
            }
            else
            {
                OC_LOG(ERROR, ENROLEE_SECURITY_TAG, "No unSecure devices found.");
                ownershipStatus = DEVICE_NOT_OWNED;

                return ownershipStatus;
            }

            return ownershipStatus;
        }

        ESResult EnrolleeSecurity::createProvisiongResourceACL(OicSecAcl_t *acl)
        {
            //TODO : Have to accept subject id of the mediator from application during easysetup
            // initialization.
            char temp_id[UUID_LENGTH+1] = {"admindeviceUUID0"};
            for (int i = 0, j = 0; temp_id[i] != '\0'; i++)
            {
                acl->subject.id[j++] = temp_id[i];
            }

            //Always resource length is 1. /oic/prov is the only resource for which needs ACL
            // permission has to be set
            acl->resourcesLen = 1;
            acl->resources = (char **) OICCalloc(acl->resourcesLen, sizeof(char *));
            if (NULL == acl->resources)
            {
                OC_LOG(ERROR, ENROLEE_SECURITY_TAG, "Error while memory allocation");
                return ES_ERROR;
            }

            char temp_rsc[MAX_URI_LENGTH] = {OC_RSRVD_ES_URI_PROV};

            for (size_t i = 0; i < acl->resourcesLen; i++)
            {
                acl->resources[i] = OICStrdup(temp_rsc);
                if (NULL == acl->resources[i])
                {
                    OC_LOG(ERROR, ENROLEE_SECURITY_TAG, "Error while memory allocation");
                    return ES_ERROR;
                }
            }
            // Set Permission
            char temp_pms[MAX_PERMISSION_LENGTH+1]={"CRUDN"};
            int ret;
            do
            {
                ret = CalculateAclPermission(temp_pms, &(acl->permission));
            } while (0 != ret);

            //TODO : Have to accept subject id of the mediator from application during easysetup
            // initialization.
            for (int i = 0, j = 0; temp_id[i] != '\0'; i++)
            {
                acl->subject.id[j++] = temp_id[i];
            }

            // Set Rowner
            acl->ownersLen = 1;//Always resource owner is only one, which is the mediator
            acl->owners = (OicUuid_t *) OICCalloc(acl->ownersLen, sizeof(OicUuid_t));
            if (NULL == acl->owners)
            {
                OC_LOG(ERROR, ENROLEE_SECURITY_TAG, "Error while memory allocation");
                return ES_ERROR;
            }
            for (size_t i = 0; i < acl->ownersLen; i++)
            {
                for (int k = 0, j = 0; temp_id[k] != '\0'; k++)
                {
                    acl->owners[i].id[j++] = temp_id[k];
                }
            }
            return ES_OK;
        }

        ESResult EnrolleeSecurity::provisionAcl()
        {
            // TODO : Currently device uuid is hardcoded, but this id has to be obtained from the
            // application
            OicSecAcl_t *acl = nullptr;

            acl = (OicSecAcl_t *) OICCalloc(1, sizeof(OicSecAcl_t));
            if (nullptr == acl)
            {
                OC_LOG(ERROR, ENROLEE_SECURITY_TAG, "Error while memory allocation");
                return ES_ERROR;
            }

            if ( createProvisiongResourceACL (acl) == ES_ERROR)
            {
               return ES_ERROR;
            }


            OC::ResultCallBack provisioningCb = std::bind(&EnrolleeSecurity::provisionCb, this,
            std::placeholders::_1, std::placeholders::_2);

            if (m_unownedDevice->provisionACL(acl, provisioningCb) != OC_STACK_OK)
            {
                OC_LOG(ERROR, ENROLEE_SECURITY_TAG, "provisionACL is failed");
                return ES_ERROR;
            }
            return ES_OK;
        }
    }
}
