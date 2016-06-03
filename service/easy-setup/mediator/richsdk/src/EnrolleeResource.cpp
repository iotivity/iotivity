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
#include <time.h>

#include "EnrolleeResource.h"

#include "OCPlatform.h"
#include "ESException.h"
#include "OCResource.h"
#include "logger.h"

namespace OIC
{
    namespace Service
    {
        #define ES_REMOTE_ENROLLEE_RES_TAG "ES_REMOTE_ENROLLEE_RES"
        #define DISCOVERY_TIMEOUT 5

        static const char ES_BASE_RES_URI[] = "/oic/res";
        static const char ES_PROV_RES_URI[] = "/oic/prov";
        static const char ES_PROV_RES_TYPE[] = "oic.r.prov";

        EnrolleeResource::EnrolleeResource(const WiFiOnboadingConnection &onboardingconn)
        {
            m_wifiOnboardingconn = onboardingconn;
            m_discoveryResponse = false;
        }
/*
        EnrolleeResource::EnrolleeResource(const ProvConfig &provConfig,
                                                  const WiFiOnboadingConnection &onboardingconn)
        {
            m_ProvConfig = provConfig;
            m_wifiOnboardingconn = onboardingconn;
            m_discoveryResponse = false;
        }
*/
        void EnrolleeResource::triggerNetworkConnectionCb(
                const HeaderOptions& /*headerOptions*/, const OCRepresentation& rep,
                const int eCode)
        {
            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "checkProvInformationCb : %s, eCode = %d",
                    rep.getUri().c_str(),
                    eCode);

            if (eCode != 0)
            {
                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                        "triggerNetworkConnectionCb : Trigger action failed ");
                std::shared_ptr< ProvisioningStatus > provStatus = std::make_shared<
                        ProvisioningStatus >(ESResult::ES_ERROR, ESState::ES_PROVISIONING_ERROR);
                m_dataProvStatusCb(provStatus);
                return;
            }
            else
            {
                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                        "triggerNetworkConnectionCb : Provisioning is success ");
                std::shared_ptr< ProvisioningStatus > provStatus = std::make_shared<
                        ProvisioningStatus >(ESResult::ES_OK, ESState::ES_PROVISIONING_SUCCESS);
                m_dataProvStatusCb(provStatus);
                return;
            }
        }

        void EnrolleeResource::triggerNetworkConnection()
        {
            if (m_ocResource == nullptr)
            {
                throw ESBadRequestException("Resource is not initialized");
            }

            OCRepresentation provisioningRepresentation;

            provisioningRepresentation.setValue(OC_RSRVD_ES_TRIGGER, 1);

            m_ocResource->post(provisioningRepresentation, QueryParamsMap(),
                    std::function<
                            void(const HeaderOptions& headerOptions, const OCRepresentation& rep,
                                    const int eCode) >(
                            std::bind(&EnrolleeResource::triggerNetworkConnectionCb, this,
                                    std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3)));
        }

        void EnrolleeResource::checkProvInformationCb(const HeaderOptions& /*headerOptions*/,
                const OCRepresentation& rep, const int eCode)
        {
            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "checkProvInformationCb : %s, eCode = %d",
                    rep.getUri().c_str(),
                    eCode);

            if (eCode != 0)
            {
                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                        "checkProvInformationCb : Provisioning is failed ");
                std::shared_ptr< ProvisioningStatus > provStatus = std::make_shared<
                        ProvisioningStatus >(ESResult::ES_ERROR, ESState::ES_PROVISIONING_ERROR);
                m_dataProvStatusCb(provStatus);
                return;
            }

            int ps = -1;

            rep.getValue(OC_RSRVD_ES_PROVSTATUS, ps);

            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "checkProvInformationCb : ps - %d", ps);

            //Provisioning status check
            if (ps == ES_PS_PROVISIONING_COMPLETED)
            {
                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                        "checkProvInformationCb : Provisioning is success. "
                        "Now trigger network connection ");

                #ifdef REMOTE_ARDUINO_ENROLEE
                 std::shared_ptr< ProvisioningStatus > provStatus = std::make_shared<
                        ProvisioningStatus >(ESResult::ES_OK, ESState::ES_PROVISIONING_SUCCESS);
                m_dataProvStatusCb(provStatus);
                #endif

                triggerNetworkConnection();
                return;
            }
            else
            {
                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                        "checkProvInformationCb : Provisioning is failed ");
                std::shared_ptr< ProvisioningStatus > provStatus = std::make_shared<
                        ProvisioningStatus >(ESResult::ES_ERROR, ESState::ES_PROVISIONING_ERROR);
                m_dataProvStatusCb(provStatus);
                return;
            }
        }

        void EnrolleeResource::getCapabilityResponse(const HeaderOptions& /*headerOptions*/,
                const OCRepresentation& rep, const int eCode)
        {
            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getCapabilityResponse : %s, eCode = %d",
                    rep.getUri().c_str(), eCode);

            if (eCode != 0)
            {
                ESResult result  = ESResult::ES_ERROR;

                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,"getCapabilityResponse : getCapabilityResponse is failed ");

                if (eCode == OCStackResult::OC_STACK_UNAUTHORIZED_REQ)
                {
                    OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                        "Mediator is unauthorized from Enrollee.");
                    result = ESResult::ES_UNAUTHORIZED;
                }

                CapabilityData capabilityData;
                std::shared_ptr< RequestCapabilityStatus > requestCapabilityStatus = std::make_shared<
                        RequestCapabilityStatus >(result, capabilityData);
                m_requestCapabilityStatusCb(requestCapabilityStatus);

                return;
            }

            CapabilityData capabilityData = CapabilityData();
            std::shared_ptr< RequestCapabilityStatus > requestCapabilityStatus = std::make_shared<
                    RequestCapabilityStatus >(ESResult::ES_OK, capabilityData);
            m_requestCapabilityStatusCb(requestCapabilityStatus);
        }

        void EnrolleeResource::getProvStatusResponse(const HeaderOptions& /*headerOptions*/,
                const OCRepresentation& rep, const int eCode)
        {
            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getProvStatusResponse : %s, eCode = %d",
                    rep.getUri().c_str(),
                    eCode);

            if (eCode != 0)
            {
                ESResult result  = ESResult::ES_ERROR;

                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,"getProvStatusResponse : Provisioning is failed ");

                if (eCode == OCStackResult::OC_STACK_UNAUTHORIZED_REQ)
                {
                    OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                        "Mediator is unauthorized from Enrollee.");
                    result = ESResult::ES_UNAUTHORIZED;
                }
                std::shared_ptr< ProvisioningStatus > provStatus = std::make_shared<
                        ProvisioningStatus >(result, ESState::ES_PROVISIONING_ERROR);
                m_dataProvStatusCb(provStatus);

                return;
            }

            int ps = -1;

            rep.getValue(OC_RSRVD_ES_PROVSTATUS, ps);

            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getProvStatusResponse : ps - %d",
                    ps);

            if (ps == ES_PS_NEED_PROVISIONING) //Indicates the need for provisioning
            {
                OCRepresentation provisioningRepresentation;

                provisioningRepresentation.setValue(OC_RSRVD_ES_SSID,
                std::string(m_ProvConfig.provData.WIFI.ssid));
                provisioningRepresentation.setValue(OC_RSRVD_ES_CRED,
                std::string(m_ProvConfig.provData.WIFI.pwd));
                provisioningRepresentation.setValue(OC_RSRVD_ES_AUTHCODE,
                std::string(m_ProvConfig.provData.WIFI.authcode));
                provisioningRepresentation.setValue(OC_RSRVD_ES_AUTHPROVIDER,
                std::string(m_ProvConfig.provData.WIFI.authserverUrl));
                provisioningRepresentation.setValue(OC_RSRVD_ES_CISERVER,
                std::string(m_ProvConfig.provData.WIFI.apiserverUrl));
                provisioningRepresentation.setValue( OC_RSRVD_ES_AUTHTYPE,
                m_ProvConfig.provData.WIFI.authtype);
                provisioningRepresentation.setValue(OC_RSRVD_ES_ENCTYPE,
                m_ProvConfig.provData.WIFI.enctype);

                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getProvStatusResponse : ssid - %s",
                        m_ProvConfig.provData.WIFI.ssid);
                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getProvStatusResponse : pwd - %s",
                        m_ProvConfig.provData.WIFI.pwd);
                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getProvStatusResponse : AuthCode - %s",
                        m_ProvConfig.provData.WIFI.authcode);
                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getProvStatusResponse : AuthServerUrl - %s",
                        m_ProvConfig.provData.WIFI.authserverUrl);
                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getProvStatusResponse : APIServerUrl - %s",
                        m_ProvConfig.provData.WIFI.apiserverUrl);
                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getProvStatusResponse : authtype - %d",
                        m_ProvConfig.provData.WIFI.authtype);
                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getProvStatusResponse : enctype - %d",
                        m_ProvConfig.provData.WIFI.enctype);

                m_ocResource->post(provisioningRepresentation, QueryParamsMap(),
                        std::function<
                                void(const HeaderOptions& headerOptions,
                                        const OCRepresentation& rep, const int eCode) >(
                        std::bind(&EnrolleeResource::checkProvInformationCb, this,
                        std::placeholders::_1, std::placeholders::_2,
                        std::placeholders::_3)));
            }
            else if (ps == ES_PS_PROVISIONING_COMPLETED) //Indicates that provisioning is completed
            {
                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                        "getProvStatusResponse : Provisioning is successful");
                std::shared_ptr< ProvisioningStatus > provStatus = std::make_shared<
                        ProvisioningStatus >(ESResult::ES_OK, ESState::ES_PROVISIONED_ALREADY);
                m_dataProvStatusCb(provStatus);
            }
        }

        void EnrolleeResource::registerInitRemoteEnrolleeStatusCallback (InitRemoteEnrolleeStatusCb callback)
        {
            m_initRemoteEnrolleeStatusCb = callback;
        }

        void EnrolleeResource::registerCapabilityStatusCallback(RequestCapabilityStatusCb callback)
        {
            m_requestCapabilityStatusCb = callback;
        }

        void EnrolleeResource::registerProvStatusCallback(DataProvStatusCb callback)
        {
            m_dataProvStatusCb = callback;
        }

        ESResult EnrolleeResource::ESDiscoveryTimeout(unsigned short waittime)
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
                    std::shared_ptr< InitRemoteEnrolleeStatus > initRemoteEnrolleeStatus = std::make_shared<
                        InitRemoteEnrolleeStatus >(ESResult::ES_ERROR);
                    m_initRemoteEnrolleeStatusCb(initRemoteEnrolleeStatus);
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

        void EnrolleeResource::onDeviceDiscovered(std::shared_ptr<OC::OCResource> resource)
        {
            OIC_LOG (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "onDeviceDiscovered");

            std::string resourceURI;
            std::string hostAddress;
            std::string hostDeviceID;
            try
            {
                if(resource)
                {
                    // Get the resource URI
                    resourceURI = resource->uri();
                    OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                            "URI of the resource: %s", resourceURI.c_str());

                    // Get the resource host address
                    hostAddress = resource->host();
                    OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                            "Host address of the resource: %s", hostAddress.c_str());

                    hostDeviceID = resource->sid();
                    OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                            "Host DeviceID of the resource: %s", hostDeviceID.c_str());
                    /*
                     * Easysetup is always performed with a single Enrollee device and
                     * in a private network (SoftAP or BLE), so the assumption is that
                     * only the intended device will respond for the discovery.
                     * With the above assumption the below two statements are written.
                     */
                    m_ocResource = resource;
                    m_discoveryResponse = true;

                    std::shared_ptr< InitRemoteEnrolleeStatus > initRemoteEnrolleeStatus = std::make_shared<
                        InitRemoteEnrolleeStatus >(ESResult::ES_OK);
                    m_initRemoteEnrolleeStatusCb(initRemoteEnrolleeStatus);
                }
                else
                {
                    OIC_LOG (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "Resource is invalid");

                    std::shared_ptr< InitRemoteEnrolleeStatus > initRemoteEnrolleeStatus = std::make_shared<
                        InitRemoteEnrolleeStatus >(ESResult::ES_ERROR);
                    m_initRemoteEnrolleeStatusCb(initRemoteEnrolleeStatus);
                }

            }
            catch(std::exception& e)
            {
                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                        "Exception in foundResource: %s", e.what());
            }
        }


        ESResult EnrolleeResource::constructResourceObject()
        {
            if (m_ocResource != nullptr)
            {
                throw ESBadRequestException("Remote resource is already created");
            }

#ifdef REMOTE_ARDUINO_ENROLEE
            //This process will create OCResource with port 55555 which is specific
            // to Arduino WiFi enrollee
            try
            {

                std::vector< std::string > interface =
                {   DEFAULT_INTERFACE};
                std::vector< std::string > resTypes =
                {   ES_PROV_RES_TYPE};

                OIC_LOG(DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "Before OCPlatform::constructResourceObject");

                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "m_host = %s",
                        m_wifiOnboardingconn.ipAddress);
                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "ES_PROV_RES_URI = %s", ES_PROV_RES_URI);
                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "m_connectivityType = %d",
                        m_ProvConfig.connType);
                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "resTypes = %s",
                        resTypes.at(0).c_str());
                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "interface = %s", interface.at(0).c_str());

                std::string host;
                if(m_wifiOnboardingconn.isSecured)
                {
                    host.append("coaps://");
                }
                else
                {
                    host.append("coap://");
                }

                if(m_ProvConfig.connType == CT_ADAPTER_IP)
                {
                    // TODO : RemoteEnrollee is current handling easysetup on IP transport.
                    // WiFiRemoteEnrollee need to extend RemoteEnrollee for providing IP specific
                    // Enrollee easysetup.

                    host.append(m_wifiOnboardingconn.ipAddress);
                    //TODO : If the target Enrollee is not a Arduino Wi-Fi device,
                    // then the port number will be found during resource discovery instead of
                    // using 55555
                    host.append(":55555");
                }

                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "HOST = %s", host.c_str());

                m_ocResource = OC::OCPlatform::constructResourceObject(host,
                        ES_PROV_RES_URI,
                        m_ProvConfig.connType,
                        true,
                        resTypes,
                        interface);
                OIC_LOG_V(DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                        "created OCResource : %s", m_ocResource->uri().c_str());

                return ES_OK;
            }
            catch (OCException & e)
            {
                OIC_LOG_V(ERROR, ES_REMOTE_ENROLLEE_RES_TAG,
                        "Exception for constructResourceObject : %s", e.reason().c_str());
                return ES_ERROR;
            }
#else
            std::string host("");
            std::string query("");

            if (m_wifiOnboardingconn.isSecured)
            {
                host.append("coaps://");
            }
            else
            {
                host.append("coap://");
            }

            query.append(ES_BASE_RES_URI);
            query.append("?rt=");
            query.append(ES_PROV_RES_TYPE);

            OIC_LOG(DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "Before OCPlatform::constructResourceObject");

            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "host = %s", host.c_str());
            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "query = %s", query.c_str());
            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "m_connectivityType = %d",
                    m_ProvConfig.connType);

            m_discoveryResponse = false;
            std::function< void (std::shared_ptr<OC::OCResource>) > onDeviceDiscoveredCb =
                    std::bind(&EnrolleeResource::onDeviceDiscovered, this,
                                                    std::placeholders::_1);
            OCStackResult result = OC::OCPlatform::findResource("", query, CT_DEFAULT,
                    onDeviceDiscoveredCb);

            if (result != OCStackResult::OC_STACK_OK)
            {
                OIC_LOG(ERROR,ES_REMOTE_ENROLLEE_RES_TAG,
                        "Failed to create device using constructResourceObject");
                return ES_ERROR;
            }


            ESResult foundResponse = ESDiscoveryTimeout (DISCOVERY_TIMEOUT);

            if (foundResponse ==ES_ERROR || !m_discoveryResponse)
            {
                OIC_LOG(ERROR,ES_REMOTE_ENROLLEE_RES_TAG,
                        "Failed to create device using constructResourceObject");
                return ES_ERROR;
            }

            return ES_OK;
#endif
        }

        void EnrolleeResource::getCapabilityData()
        {
            if (m_ocResource == nullptr)
            {
                throw ESBadRequestException("Resource is not initialized");
            }

            OC::QueryParamsMap query;
            OC::OCRepresentation rep;

            std::function< OCStackResult(void) > getCapabilityStatus = [&]
            {   return m_ocResource->get(m_ocResource->getResourceTypes().at(0),
                        BATCH_INTERFACE, query, std::function<void(const HeaderOptions& headerOptions,
                        const OCRepresentation& rep, const int eCode) >(
                                std::bind(&EnrolleeResource::getCapabilityResponse, this,
                                        std::placeholders::_1, std::placeholders::_2,
                                        std::placeholders::_3)));
            };

            OCStackResult result = getCapabilityStatus();

            if (result != OCStackResult::OC_STACK_OK)
            {
                CapabilityData capabilityData;
                std::shared_ptr< RequestCapabilityStatus > requestCapabilityStatus = std::make_shared<
                        RequestCapabilityStatus >(ESResult::ES_ERROR, capabilityData);
                m_requestCapabilityStatusCb(requestCapabilityStatus);
                return;
            }
        }

        void EnrolleeResource::provisionEnrollee()

        {
            if (m_ocResource == nullptr)
            {
                throw ESBadRequestException("Resource is not initialized");
            }

            OC::QueryParamsMap query;
            OC::OCRepresentation rep;

            std::function< OCStackResult(void) > getProvisioingStatus = [&]
            {   return m_ocResource->get(m_ocResource->getResourceTypes().at(0),
                        m_ocResource->getResourceInterfaces().at(0), query,
                        std::function<
                        void(const HeaderOptions& headerOptions, const OCRepresentation& rep,
                                const int eCode) >(
                                std::bind(&EnrolleeResource::getProvStatusResponse, this,
                                        std::placeholders::_1, std::placeholders::_2,
                                        std::placeholders::_3)));
            };

            OCStackResult result = getProvisioingStatus();

            if (result != OCStackResult::OC_STACK_OK)
            {
                std::shared_ptr< ProvisioningStatus > provStatus = std::make_shared<
                        ProvisioningStatus >(ESResult::ES_ERROR, ESState::ES_PROVISIONING_ERROR);
                m_dataProvStatusCb(provStatus);
                return;
            }
        }

        void EnrolleeResource::unprovisionEnrollee()
        {
            if (m_ocResource == nullptr)
            {
                throw ESBadRequestException("Resource is not initialized");
            }

            OCRepresentation provisioningRepresentation;

            provisioningRepresentation.setValue(OC_RSRVD_ES_SSID, "");
            provisioningRepresentation.setValue(OC_RSRVD_ES_CRED, "");

            m_ocResource->post(provisioningRepresentation, QueryParamsMap(),
                    std::function<
                            void(const HeaderOptions& headerOptions, const OCRepresentation& rep,
                                    const int eCode) >(
                    std::bind(&EnrolleeResource::checkProvInformationCb, this,
                    std::placeholders::_1, std::placeholders::_2,
                    std::placeholders::_3)));
        }
    }
}
