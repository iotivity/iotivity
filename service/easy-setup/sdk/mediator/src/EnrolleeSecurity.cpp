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

#include "EnrolleeSecurity.h"
#include "oxmjustworks.h"
#include "oxmrandompin.h"
#include "RemoteEnrolleeResource.h"
#include "logger.h"
#include "ESException.h"
#include "oic_malloc.h"

namespace OIC
{
    namespace Service
    {
        //TODO : Currently discovery timeout for owned and unowned devices is fixed as 5
        // The value should be accepted from the application as a parameter during ocplatform
        // config call
        #define ES_SEC_DISCOVERY_TIMEOUT 5

        EnrolleeSecurity::EnrolleeSecurity(
                std::shared_ptr< RemoteEnrolleeResource > remoteEnrolleeResource,
                std::string secDbPath)
        {
            m_enrolleeSecState = EnrolleeSecState::ES_SEC_UNKNOWN;

            //Initializing the provisioning client stack using the db path provided by the
            // application.
            // Note : If the path is NULL or empty, the PDM.db should be present in the same path.
            OCStackResult result = OCSecure::provisionInit(secDbPath);

            if (result != OC_STACK_OK)
            {
                throw ESPlatformException(result);
            }
        }

        ESResult EnrolleeSecurity::registerCallbackHandler(
                EnrolleeSecStatusCb enrolleeSecStatusCb, SecurityPinCb securityPinCb,
                SecProvisioningDbPathCb secProvisioningDbPathCb)

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
                        list[i]->getDeviceID().c_str());
                OC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "From IP :%s", list[i]->getDevAddr().c_str());

                if(list[i]->getDevAddr() == host)
                {
                    return list[i];
                }
            }

            return nullptr;
        }

        void EnrolleeSecurity::ownershipTransferCb(OC::PMResultList_t *result, int hasError)
        {
            if (hasError)
            {
                OC_LOG(ERROR, ENROLEE_SECURITY_TAG,"Error!!! in OwnershipTransfer");
            }
            else
            {
                OC_LOG(DEBUG, ENROLEE_SECURITY_TAG,"Transferred Ownership successfuly for device : ");
                OC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "UUID : %s",result->at(0).deviceId.id);

                delete result;

                //TODO : Decide if we have to manage the owned/unowned devices.
                //pOwnedDevList.push_back(pUnownedDevList[transferDevIdx]);
                //pUnownedDevList.erase(pUnownedDevList.begin() + transferDevIdx);
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
                OC_LOG(ERROR, ENROLEE_SECURITY_TAG, "Error in provisioning operation!");
            }
            else
            {
                OC_LOG(DEBUG, ENROLEE_SECURITY_TAG, "Received provisioning results: ");
                for (unsigned int i = 0; i < result->size(); i++)
                {
                    OC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "Result is = %d for device",result->at(i).res); OC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "UUID : %s",result->at(0).deviceId.id);
                }

                delete result;
            }
        }

        ESResult EnrolleeSecurity::performOwnershipTransfer()
        {
            ESResult res = ES_ERROR;

            OC::DeviceList_t pUnownedDevList, pOwnedDevList;

            pOwnedDevList.clear();
            pUnownedDevList.clear();

            OCStackResult result = OCSecure::discoverOwnedDevices(ES_SEC_DISCOVERY_TIMEOUT,
                    pOwnedDevList);
            if (result != OC_STACK_OK)
            {
                OC_LOG(ERROR, ENROLEE_SECURITY_TAG, "Owned Discovery failed.");
                res = ES_ERROR;

                return res;
            }
            else if (pOwnedDevList.size())
            {
                OC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "Found owned devices. Count =%d",
                        pOwnedDevList.size());
                std::shared_ptr< OC::OCSecureResource > ownedDevice =
                        findEnrollee(
                                m_remoteEnrolleeResource->m_enrolleeNWProvInfo.netAddressInfo.WIFI.ipAddress,
                                pOwnedDevList);
                if(ownedDevice)
                {
                    res = ES_OK;
                }
            }
            else
            {
                OC_LOG(ERROR, ENROLEE_SECURITY_TAG, "No Secure devices found.");
                res = ES_ERROR;

                return res;
            }

            result = OCSecure::discoverUnownedDevices(ES_SEC_DISCOVERY_TIMEOUT,
                    pUnownedDevList);
            if (result != OC_STACK_OK)
            {
                OC_LOG(ERROR, ENROLEE_SECURITY_TAG, "UnOwned Discovery failed.");
                res = ES_ERROR;

                return res;
            }
            else if (pUnownedDevList.size())
            {
                OC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "Found Unowned devices. Count =%d",
                        pUnownedDevList.size());

                std::shared_ptr< OC::OCSecureResource > unownedDevice =
                        findEnrollee(
                                m_remoteEnrolleeResource->m_enrolleeNWProvInfo.netAddressInfo.WIFI.ipAddress,
                                pUnownedDevList);
                if (unownedDevice)
                {
                    OTMCallbackData_t justWorksCBData;
                    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
                    justWorksCBData.createSecureSessionCB = CreateSecureSessionJustWorksCallback;
                    justWorksCBData.createSelectOxmPayloadCB = CreateJustWorksSelectOxmPayload;
                    justWorksCBData.createOwnerTransferPayloadCB =
                            CreateJustWorksOwnerTransferPayload;
                    OCSecure::setOwnerTransferCallbackData(OIC_JUST_WORKS, &justWorksCBData, NULL);

                    OC_LOG_V(DEBUG, ENROLEE_SECURITY_TAG, "Transfering ownership for : %s ",
                            unownedDevice->getDeviceID().c_str());

                    OC::ResultCallBack ownershipTransferCb = std::bind(
                            &EnrolleeSecurity::ownershipTransferCb, this, std::placeholders::_1,
                            std::placeholders::_2);

                    if (unownedDevice->doOwnershipTransfer(ownershipTransferCb) != OC_STACK_OK)
                    {
                        OC_LOG(ERROR, ENROLEE_SECURITY_TAG, "OwnershipTransferCallback is failed");
                        res = ES_ERROR;
                    }
                    else
                    {
                        if(provisionCreds() == ES_OK)
                        {
                            OC_LOG(DEBUG, ENROLEE_SECURITY_TAG,
                                    "provisionCreds success. Continuing with provisioning ACL");

                            if (provisionAcl() == ES_OK)
                            {
                                OC_LOG(DEBUG, ENROLEE_SECURITY_TAG,
                                        "provisionAcl success.");

                                //returning success;
                                res = ES_OK;
                            }
                            else
                            {
                                OC_LOG(ERROR, ENROLEE_SECURITY_TAG, "provisionAcl failed.");
                                res = ES_ERROR;
                            }
                        }
                        else
                        {
                            OC_LOG(ERROR, ENROLEE_SECURITY_TAG, "provisionCreds failed.");
                            res = ES_ERROR;
                        }
                    }
                }
            }
            else
            {
                OC_LOG(ERROR, ENROLEE_SECURITY_TAG, "No Secure devices found.");
                res = ES_ERROR;

                return res;
            }

            return res;
        }

        ESResult EnrolleeSecurity::provisionAcl()
        {
            // TODO : Currently device id is hardcoded, but this id has to be obtained from the
            // application
            OicSecAcl_t *acl = nullptr;

            acl = (OicSecAcl_t *) OICCalloc(1, sizeof(OicSecAcl_t));
            if (nullptr == acl)
            {
                OC_LOG(ERROR, ENROLEE_SECURITY_TAG, "Error while memory allocation");
                return ES_ERROR;
            }

//            std::cout << "Please input ACL for selected device: " << std::endl;
//            if (0 != InputACL(acl1))
//            {
//                break;
//            }

            OC::ResultCallBack provisioningCb = std::bind(&EnrolleeSecurity::provisionCb, this,
                    std::placeholders::_1, std::placeholders::_2);

            if (m_securedResource->provisionACL(acl, provisioningCb) != OC_STACK_OK)
            {
                OC_LOG(ERROR, ENROLEE_SECURITY_TAG, "provisionACL is failed");
            }
            return ES_ERROR;
        }

        ESResult EnrolleeSecurity::provisionCreds()
        {
//            int devices[2];
//
//            if (0 != readDeviceNumber(pOwnedDevList, 2, devices))
//                break;
//
//            int first = devices[0];
//            int second = devices[1];
//
//            std::cout << "Provision Credentials to devices: "
//                    << pOwnedDevList[first]->getDeviceID();
//            std::cout << " and " << pOwnedDevList[second]->getDeviceID() << std::endl;
//
//            Credential cred(NO_SECURITY_MODE, 0);
//            std::cout << "Please input credentials for selected devices: " << std::endl;
//            if (0 != InputCredentials(cred))
//                break;
//
//            ask = 0;
//
//            if (pOwnedDevList[first]->provisionCredentials(cred, *pOwnedDevList[second].get(),
//                    provisionCB) != OC_STACK_OK)
//            {
//                ask = 1;
//                std::cout << "provisionCredentials is failed" << std::endl;
//            }
            return ES_ERROR;
        }
    }
}
