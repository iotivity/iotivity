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

#include "EnrolleeResource.h"

#include "OCPlatform.h"
#include "ESException.h"
#include "OCResource.h"
#include "logger.h"

namespace OIC
{
    namespace Service
    {
        #define ES_REMOTE_ENROLLEE_RES_TAG "ES_ENROLLEE_RESOURCE"

        EnrolleeResource::EnrolleeResource(std::shared_ptr< OC::OCResource > resource)
        {
            m_ocResource = resource;
        }

        void EnrolleeResource::checkProvInformationCb(const HeaderOptions& /*headerOptions*/,
                const OCRepresentation& rep, const int eCode)
        {
            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "checkProvInformationCb : %s, eCode = %d",
                    rep.getUri().c_str(),
                    eCode);

            if (eCode > OCStackResult::OC_STACK_RESOURCE_CHANGED)
            {
                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                        "checkProvInformationCb : Provisioning is failed ");
                std::shared_ptr< DevicePropProvisioningStatus > provStatus = std::make_shared<
                        DevicePropProvisioningStatus >(ESResult::ES_ERROR);
                m_devicePropProvStatusCb(provStatus);
                return;
            }

            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                    "checkProvInformationCb : Provisioning is success. ");

            std::shared_ptr< DevicePropProvisioningStatus > provStatus = std::make_shared<
                    DevicePropProvisioningStatus >(ESResult::ES_OK);
            m_devicePropProvStatusCb(provStatus);
        }

        void EnrolleeResource::onGetStatusResponse(const HeaderOptions& /*headerOptions*/,
                const OCRepresentation& rep, const int eCode)
        {
            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "onGetStatusResponse : %s, eCode = %d",
                    rep.getUri().c_str(), eCode);

            if (eCode > OCStackResult::OC_STACK_RESOURCE_CHANGED)
            {
                ESResult result = ESResult::ES_ERROR;

                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                            "onGetStatusResponse : onGetStatusResponse is failed ");

                if (eCode == OCStackResult::OC_STACK_UNAUTHORIZED_REQ)
                {
                    OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                        "Mediator is unauthorized from Enrollee.");
                    result = ESResult::ES_UNAUTHORIZED;
                }

                EnrolleeStatus enrolleeStatus = {ES_STATE_INIT, ES_ERRCODE_NO_ERROR};
                std::shared_ptr< GetEnrolleeStatus > getEnrolleeStatus = std::make_shared<
                        GetEnrolleeStatus >(ESResult::ES_ERROR, enrolleeStatus);

                m_getStatusCb(getEnrolleeStatus);
            }
            else
            {
                EnrolleeStatus enrolleeStatus = parseEnrolleeStatusFromRepresentation(rep);
                std::shared_ptr< GetEnrolleeStatus > getEnrolleeStatus = std::make_shared<
                        GetEnrolleeStatus >(ESResult::ES_OK, enrolleeStatus);

                m_getStatusCb(getEnrolleeStatus);
            }
        }

        void EnrolleeResource::onGetConfigurationResponse(const HeaderOptions& /*headerOptions*/,
                const OCRepresentation& rep, const int eCode)
        {
            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "onGetConfigurationResponse : %s, eCode = %d",
                    rep.getUri().c_str(), eCode);

            if (eCode > OCStackResult::OC_STACK_RESOURCE_CHANGED)
            {
                ESResult result  = ESResult::ES_ERROR;

                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                            "onGetConfigurationResponse : onGetConfigurationResponse is failed ");

                if (eCode == OCStackResult::OC_STACK_UNAUTHORIZED_REQ)
                {
                    OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                        "Mediator is unauthorized from Enrollee.");
                    result = ESResult::ES_UNAUTHORIZED;
                }

                EnrolleeConf enrolleeConf;
                std::shared_ptr< GetConfigurationStatus > getConfigurationStatus = std::make_shared<
                        GetConfigurationStatus >(result, enrolleeConf);
                m_getConfigurationStatusCb(getConfigurationStatus);
            }
            else
            {
                EnrolleeConf enrolleeConf = parseEnrolleeConfFromRepresentation(rep);

                std::shared_ptr< GetConfigurationStatus > getConfigurationStatus = std::make_shared<
                        GetConfigurationStatus >(ESResult::ES_OK, enrolleeConf);
                m_getConfigurationStatusCb(getConfigurationStatus);
            }
        }

        void EnrolleeResource::registerGetStatusCallback(GetStatusCb callback)
        {
            m_getStatusCb = callback;
        }

        void EnrolleeResource::registerGetConfigurationStatusCallback(GetConfigurationStatusCb callback)
        {
            m_getConfigurationStatusCb = callback;
        }

        void EnrolleeResource::registerDevicePropProvStatusCallback(DevicePropProvStatusCb callback)
        {
            m_devicePropProvStatusCb = callback;
        }

        void EnrolleeResource::getStatus()
        {
            if (m_ocResource == nullptr)
            {
                throw ESBadRequestException("Resource is not initialized");
            }

            OC::QueryParamsMap query;
            OC::OCRepresentation rep;

            std::function< OCStackResult(void) > getStatus = [&]
            {   return m_ocResource->get(m_ocResource->getResourceTypes().at(0),
                        DEFAULT_INTERFACE, query, std::function<void(const HeaderOptions& headerOptions,
                        const OCRepresentation& rep, const int eCode) >(
                                std::bind(&EnrolleeResource::onGetStatusResponse, this,
                                        std::placeholders::_1, std::placeholders::_2,
                                        std::placeholders::_3)));
            };

            OCStackResult result = getStatus();

            if (result != OCStackResult::OC_STACK_OK)
            {
                EnrolleeStatus enrolleeStatus = {ES_STATE_INIT, ES_ERRCODE_NO_ERROR};
                std::shared_ptr< GetEnrolleeStatus > getEnrolleeStatus = std::make_shared<
                        GetEnrolleeStatus >(ESResult::ES_ERROR, enrolleeStatus);

                m_getStatusCb(getEnrolleeStatus);

                return;
            }
        }

        void EnrolleeResource::getConfiguration()
        {
            if (m_ocResource == nullptr)
            {
                throw ESBadRequestException("Resource is not initialized");
            }

            OC::QueryParamsMap query;
            OC::OCRepresentation rep;

            std::function< OCStackResult(void) > getConfigurationStatus = [&]
            {   return m_ocResource->get(m_ocResource->getResourceTypes().at(0),
                        BATCH_INTERFACE, query, std::function<void(const HeaderOptions& headerOptions,
                        const OCRepresentation& rep, const int eCode) >(
                                std::bind(&EnrolleeResource::onGetConfigurationResponse, this,
                                        std::placeholders::_1, std::placeholders::_2,
                                        std::placeholders::_3)));
            };

            OCStackResult result = getConfigurationStatus();

            if (result != OCStackResult::OC_STACK_OK)
            {
                EnrolleeConf enrolleeConf;
                std::shared_ptr< GetConfigurationStatus > getConfigurationStatus = std::make_shared<
                        GetConfigurationStatus >(ESResult::ES_ERROR, enrolleeConf);
                m_getConfigurationStatusCb(getConfigurationStatus);
                return;
            }
        }

        void EnrolleeResource::provisionEnrollee(const DeviceProp& deviceProp)

        {
            if (m_ocResource == nullptr)
            {
                throw ESBadRequestException("Resource is not initialized");
            }

            OC::QueryParamsMap query;
            OC::OCRepresentation provisioningRepresentation;

            provisioningRepresentation.setValue(OC_RSRVD_ES_SSID, deviceProp.WIFI.ssid);
            provisioningRepresentation.setValue(OC_RSRVD_ES_CRED, deviceProp.WIFI.pwd);
            provisioningRepresentation.setValue(OC_RSRVD_ES_AUTHTYPE, deviceProp.WIFI.authtype);
            provisioningRepresentation.setValue(OC_RSRVD_ES_ENCTYPE, deviceProp.WIFI.enctype);
            provisioningRepresentation.setValue(OC_RSRVD_ES_LANGUAGE, deviceProp.Device.language);
            provisioningRepresentation.setValue(OC_RSRVD_ES_COUNTRY, deviceProp.Device.country);

            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getProvStatusResponse : ssid - %s",
                    (deviceProp.WIFI.ssid).c_str());
            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getProvStatusResponse : pwd - %s",
                    (deviceProp.WIFI.pwd).c_str());
            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getProvStatusResponse : authtype - %d",
                    deviceProp.WIFI.authtype);
            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getProvStatusResponse : enctype - %d",
                    deviceProp.WIFI.enctype);
            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getProvStatusResponse : language - %s",
                    (deviceProp.Device.language).c_str());
            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getProvStatusResponse : country - %s",
                    (deviceProp.Device.country).c_str());

            m_ocResource->post(OC_RSRVD_ES_RES_TYPE_PROV, BATCH_INTERFACE,
                    provisioningRepresentation, QueryParamsMap(),
                    std::function<
                            void(const HeaderOptions& headerOptions,
                                    const OCRepresentation& rep, const int eCode) >(
                    std::bind(&EnrolleeResource::checkProvInformationCb, this,
                    std::placeholders::_1, std::placeholders::_2,
                    std::placeholders::_3)));
        }

        EnrolleeConf EnrolleeResource::parseEnrolleeConfFromRepresentation(const OCRepresentation& rep)
        {
            OIC_LOG(DEBUG,ES_REMOTE_ENROLLEE_RES_TAG, "Enter parseEnrolleeConfFromRepresentation");

            DeviceConfig devConf;
            WiFiConfig wifiConf;
            bool cloudable = false;

            std::vector<OCRepresentation> children = rep.getChildren();

            for(auto prop = children.begin(); prop != children.end(); ++prop)
            {
                if(prop->getUri().find(OC_RSRVD_ES_URI_WIFI) != std::string::npos)
                {
                    OIC_LOG(DEBUG,ES_REMOTE_ENROLLEE_RES_TAG, "Find wifi resource");
                    if(prop->hasAttribute(OC_RSRVD_ES_SUPPORTEDWIFIMODE)
                                && prop->hasAttribute(OC_RSRVD_ES_SUPPORTEDWIFIFREQ))
                    {
                        std::vector<int> modes = prop->getValue<std::vector<int>>(OC_RSRVD_ES_SUPPORTEDWIFIMODE);

                        for(auto mode = modes.begin(); mode != modes.end(); ++mode)
                        {
                            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "OC_RSRVD_ES_SUPPORTEDWIFIMODE = %d",
                                *mode);
                            wifiConf.modes.push_back(static_cast<WIFI_MODE>(*mode));
                        }

                        wifiConf.freq = static_cast<WIFI_FREQ>(prop->getValue<int>(OC_RSRVD_ES_SUPPORTEDWIFIFREQ));


                        OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "OC_RSRVD_ES_SUPPORTEDWIFIFREQ = %d",
                                wifiConf.freq);
                    }
                }

                else if(prop->getUri().find(OC_RSRVD_ES_URI_DEVCONF) != std::string::npos)
                {
                    OIC_LOG(DEBUG,ES_REMOTE_ENROLLEE_RES_TAG, "Find devconf");
                    if(prop->hasAttribute(OC_RSRVD_ES_DEVNAME)
                                && prop->hasAttribute(OC_RSRVD_ES_LANGUAGE)
                                && prop->hasAttribute(OC_RSRVD_ES_COUNTRY))
                    {
                        //TODO:: setting DeviceID.
                        //devInfo.id = devId;
                        devConf.name = prop->getValue<std::string>(OC_RSRVD_ES_DEVNAME);
                        devConf.language = prop->getValue<std::string>(OC_RSRVD_ES_LANGUAGE);
                        devConf.country = prop->getValue<std::string>(OC_RSRVD_ES_COUNTRY);

                        OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "OC_RSRVD_ES_DEVNAME = %s",
                                devConf.name.c_str());
                        OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "OC_RSRVD_ES_LANGUAGE = %s",
                                devConf.language.c_str());
                        OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "OC_RSRVD_ES_COUNTRY = %s",
                                devConf.country.c_str());
                    }
                }

                else if(prop->getUri().find(OC_RSRVD_ES_URI_CLOUDSERVER) != std::string::npos)
                {
                    OIC_LOG(DEBUG,ES_REMOTE_ENROLLEE_RES_TAG, "Find cloudserver");
                    cloudable = true;
                    OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "cloudable = %d",
                                cloudable);
                }
            }

            return EnrolleeConf(devConf, wifiConf, cloudable);
        }

        EnrolleeStatus EnrolleeResource::parseEnrolleeStatusFromRepresentation(const OCRepresentation& rep)
        {
            OIC_LOG(DEBUG,ES_REMOTE_ENROLLEE_RES_TAG, "Enter parseEnrolleeStatusFromRepresentation");

            EnrolleeStatus enrolleeStatus;

            if(rep.hasAttribute(OC_RSRVD_ES_PROVSTATUS))
            {
                enrolleeStatus.provStatus = static_cast<ProvStatus>(
                                                        rep.getValue<int>(OC_RSRVD_ES_PROVSTATUS));
            }

            if(rep.hasAttribute(OC_RSRVD_ES_LAST_ERRORCODE))
            {
                enrolleeStatus.lastErrCode = static_cast<ESErrorCode>(
                                                        rep.getValue<int>(OC_RSRVD_ES_LAST_ERRORCODE));
            }

            return enrolleeStatus;
        }
    }
}
