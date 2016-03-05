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
#include "RemoteEnrolleeResource.h"
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
        RemoteEnrollee::RemoteEnrollee(const ProvConfig& provConfig, const WiFiOnboadingConnection& connection) :
                m_ProvConfig(provConfig), m_wifiOnboardingconn(connection)
        {
            m_currentESState = CurrentESState::ES_UNKNOWN;
            m_isSecured = connection.isSecured; //enrolleeNWProvInfo.needSecuredEasysetup;

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

        void RemoteEnrollee::registerEasySetupStatusHandler(EasySetupStatusCB callback)
        {
            OIC_LOG ( DEBUG, ES_REMOTE_ENROLLEE_TAG, "Entered registerStatusHandler");
            if(!callback)
            {
                throw ESInvalidParameterException("Callback is empty");
            }

            if (m_easySetupStatusCb)
            {
                throw ESBadRequestException("Callback handler already registered");
            }
            else
            {
                m_easySetupStatusCb = callback;

                m_remoteResource = std::make_shared< RemoteEnrolleeResource >(m_ProvConfig, m_wifiOnboardingconn);
            }
        }

        void RemoteEnrollee::easySetupSecurityStatusCallback(
                        std::shared_ptr< SecProvisioningResult > secProvisioningResult)
        {
            OIC_LOG_V(DEBUG, ES_REMOTE_ENROLLEE_TAG, "easySetupStatusCallback status is, UUID = %s, "
                    "Status = %d", secProvisioningResult->getDeviceUUID().c_str(),
                    secProvisioningResult->getResult());

            if(secProvisioningResult->getResult() == ES_OK)
            {
                OIC_LOG(DEBUG, ES_REMOTE_ENROLLEE_TAG, "Ownership and ACL are successful. "
                        "Continue with Network information provisioning");

                m_currentESState = CurrentESState::ES_OWNED;

                OIC_LOG(DEBUG,ES_REMOTE_ENROLLEE_TAG,"Before ProvisionEnrollee");

                RemoteEnrolleeResource::ProvStatusCb provStatusCb = std::bind(
                        &RemoteEnrollee::provisioningStatusHandler, this, std::placeholders::_1);

                m_remoteResource->registerProvStatusCallback(provStatusCb);
                m_remoteResource->provisionEnrollee();
            }
            else
            {
                OIC_LOG(DEBUG, ES_REMOTE_ENROLLEE_TAG, "Ownership and ACL are successful");
                std::shared_ptr< EasySetupStatus > easySetupStatus = nullptr;
                easySetupStatus = std::make_shared< EasySetupStatus >(DEVICE_NOT_PROVISIONED,
                                            m_ProvConfig);
                if (m_easySetupStatusCb)
                {
                    if (easySetupStatus)
                    {
                        m_easySetupStatusCb(easySetupStatus);
                    }
                    else
                    {
                        m_easySetupStatusCb(nullptr);
                    }
                }
            }
        }

        void RemoteEnrollee::provisioningStatusHandler(
                std::shared_ptr< ProvisioningStatus > provStatus)
        {
            OIC_LOG(DEBUG,ES_REMOTE_ENROLLEE_TAG,"Entering ProvisioningStatusHandler");

            OIC_LOG_V(DEBUG,ES_REMOTE_ENROLLEE_TAG,"ProvStatus = %d", provStatus->getESResult());

            std::shared_ptr< EasySetupStatus > easySetupStatus = nullptr;

            if (m_isSecured)
            {
                if (m_currentESState >= CurrentESState::ES_OWNED)
                {
                    goto CALLBACK_CHECK;
                }
                else
                {
                    goto FAILURE;
                }
            }
            else
            {
                goto CALLBACK_CHECK;
            }

            CALLBACK_CHECK:

            if (provStatus->getESResult() == ES_OK)
            {
                if (provStatus->getESState() >= ESState::ES_PROVISIONED_ALREADY)
                {
                    easySetupStatus = std::make_shared< EasySetupStatus >(DEVICE_PROVISIONED,
                            m_ProvConfig);
                }
                else
                {
                    easySetupStatus = std::make_shared< EasySetupStatus >(DEVICE_NOT_PROVISIONED,
                            m_ProvConfig);
                }
            }
            else
            {
                easySetupStatus = std::make_shared< EasySetupStatus >(DEVICE_NOT_PROVISIONED,
                        m_ProvConfig);
            }

            if (m_easySetupStatusCb)
            {
                if (easySetupStatus)
                {
                    m_easySetupStatusCb(easySetupStatus);
                }
                else
                {
                    m_easySetupStatusCb(nullptr);
                }
            }

            return;

            FAILURE:

            easySetupStatus = std::make_shared< EasySetupStatus >(DEVICE_NOT_PROVISIONED,
                                    m_ProvConfig);

            if (easySetupStatus)
            {
                m_easySetupStatusCb(easySetupStatus);
            }
            else
            {
                m_easySetupStatusCb(nullptr);
            }
            return;
        }

        void RemoteEnrollee::startProvisioning()
        {
            OIC_LOG(DEBUG,ES_REMOTE_ENROLLEE_TAG,"Entering startProvisioning");
            if (m_remoteResource == nullptr)
            {
                throw ESBadRequestException ("Device not created");
            }

            ESResult result = ES_ERROR;

            result = m_remoteResource->constructResourceObject();

            if (result == ES_ERROR)
            {
                OIC_LOG(ERROR,ES_REMOTE_ENROLLEE_TAG,
                                    "Failed to create device using constructResourceObject");
                throw ESBadRequestException ("Device not created");
            }

            m_currentESState = CurrentESState::ES_ONBOARDED;

#ifdef __WITH_DTLS__
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

                        RemoteEnrolleeResource::ProvStatusCb provStatusCb = std::bind(
                                &RemoteEnrollee::provisioningStatusHandler,
                                this, std::placeholders::_1);

                        m_remoteResource->registerProvStatusCallback(provStatusCb);
                        m_remoteResource->provisionEnrollee();
                    }
                }
                catch (OCException & e)
                {
                    OIC_LOG_V(ERROR, ES_REMOTE_ENROLLEE_TAG,
                            "Exception for performOwnershipTransfer : %s", e.reason().c_str());
                    return ;
                }
            }
#else
            OIC_LOG(DEBUG,ES_REMOTE_ENROLLEE_TAG,"Before ProvisionEnrollee");

            RemoteEnrolleeResource::ProvStatusCb provStatusCb = std::bind(
                    &RemoteEnrollee::provisioningStatusHandler, this, std::placeholders::_1);

            m_remoteResource->registerProvStatusCallback(provStatusCb);
            m_remoteResource->provisionEnrollee();
#endif
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
