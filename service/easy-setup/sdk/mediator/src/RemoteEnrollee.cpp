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
        RemoteEnrollee::RemoteEnrollee(const EnrolleeNWProvInfo& enrolleeNWProvInfo) :
                m_enrolleeNWProvInfo(enrolleeNWProvInfo)
        {
            m_currentESState = CurrentESState::ES_UNKNOWN;
            m_needSecuredEasysetup = enrolleeNWProvInfo.needSecuredEasysetup;

            OC_LOG ( DEBUG, ES_REMOTE_ENROLLEE_TAG, "Inside RemoteEnrollee constr");
        }

#ifdef __WITH_DTLS__
        ESResult RemoteEnrollee::registerSecurityCallbackHandler(
                EnrolleeSecStatusCb enrolleeSecStatusCb, SecurityPinCb securityPinCb,
                SecProvisioningDbPathCb secProvisioningDbPathCb)
        {
            // No need to check NULL for m_secProvisioningDbPathCB as this is not a mandatory
            // callback function. If m_secProvisioningDbPathCB is NULL, provisioning manager
            // in security layer will try to find the PDM.db file in the local path.
            // If PDM.db is found, the provisioning manager operations will succeed.
            // Otherwise all the provisioning manager operations will fail.
            m_secProvisioningDbPathCb = secProvisioningDbPathCb;
            m_enrolleeSecStatusCb = enrolleeSecStatusCb;
            m_securityPinCb = securityPinCb;
            return ES_OK;
        }
#endif //__WITH_DTLS__

        void RemoteEnrollee::registerEasySetupStatusHandler(EasySetupStatusCB callback)
        {
            OC_LOG ( DEBUG, ES_REMOTE_ENROLLEE_TAG, "Entered registerStatusHandler");
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

                m_remoteResource = std::make_shared< RemoteEnrolleeResource >(m_enrolleeNWProvInfo);

                m_remoteResource->constructResourceObject();
            }
        }

        void RemoteEnrollee::provisioningStatusHandler(
                std::shared_ptr< ProvisioningStatus > provStatus)
        {
            OC_LOG(DEBUG,ES_REMOTE_ENROLLEE_TAG,"Entering ProvisioningStatusHandler");

            OC_LOG_V(DEBUG,ES_REMOTE_ENROLLEE_TAG,"ProvStatus = %d", provStatus->getESResult());

            std::shared_ptr< EasySetupStatus > easySetupStatus = nullptr;

            if (m_enrolleeNWProvInfo.isSecured)
            {
                if (m_currentESState > CurrentESState::ES_OWNED)
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
                            m_enrolleeNWProvInfo);
                }
                else
                {
                    easySetupStatus = std::make_shared< EasySetupStatus >(DEVICE_NOT_PROVISIONED,
                            m_enrolleeNWProvInfo);
                }
            }
            else
            {
                easySetupStatus = std::make_shared< EasySetupStatus >(DEVICE_NOT_PROVISIONED,
                        m_enrolleeNWProvInfo);
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
                                    m_enrolleeNWProvInfo);

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
            OC_LOG(DEBUG,ES_REMOTE_ENROLLEE_TAG,"Entering startProvisioning");
            if (m_remoteResource == nullptr)
            {
                throw ESBadRequestException ("Device not created");
            }
            else
            {
                m_currentESState = CurrentESState::ES_ONBOARDED;

#ifdef __WITH_DTLS__
                if (m_needSecuredEasysetup && m_currentESState < CurrentESState::ES_OWNED)
                {
                    //TODO : DBPath is passed empty as of now. Need to take dbpath from application.
                    m_enrolleeSecurity = std::make_shared <EnrolleeSecurity> (m_remoteResource, "");

                    m_enrolleeSecurity->registerCallbackHandler(m_enrolleeSecStatusCb,
                            m_securityPinCb, m_secProvisioningDbPathCb);

                    if (m_enrolleeSecurity->performOwnershipTransfer() == ES_ERROR)
                    {
                        std::shared_ptr< EasySetupStatus > easySetupStatus = nullptr;

                        easySetupStatus = std::make_shared < EasySetupStatus
                        > (DEVICE_NOT_OWNED, m_enrolleeNWProvInfo);
                        return;
                    }
                }
#endif

                OC_LOG(DEBUG,ES_REMOTE_ENROLLEE_TAG,"Before ProvisionEnrollee");

                RemoteEnrolleeResource::ProvStatusCb provStatusCb = std::bind(
                        &RemoteEnrollee::provisioningStatusHandler, this, std::placeholders::_1);

                m_remoteResource->registerProvStatusCallback(provStatusCb);
                m_remoteResource->provisionEnrollee();
            }
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

        EnrolleeNWProvInfo& RemoteEnrollee::getEnrolleeProvisioningInfo ()
        {
            return m_enrolleeNWProvInfo;
        }
    }
}
