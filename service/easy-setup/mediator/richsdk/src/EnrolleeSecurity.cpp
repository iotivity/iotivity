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
        std::shared_ptr< EnrolleeResource > EnrolleeResource,
        std::string secDbPath)
        {
            m_enrolleeSecState = EnrolleeSecState::ES_SEC_UNKNOWN;
            m_EnrolleeResource = EnrolleeResource;
        }

        void EnrolleeSecurity::registerCallbackHandler(SecurityProvStatusCb securityProvStatusCb,
                SecurityPinCb securityPinCb, SecProvisioningDbPathCb secProvisioningDbPathCb)
        {
            m_securityProvStatusCb = securityProvStatusCb;
            m_securityPinCb = securityPinCb;
            m_secProvisioningDbPathCb = secProvisioningDbPathCb;
        }

        std::shared_ptr< OC::OCSecureResource > EnrolleeSecurity::getEnrollee(DeviceList_t &list)
        {
            for (unsigned int i = 0; i < list.size(); i++)
            {
                if(m_deviceId == list[i]->getDeviceID().c_str())
                {
                    OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "Device %d ID %s ", i + 1,
                            list[i]->getDeviceID().c_str());
                    OIC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "From IP :%s", list[i]->getDevAddr().c_str());
                    return list[i];
                }
                //Always return the first element of the unOwned devices. This is considering that Mediator is
                // always connected with only one Enrollee for which ownership transfer is being performed.
                // Incase of multiple Enrollee devices connected to the Mediator via any OnBoarding method (SoftAp
                // for example), the Enrollee devices will be provisioned in the first come first serve basis in the order
                // returned by the security layer.

            }
            OIC_LOG(ERROR, ENROLEE_SECURITY_TAG,"Error!!! DeviceList_t is NULL");
            return NULL;
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

        void EnrolleeSecurity::setTargetDevID(const std::string devID)
        {
            m_deviceId = devID;
        }

        void EnrolleeSecurity::performOwnershipTransfer()
        {
            OC::DeviceList_t pUnownedDevList, pOwnedDevList;

            pOwnedDevList.clear();
            pUnownedDevList.clear();

            OCStackResult result;
            /*
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
            */
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
            }
            else
            {
                OIC_LOG(ERROR, ENROLEE_SECURITY_TAG, "No unOwned devices found.");
                throw ESException("No unOwned devices found.");
            }
        }
    }
}
