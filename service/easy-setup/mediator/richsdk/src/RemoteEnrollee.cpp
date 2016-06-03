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
#include "ESException.h"
#include "logger.h"
#ifdef __WITH_DTLS__
#include "EnrolleeSecurity.h"
#endif //__WITH_DTLS

namespace OIC
{
    #define ES_REMOTE_ENROLLEE_TAG "ES_REMOTE_ENROLLEE"

    namespace Service
    {
        RemoteEnrollee::RemoteEnrollee(const WiFiOnboadingConnection& wifiOnboardingconn) :
                m_wifiOnboardingconn(wifiOnboardingconn)
        {
            m_requestCapabilityStatusCb = nullptr;
            m_currentESState = CurrentESState::ES_ONBOARDED;
            m_isSecured = m_wifiOnboardingconn.isSecured;

            m_remoteResource = std::make_shared< EnrolleeResource >(m_wifiOnboardingconn);

            OIC_LOG ( DEBUG, ES_REMOTE_ENROLLEE_TAG, "Inside RemoteEnrollee constr");
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

        void RemoteEnrollee::easySetupSecurityStatusCallback(
                        std::shared_ptr< SecProvisioningStatus > secProvisioningStatus)
        {
            OIC_LOG_V(DEBUG, ES_REMOTE_ENROLLEE_TAG, "easySetupStatusCallback status is, UUID = %s, "
                    "Status = %d", secProvisioningStatus->getDeviceUUID().c_str(),
                    secProvisioningStatus->getResult());

            if(secProvisioningStatus->getResult() == ES_OK)
            {
                OIC_LOG(DEBUG, ES_REMOTE_ENROLLEE_TAG, "Ownership and ACL are successful. "
                        "Continue with Network information provisioning");

                m_currentESState = CurrentESState::ES_OWNED;

                OIC_LOG(DEBUG,ES_REMOTE_ENROLLEE_TAG,"Before ProvisionEnrollee");

                m_enrolleeSecStatusCb(secProvisioningStatus);
            }
            else
            {
                OIC_LOG(DEBUG, ES_REMOTE_ENROLLEE_TAG, "Ownership and ACL are fail");

                m_enrolleeSecStatusCb(secProvisioningStatus);
            }
        }

        void RemoteEnrollee::InitRemoteEnrolleeStatusHandler (
                std::shared_ptr< InitRemoteEnrolleeStatus > initRemoteEnrolleeStatus)
        {
            OIC_LOG(DEBUG,ES_REMOTE_ENROLLEE_TAG,"Entering InitRemoteEnrolleeStatusHandler");

            OIC_LOG_V(DEBUG,ES_REMOTE_ENROLLEE_TAG,"initRemoteEnrolleeStatus = %d", initRemoteEnrolleeStatus->getESResult());

            m_initRemoteEnrolleeStatusCb(initRemoteEnrolleeStatus);
        }

        void RemoteEnrollee::requestCapabilityStatusHandler (
                std::shared_ptr< RequestCapabilityStatus > requestCapabilityStatus)
        {
            OIC_LOG(DEBUG,ES_REMOTE_ENROLLEE_TAG,"Entering requestCapabilityStatusHandler");

            OIC_LOG_V(DEBUG,ES_REMOTE_ENROLLEE_TAG,"requestCapabilityStatus = %d", requestCapabilityStatus->getESResult());

            m_requestCapabilityStatusCb(requestCapabilityStatus);
        }

        void RemoteEnrollee::dataProvisioningStatusHandler(
                std::shared_ptr< ProvisioningStatus > provStatus)
        {
            OIC_LOG(DEBUG,ES_REMOTE_ENROLLEE_TAG,"Entering dataprovisioningStatusHandler");

            OIC_LOG_V(DEBUG,ES_REMOTE_ENROLLEE_TAG,"ProvStatus = %d", provStatus->getESResult());

            if (provStatus->getESResult() == ES_OK)
            {
                if (provStatus->getESState() >= ESState::ES_PROVISIONED_ALREADY)
                {
                    OIC_LOG_V(DEBUG,ES_REMOTE_ENROLLEE_TAG,"ProvStatus = %d", provStatus->getESResult());
                    m_currentESState = CurrentESState::ES_PROVISIONED;
                }
            }
            m_dataProvStatusCb(provStatus);

            return;
        }

        void RemoteEnrollee::initRemoteEnrollee(InitRemoteEnrolleeStatusCb callback)
        {
            ESResult result = ES_ERROR;

            if(!callback)
            {
                throw ESInvalidParameterException("Callback is empty");
            }

            m_initRemoteEnrolleeStatusCb = callback;

            if (m_remoteResource != nullptr)
            {
                throw ESBadRequestException ("Already created");
            }

            InitRemoteEnrolleeStatusCb initRemoteEnrolleeStatusCb = std::bind(
                    &RemoteEnrollee::InitRemoteEnrolleeStatusHandler, this, std::placeholders::_1);
            m_remoteResource->registerInitRemoteEnrolleeStatusCallback(initRemoteEnrolleeStatusCb);

            result = m_remoteResource->constructResourceObject();

            if (result == ES_ERROR)
            {
                OIC_LOG(ERROR,ES_REMOTE_ENROLLEE_TAG,
                                    "Failed to create device using constructResourceObject");
                throw ESBadRequestException ("Device not created");
            }
        }

        void RemoteEnrollee::startSecurityProvisioning(EnrolleeSecStatusCb callback)
        {
#ifdef __WITH_DTLS__

            m_enrolleeSecStatusCb = callback;

            if (m_isSecured && m_currentESState < CurrentESState::ES_OWNED)
            {
                EnrolleeSecStatusCb securityProvStatusCb = std::bind(
                        &RemoteEnrollee::easySetupSecurityStatusCallback,
                        this,
                        std::placeholders::_1);
                //TODO : DBPath is passed empty as of now. Need to take dbpath from application.
                m_enrolleeSecurity = std::make_shared <EnrolleeSecurity> (m_remoteResource, "");

                m_enrolleeSecurity->registerCallbackHandler(securityProvStatusCb,
                        m_securityPinCb, m_secProvisioningDbPathCb);

                try
                {
                    EasySetupState easySetupState = m_enrolleeSecurity->performOwnershipTransfer();
                    if (easySetupState == DEVICE_NOT_OWNED)
                    {
                        OIC_LOG_V(DEBUG, ES_REMOTE_ENROLLEE_TAG,
                                "performOwnershipTransfer returned : %d",
                                easySetupState);
                        return;
                    }
                    else if (easySetupState == DEVICE_OWNED)
                    {
                        OIC_LOG_V(DEBUG, ES_REMOTE_ENROLLEE_TAG,
                                "performOwnershipTransfer returned : %d",
                                easySetupState);
                        OIC_LOG(DEBUG,ES_REMOTE_ENROLLEE_TAG,"Before ProvisionEnrollee");
                    }
                }
                catch (OCException & e)
                {
                    OIC_LOG_V(ERROR, ES_REMOTE_ENROLLEE_TAG,
                            "Exception for performOwnershipTransfer : %s", e.reason().c_str());
                    return ;
                }
            }
#endif
        }

        void RemoteEnrollee::getCapabilityData(RequestCapabilityStatusCb callback)
        {
            ESResult result = ES_ERROR;

            if(!callback)
            {
                throw ESInvalidParameterException("Callback is empty");
            }

            m_requestCapabilityStatusCb = callback;

            if (m_remoteResource == nullptr)
            {
                throw ESBadRequestException ("Device not created");
            }

            RequestCapabilityStatusCb requestCapabilityStatusCb = std::bind(
                    &RemoteEnrollee::requestCapabilityStatusHandler, this, std::placeholders::_1);
            m_remoteResource->registerCapabilityStatusCallback(requestCapabilityStatusCb);
            m_remoteResource->getCapabilityData();
        }

        void RemoteEnrollee::startDataProvisioning(const ProvConfig& dataProvConfig, DataProvStatusCb callback)
        {
            OIC_LOG(DEBUG,ES_REMOTE_ENROLLEE_TAG,"Before ProvisionEnrollee");

            m_ProvConfig = dataProvConfig;
            m_dataProvStatusCb = callback;

            DataProvStatusCb dataProvStatusCb = std::bind(
                    &RemoteEnrollee::dataProvisioningStatusHandler, this, std::placeholders::_1);

            m_remoteResource->registerProvStatusCallback(dataProvStatusCb);
            m_remoteResource->provisionEnrollee();
        }

        void RemoteEnrollee::stopProvisioning()
        {
            m_currentESState = CurrentESState::ES_UNKNOWN;

            m_remoteResource->unprovisionEnrollee();
        }

        bool RemoteEnrollee::isEnrolleeProvisioned()
        {
            if(m_currentESState >= CurrentESState::ES_PROVISIONED)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        ProvConfig RemoteEnrollee::getProvConfig ()
        {
            return m_ProvConfig;
        }

       WiFiOnboadingConnection RemoteEnrollee::getOnboardConn()
       {
         return m_wifiOnboardingconn;
       }

    }
}
