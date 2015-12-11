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

#include <functional>

#include "RemoteEnrolleeResource.h"

#include "OCPlatform.h"
#include "ESException.h"
#include "OCResource.h"
#include "logger.h"

namespace OIC
{
    namespace Service
    {
        #define ES_REMOTE_ENROLLEE_TAG "ES_REMOTE_ENROLLEE"
        static const char ES_PROV_RES_URI[]  = "/oic/prov";
        static const char ES_PROV_RES_TYPE[] = "oic.r.prov";

        RemoteEnrolleeResource::RemoteEnrolleeResource(EnrolleeNWProvInfo enrolleeNWProvInfo)
        {
            m_enrolleeNWProvInfo = enrolleeNWProvInfo;
        }

        void RemoteEnrolleeResource::checkProvInformationCb(const HeaderOptions& /*headerOptions*/,
                const OCRepresentation& rep, const int eCode)
        {
            OC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG, "checkProvInformationCb : %s, eCode = %d",
                    rep.getUri().c_str(),
                    eCode);

            if(eCode != 0)
            {
                OC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG,
                        "checkProvInformationCb : Provisioning is failed ");
                std::shared_ptr< ProvisioningStatus > provStatus = std::make_shared<
                        ProvisioningStatus >(ESResult::ES_ERROR, ESState::ES_PROVISIONING_ERROR);
                m_provStatusCb(provStatus);
                return;
            }

            int ps = -1;
            std::string tnn = "";
            std::string cd = "";

            rep.getValue(OC_RSRVD_ES_PS, ps);
            rep.getValue(OC_RSRVD_ES_TNN, tnn);
            rep.getValue(OC_RSRVD_ES_CD, cd);

            OC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG, "checkProvInformationCb : ps - %d", ps);
            OC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG, "checkProvInformationCb : tnn - %s", tnn.c_str());
            OC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG, "checkProvInformationCb : cd - %s", cd.c_str());

            //Provisioning status check
            if (ps == ES_PS_PROVISIONING_COMPLETED)
            {
                if(tnn != std::string(m_enrolleeNWProvInfo.netAddressInfo.WIFI.ssid))
                {
                    OC_LOG_V (ERROR, ES_REMOTE_ENROLLEE_TAG,
                            "checkProvInformationCb : Network SSID is not the same as the "
                            "SSID provisioned");
                    std::shared_ptr< ProvisioningStatus > provStatus = std::make_shared<
                            ProvisioningStatus >(ESResult::ES_ERROR,
                            ESState::ES_PROVISIONING_ERROR);
                            m_provStatusCb(provStatus);
                    return;
                }

                if(cd != std::string(m_enrolleeNWProvInfo.netAddressInfo.WIFI.pwd))
                {
                    OC_LOG_V (ERROR, ES_REMOTE_ENROLLEE_TAG,
                            "checkProvInformationCb : Network PWD is not the same as the "
                            "PWD provisioned");
                    std::shared_ptr< ProvisioningStatus > provStatus = std::make_shared<
                            ProvisioningStatus >(ESResult::ES_ERROR,
                            ESState::ES_PROVISIONING_ERROR);
                    m_provStatusCb(provStatus);
                    return;
                }

                OC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG,
                        "checkProvInformationCb : Provisioning is success ");
                std::shared_ptr< ProvisioningStatus > provStatus = std::make_shared<
                        ProvisioningStatus >(ESResult::ES_OK, ESState::ES_PROVISIONING_SUCCESS);
                m_provStatusCb(provStatus);
                return;
            }
            else
            {
                OC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG,
                        "checkProvInformationCb : Provisioning is failed ");
                std::shared_ptr< ProvisioningStatus > provStatus = std::make_shared<
                        ProvisioningStatus >(ESResult::ES_ERROR, ESState::ES_PROVISIONING_ERROR);
                m_provStatusCb(provStatus);
                return;
            }
        }

        void RemoteEnrolleeResource::getProvStatusResponse(const HeaderOptions& /*headerOptions*/,
                                                    const OCRepresentation& rep,
                                                    const int eCode)
        {
            OC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG, "getProvStatusResponse : %s, eCode = %d",
                    rep.getUri().c_str(),
                    eCode);

            if (eCode != 0)
            {
                OC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG,
                                            "getProvStatusResponse : Provisioning is failed ");
                std::shared_ptr< ProvisioningStatus > provStatus = std::make_shared<
                        ProvisioningStatus >(ESResult::ES_ERROR, ESState::ES_PROVISIONING_ERROR);
                m_provStatusCb(provStatus);
                return;
            }

            int ps = -1;
            std::string tnn = "";
            std::string cd = "";

            rep.getValue(OC_RSRVD_ES_PS, ps);
            rep.getValue(OC_RSRVD_ES_TNN, tnn);
            rep.getValue(OC_RSRVD_ES_CD, cd);

            OC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG, "getProvStatusResponse : ps - %d",
                                ps);
            OC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG, "getProvStatusResponse : tnn - %s",
                                            tnn.c_str());
            OC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG, "getProvStatusResponse : cd - %s",
                                            cd.c_str());

            if (ps == ES_PS_NEED_PROVISIONING) //Indicates the need for provisioning
            {
                OCRepresentation provisioningRepresentation;

                provisioningRepresentation.setValue(OC_RSRVD_ES_TNN,
                        std::string(m_enrolleeNWProvInfo.netAddressInfo.WIFI.ssid));
                provisioningRepresentation.setValue(OC_RSRVD_ES_CD,
                        std::string(m_enrolleeNWProvInfo.netAddressInfo.WIFI.pwd));

                OC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG, "getProvStatusResponse : ssid - %s",
                        m_enrolleeNWProvInfo.netAddressInfo.WIFI.ssid);
                OC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG, "getProvStatusResponse : pwd - %s",
                        m_enrolleeNWProvInfo.netAddressInfo.WIFI.pwd);

                m_ocResource->put(provisioningRepresentation, QueryParamsMap(),
                        std::function<
                                void(const HeaderOptions& headerOptions,
                                        const OCRepresentation& rep, const int eCode) >(
                                std::bind(&RemoteEnrolleeResource::checkProvInformationCb, this,
                                        std::placeholders::_1, std::placeholders::_2,
                                        std::placeholders::_3)));
            }
            else if(ps == ES_PS_PROVISIONING_COMPLETED) //Indicates that provisioning is completed
            {
                OC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG,
                        "getProvStatusResponse : Provisioning is successful");
                std::shared_ptr< ProvisioningStatus > provStatus = std::make_shared<
                        ProvisioningStatus >(ESResult::ES_OK, ESState::ES_PROVISIONED_ALREADY);
                m_provStatusCb(provStatus);
            }
        }

        void RemoteEnrolleeResource::registerProvStatusCallback (ProvStatusCb provStatusCb)
        {
            m_provStatusCb = provStatusCb;
        }

        void RemoteEnrolleeResource::constructResourceObject()
        {
            if (m_ocResource != nullptr)
            {
                throw ESBadRequestException("Remote resource is already created");
            }

            try
            {

                std::vector< std::string > m_if = { DEFAULT_INTERFACE };
                std::vector< std::string > m_resTypes = {ES_PROV_RES_TYPE};


                OC_LOG(DEBUG, ES_REMOTE_ENROLLEE_TAG, "Before OCPlatform::constructResourceObject");

                OC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG, "m_host = %s",
                                    m_enrolleeNWProvInfo.netAddressInfo.WIFI.ipAddress);
                OC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG, "ES_PROV_RES_URI = %s", ES_PROV_RES_URI);
                OC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG, "m_connectivityType = %d",
                                    m_enrolleeNWProvInfo.connType);
                OC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG, "m_resTypes = %s",
                                    m_resTypes.at(0).c_str());
                OC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG, "m_if = %s", m_if.at(0).c_str());

                std::string host;
                if(m_enrolleeNWProvInfo.needSecuredEasysetup)
                {
                    host.append("coaps://");
                }
                else
                {
                    host.append("coap://");
                }

                if(m_enrolleeNWProvInfo.connType == CT_ADAPTER_IP)
                {
                    // TODO : RemoteEnrollee is current handling easysetup on IP transport.
                    // WiFiRemoteEnrollee need to extend RemoteEnrollee for providing IP specific
                    // Enrollee easysetup.

                    host.append(m_enrolleeNWProvInfo.netAddressInfo.WIFI.ipAddress);
                    //TODO : If the target Enrollee is not a Arduino Wi-Fi device,
                    // then the port number will be found during resource discovery instead of
                    // using 55555
                    host.append(":55555");
                }

                OC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_TAG, "HOST = %s", host.c_str());

                m_ocResource = OC::OCPlatform::constructResourceObject(host,
                                                                   ES_PROV_RES_URI,
                                                                   m_enrolleeNWProvInfo.connType,
                                                                   true,
                                                                   m_resTypes,
                                                                   m_if);
                OC_LOG_V(DEBUG, ES_REMOTE_ENROLLEE_TAG,
                        "created OCResource : %s", m_ocResource->uri().c_str());
            }
            catch (OCException & e)
            {
                OC_LOG_V(ERROR, ES_REMOTE_ENROLLEE_TAG,
                        "Exception for constructResourceObject : %s", e.reason().c_str());
            }
        }

        void RemoteEnrolleeResource::provisionEnrollee()

        {
            if(m_ocResource == nullptr)
            {
                throw ESBadRequestException ("Resource is not initialized");
            }

            OC::QueryParamsMap query;
            OC::OCRepresentation rep;

            std::function <OCStackResult(void)> getProvisioingStatus = [&]
            {   return m_ocResource->get(m_ocResource->getResourceTypes().at(0),
                        m_ocResource->getResourceInterfaces().at(0), query,
                        std::function<
                        void(const HeaderOptions& headerOptions, const OCRepresentation& rep,
                                const int eCode) >(
                                std::bind(&RemoteEnrolleeResource::getProvStatusResponse, this,
                                        std::placeholders::_1, std::placeholders::_2,
                                        std::placeholders::_3)));
            };

            OCStackResult result = getProvisioingStatus();

            if (result != OCStackResult::OC_STACK_OK)
            {
                std::shared_ptr< ProvisioningStatus > provStatus = std::make_shared<
                        ProvisioningStatus >(ESResult::ES_ERROR, ESState::ES_PROVISIONING_ERROR);
                m_provStatusCb(provStatus);
                return;
            }
        }

        void RemoteEnrolleeResource::unprovisionEnrollee()
        {
            if (m_ocResource == nullptr)
            {
                throw ESBadRequestException("Resource is not initialized");
            }

            OCRepresentation provisioningRepresentation;

            provisioningRepresentation.setValue(OC_RSRVD_ES_TNN, "");
            provisioningRepresentation.setValue(OC_RSRVD_ES_CD, "");

            m_ocResource->post(provisioningRepresentation, QueryParamsMap(),
                    std::function<
                            void(const HeaderOptions& headerOptions, const OCRepresentation& rep,
                                    const int eCode) >(
                            std::bind(&RemoteEnrolleeResource::checkProvInformationCb, this,
                                    std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3)));
        }
    }
}
