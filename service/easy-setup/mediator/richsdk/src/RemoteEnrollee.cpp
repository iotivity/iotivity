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

#include "RemoteEnrollee.h"
#include "EnrolleeResource.h"
#include "CloudResource.h"
#include "OCPlatform.h"
#include "ESException.h"
#include "logger.h"
#include "OCResource.h"
#ifdef __WITH_DTLS__
#include "EnrolleeSecurity.h"
#endif //__WITH_DTLS

namespace OIC
{
    namespace Service
    {
        static const char ES_BASE_RES_URI[] = "/oic/res";
        #define ES_REMOTE_ENROLLEE_TAG "ES_REMOTE_ENROLLEE"
        #define DISCOVERY_TIMEOUT 5

        RemoteEnrollee::RemoteEnrollee()
        {
            m_securityProvStatusCb = nullptr;
            m_requestPropertyDataStatusCb = nullptr;
            m_securityPinCb = nullptr;
            m_secProvisioningDbPathCb = nullptr;
            m_dataProvStatusCb = nullptr;
            m_cloudProvStatusCb = nullptr;

            //m_deviceId = nullptr;
        }

#ifdef __WITH_DTLS__
        ESResult RemoteEnrollee::registerSecurityCallbackHandler(SecurityPinCb securityPinCb,
                SecProvisioningDbPathCb secProvisioningDbPathCb)
        {
            // No need to check NULL for m_secProvisioningDbPathCB as this is not a mandatory
            // callback function. If m_secProvisioningDbPathCB is NULL, provisioning manager
            // in security layer will try to find the PDM.db file in the local path.
            // If PDM.db is found, the provisioning manager operations will succeed.
            // Otherwise all the provisioning manager operations will fail.
            m_secProvisioningDbPathCb = secProvisioningDbPathCb;
            m_securityPinCb = securityPinCb;
            return ES_OK;
        }
#endif //__WITH_DTLS__

        void RemoteEnrollee::securityStatusHandler(
                        std::shared_ptr< SecProvisioningStatus > status)
        {
            OIC_LOG_V(DEBUG, ES_REMOTE_ENROLLEE_TAG, "easySetupStatusCallback status is, UUID = %s, "
                    "Status = %d", status->getDeviceUUID().c_str(),
                    status->getESResult());

            if(status->getESResult() == ES_OK)
            {
                OIC_LOG(DEBUG, ES_REMOTE_ENROLLEE_TAG, "Ownership and ACL are successful. "
                        "Continue with Network information provisioning");

                OIC_LOG(DEBUG,ES_REMOTE_ENROLLEE_TAG,"Before ProvisionEnrollee");

                m_securityProvStatusCb(status);
            }
            else
            {
                OIC_LOG(DEBUG, ES_REMOTE_ENROLLEE_TAG, "Ownership and ACL are fail");

                m_securityProvStatusCb(status);
            }
        }

        void RemoteEnrollee::RequestPropertyDataStatusHandler (
                std::shared_ptr< RequestPropertyDataStatus > status)
        {
            OIC_LOG(DEBUG,ES_REMOTE_ENROLLEE_TAG,"Entering RequestPropertyDataStatusHandler");

            OIC_LOG_V(DEBUG,ES_REMOTE_ENROLLEE_TAG,"RequestPropertyDataStatus = %d", status->getESResult());

            if(status->getESResult() == ES_OK)
            {
                m_propertyData = status->getPropertyData();
            }

            m_requestPropertyDataStatusCb(status);
        }

        void RemoteEnrollee::dataProvisioningStatusHandler(
                std::shared_ptr< DataProvisioningStatus > status)
        {
            OIC_LOG(DEBUG,ES_REMOTE_ENROLLEE_TAG,"Entering dataProvisioningStatusHandler");

            OIC_LOG_V(DEBUG,ES_REMOTE_ENROLLEE_TAG,"ProvStatus = %d", status->getESResult());

            if (status->getESResult() == ES_OK)
            {
                if (status->getESDataProvState() >= ESDataProvState::ES_PROVISIONED_ALREADY)
                {
                    OIC_LOG_V(DEBUG,ES_REMOTE_ENROLLEE_TAG,"ProvStatus = %d", status->getESResult());
                }
            }
            m_dataProvStatusCb(status);

            return;
        }

        void RemoteEnrollee::cloudProvisioningStatusHandler (
                std::shared_ptr< CloudProvisioningStatus > status)
        {
            OIC_LOG(DEBUG,ES_REMOTE_ENROLLEE_TAG,"Entering cloudProvisioningStatusHandler");

            OIC_LOG_V(DEBUG,ES_REMOTE_ENROLLEE_TAG,"CloudProvStatus = %d", status->getESCloudState());

            m_cloudProvStatusCb(status);
            return;
        }

        ESResult RemoteEnrollee::ESDiscoveryTimeout(unsigned short waittime)
        {
            struct timespec startTime;
            startTime.tv_sec=0;
            startTime.tv_sec=0;
            struct timespec currTime;
            currTime.tv_sec=0;
            currTime.tv_nsec=0;

            ESResult res = ES_OK;
            #ifdef _POSIX_MONOTONIC_CLOCK
                int clock_res = clock_gettime(CLOCK_MONOTONIC, &startTime);
            #else
                int clock_res = clock_gettime(CLOCK_REALTIME, &startTime);
            #endif

            if (0 != clock_res)
            {
                return ES_ERROR;
            }

            while (ES_OK == res || m_discoveryResponse == false)
            {
                #ifdef _POSIX_MONOTONIC_CLOCK
                        clock_res = clock_gettime(CLOCK_MONOTONIC, &currTime);
                #else
                        clock_res = clock_gettime(CLOCK_REALTIME, &currTime);
                #endif

                if (0 != clock_res)
                {
                    return ES_ERROR;
                }
                long elapsed = (currTime.tv_sec - startTime.tv_sec);
                if (elapsed > waittime)
                {
                    return ES_OK;
                }
                if (m_discoveryResponse)
                {
                    res = ES_OK;
                }
             }
             return res;
        }

        void RemoteEnrollee::onDeviceDiscovered(std::shared_ptr<OC::OCResource> resource)
        {
            OIC_LOG (DEBUG, ES_REMOTE_ENROLLEE_TAG, "onDeviceDiscovered");

            std::string resourceURI;
            std::string hostAddress;
            std::string hostDeviceID;

            try
            {
                if(resource)
                {
                    if(!(resource->connectivityType() & CT_ADAPTER_TCP))
                    {
                        // Get the resource URI
                        resourceURI = resource->uri();
                        OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG,
                                "URI of the resource: %s", resourceURI.c_str());

                        // Get the resource host address
                        hostAddress = resource->host();
                        OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG,
                                "Host address of the resource: %s", hostAddress.c_str());

                        hostDeviceID = resource->sid();
                        OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG,
                                "Host DeviceID of the resource: %s", hostDeviceID.c_str());

                        if(m_deviceId .empty())
                        {
                            /*
                             * Easysetup is always performed with a single Enrollee device and
                             * in a private network (SoftAP or BLE), so the assumption is that
                             * only the intended device will respond for the discovery.
                             * With the above assumption the below two statements are written.
                             */
                            m_ocResource = resource;
                            m_deviceId = resource->sid();
                            m_discoveryResponse = true;
                        }

                        else if(!m_deviceId.empty() && m_deviceId == hostDeviceID)
                        {
                            OIC_LOG (DEBUG, ES_REMOTE_ENROLLEE_TAG, "Find matched CloudResource");
                            m_ocResource = resource;
                            m_discoveryResponse = true;
                        }
                    }
                }
            }
            catch(std::exception& e)
            {
                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG,
                        "Exception in foundResource: %s", e.what());
            }
        }

        ESResult RemoteEnrollee::discoverResource()
        {
            OIC_LOG(DEBUG, ES_REMOTE_ENROLLEE_TAG, "Enter discoverResource");

            if (m_ocResource != nullptr)
            {
                throw ESBadRequestException("resource is already created");
            }

            std::string query("");
            query.append(ES_BASE_RES_URI);
            query.append("?rt=");
            query.append(OC_RSRVD_ES_RES_TYPE_PROV);

            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG, "query = %s", query.c_str());

            m_discoveryResponse = false;

            std::function< void (std::shared_ptr<OC::OCResource>) > onDeviceDiscoveredCb =
                    std::bind(&RemoteEnrollee::onDeviceDiscovered, this,
                                                    std::placeholders::_1);
            OCStackResult result = OC::OCPlatform::findResource("", query, CT_DEFAULT,
                    onDeviceDiscoveredCb);

            if (result != OCStackResult::OC_STACK_OK)
            {
                OIC_LOG(ERROR,ES_REMOTE_ENROLLEE_TAG,
                        "Failed discoverResource");
                return ES_ERROR;
            }

            ESResult foundResponse = ESDiscoveryTimeout (DISCOVERY_TIMEOUT);

            if (foundResponse == ES_ERROR || !m_discoveryResponse)
            {
                OIC_LOG(ERROR,ES_REMOTE_ENROLLEE_TAG,
                        "Failed discoverResource because timeout");
                return ES_ERROR;
            }
            return ES_OK;
        }

        void RemoteEnrollee::initRemoteEnrollee()
        {
            ESResult result = ES_ERROR;

            if (m_enrolleeResource != nullptr)
            {
                throw ESBadRequestException ("Already created");
            }

            result = discoverResource();

            if (result == ES_ERROR)
            {
                OIC_LOG(ERROR,ES_REMOTE_ENROLLEE_TAG,
                                    "Failed to create resource object using discoverResource");
                throw ESBadRequestException ("Resource object not created");
            }

            else
            {
                if(m_ocResource != nullptr)
                {
                    m_enrolleeResource = std::make_shared<EnrolleeResource>(std::move(m_ocResource));
                }
                else
                {
                    throw ESBadGetException ("Resource handle is invalid");
                }
            }
        }

        void RemoteEnrollee::startSecurityProvisioning(SecurityProvStatusCb callback)
        {
#ifdef __WITH_DTLS__
            m_securityProvStatusCb = callback;

            SecurityProvStatusCb securityProvStatusCb = std::bind(
                    &RemoteEnrollee::securityStatusHandler,
                    this,
                    std::placeholders::_1);
            //TODO : DBPath is passed empty as of now. Need to take dbpath from application.
            m_enrolleeSecurity = std::make_shared <EnrolleeSecurity> (m_enrolleeResource, "");

            m_enrolleeSecurity->setTargetDevID(m_deviceId);
            m_enrolleeSecurity->registerCallbackHandler(securityProvStatusCb, m_securityPinCb, m_secProvisioningDbPathCb);

            try
            {
                m_enrolleeSecurity->performOwnershipTransfer();
            }
            catch (OCException & e)
            {
                OIC_LOG_V(ERROR, ES_REMOTE_ENROLLEE_TAG,
                        "Exception for performOwnershipTransfer : %s", e.reason().c_str());

                OIC_LOG(DEBUG,ES_REMOTE_ENROLLEE_TAG,"Fail performOwnershipTransfer");
                    std::shared_ptr< SecProvisioningStatus > securityProvisioningStatus =
                            std::make_shared< SecProvisioningStatus >(nullptr, ES_ERROR);
                    m_securityProvStatusCb(securityProvisioningStatus);
                return ;
            }
#else
            (void) callback;
            OIC_LOG(DEBUG,ES_REMOTE_ENROLLEE_TAG,"Mediator is unsecured.");

            std::shared_ptr< SecProvisioningStatus > securityProvisioningStatus =
                     std::make_shared< SecProvisioningStatus >(nullptr, ES_ERROR);
            m_securityProvStatusCb(securityProvisioningStatus);
#endif
        }

        void RemoteEnrollee::requestPropertyData(RequestPropertyDataStatusCb callback)
        {
            if(!callback)
            {
                throw ESInvalidParameterException("Callback is empty");
            }

            m_requestPropertyDataStatusCb = callback;

            if (m_enrolleeResource == nullptr)
            {
                throw ESBadRequestException ("Device not created");
            }

            RequestPropertyDataStatusCb RequestPropertyDataStatusCb = std::bind(
                    &RemoteEnrollee::RequestPropertyDataStatusHandler, this, std::placeholders::_1);
            m_enrolleeResource->registerRequestPropertyDataStatusCallback(RequestPropertyDataStatusCb);
            m_enrolleeResource->RequestPropertyData();
        }

        void RemoteEnrollee::startDataProvisioning(DataProvStatusCb callback)
        {
            OIC_LOG(DEBUG,ES_REMOTE_ENROLLEE_TAG,"Enter startDataProvisioning");

            if(!callback)
            {
                throw ESInvalidParameterException("Callback is empty");
            }

            m_dataProvStatusCb = callback;

            if (m_enrolleeResource == nullptr)
            {
                throw ESBadRequestException ("Device not created");
            }

            if(m_dataProvInfo.WIFI.ssid.empty())
            {
                throw ESBadRequestException ("Invalid Provisiong Data.");
            }

            m_dataProvStatusCb = callback;

            DataProvStatusCb dataProvStatusCb = std::bind(
                    &RemoteEnrollee::dataProvisioningStatusHandler, this, std::placeholders::_1);

            m_enrolleeResource->registerProvStatusCallback(dataProvStatusCb);
            m_enrolleeResource->provisionEnrollee(m_dataProvInfo);
        }

        void RemoteEnrollee::initCloudResource()
        {
            ESResult result = ES_ERROR;

            if (m_cloudResource != nullptr)
            {
                throw ESBadRequestException ("Already created");
            }

            result = discoverResource();

            if (result == ES_ERROR)
            {
                OIC_LOG(ERROR,ES_REMOTE_ENROLLEE_TAG,
                                    "Failed to create resource object using discoverResource");
                throw ESBadRequestException ("Resource object not created");
            }

            else
            {
                if(m_ocResource != nullptr)
                {
                    m_cloudResource = std::make_shared<CloudResource>(std::move(m_ocResource));

                    std::shared_ptr< CloudProvisioningStatus > provStatus = std::make_shared<
                        CloudProvisioningStatus >(ESResult::ES_OK, ESCloudProvState::ES_CLOUD_ENROLLEE_FOUND);

                    m_cloudProvStatusCb(provStatus);
                }
                else
                {
                    throw ESBadGetException ("Resource handle is invalid");
                }
            }
        }


        void RemoteEnrollee::startCloudProvisioning(CloudProvStatusCb callback)
        {
            OIC_LOG(DEBUG,ES_REMOTE_ENROLLEE_TAG,"Enter startCloudProvisioning");

            if(!callback)
            {
                throw ESInvalidParameterException("Callback is empty");
            }

            m_cloudProvStatusCb = callback;

            try
            {
                initCloudResource();
            }

            catch (const std::exception& e)
            {
                OIC_LOG_V(ERROR, ES_REMOTE_ENROLLEE_TAG,
                    "Exception caught in startCloudProvisioning = %s", e.what());

                std::shared_ptr< CloudProvisioningStatus > provStatus = std::make_shared<
                        CloudProvisioningStatus >(ESResult::ES_ERROR, ESCloudProvState::ES_CLOUD_ENROLLEE_NOT_FOUND);
                m_cloudProvStatusCb(provStatus);
            }

            if (m_cloudResource == nullptr)
            {
                throw ESBadRequestException ("Cloud Resource not created");
            }

            if(m_cloudProvInfo.authCode.empty()
                || m_cloudProvInfo.authProvider.empty()
                || m_cloudProvInfo.ciServer.empty())
            {
                throw ESBadRequestException ("Invalid Cloud Provisiong Info.");
            }

            CloudProvStatusCb cloudProvStatusCb = std::bind(
                    &RemoteEnrollee::cloudProvisioningStatusHandler, this, std::placeholders::_1);

            m_cloudResource->registerCloudProvisioningStatusCallback(cloudProvStatusCb);
            m_cloudResource->provisionEnrollee(m_cloudProvInfo);
        }

        void RemoteEnrollee::setDataProvInfo(const DataProvInfo& dataProvInfo)
        {
            m_dataProvInfo = dataProvInfo;
        }

        void RemoteEnrollee::setCloudProvInfo(const CloudProvInfo& cloudProvInfo)
        {
            m_cloudProvInfo = cloudProvInfo;
        }

        DataProvInfo RemoteEnrollee::getDataProvInfo()
        {
            return m_dataProvInfo;
        }
    }
}
