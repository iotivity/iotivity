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

            if (eCode != 0)
            {
                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                        "checkProvInformationCb : Provisioning is failed ");
                std::shared_ptr< DataProvisioningStatus > provStatus = std::make_shared<
                        DataProvisioningStatus >(ESResult::ES_ERROR, ESDataProvState::ES_PROVISIONING_ERROR);
                m_dataProvStatusCb(provStatus);
                return;
            }

            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                    "checkProvInformationCb : Provisioning is success. ");

            std::shared_ptr< DataProvisioningStatus > provStatus = std::make_shared<
                    DataProvisioningStatus >(ESResult::ES_OK, ESDataProvState::ES_PROVISIONING_SUCCESS);
            m_dataProvStatusCb(provStatus);
        }

        void EnrolleeResource::onRequestPropertyDataResponse(const HeaderOptions& /*headerOptions*/,
                const OCRepresentation& rep, const int eCode)
        {
            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "onRequestPropertyDataResponse : %s, eCode = %d",
                    rep.getUri().c_str(), eCode);

            if (eCode != 0)
            {
                ESResult result  = ESResult::ES_ERROR;

                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,"onRequestPropertyDataResponse : onRequestPropertyDataResponse is failed ");

                if (eCode == OCStackResult::OC_STACK_UNAUTHORIZED_REQ)
                {
                    OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                        "Mediator is unauthorized from Enrollee.");
                    result = ESResult::ES_UNAUTHORIZED;
                }

                PropertyData propertyData;
                std::shared_ptr< RequestPropertyDataStatus > requestPropertyDataStatus = std::make_shared<
                        RequestPropertyDataStatus >(result, propertyData );
                m_RequestPropertyDataStatusCb(requestPropertyDataStatus);
            }
            else
            {
                PropertyData propertyData = parsePropertyDataFromRepresentation(rep);

                std::shared_ptr< RequestPropertyDataStatus > requestPropertyDataStatus = std::make_shared<
                        RequestPropertyDataStatus >(ESResult::ES_OK, propertyData);
                m_RequestPropertyDataStatusCb(requestPropertyDataStatus);
            }
        }

        void EnrolleeResource::registerRequestPropertyDataStatusCallback(RequestPropertyDataStatusCb callback)
        {
            m_RequestPropertyDataStatusCb = callback;
        }

        void EnrolleeResource::registerProvStatusCallback(DataProvStatusCb callback)
        {
            m_dataProvStatusCb = callback;
        }

        void EnrolleeResource::RequestPropertyData()
        {
            if (m_ocResource == nullptr)
            {
                throw ESBadRequestException("Resource is not initialized");
            }

            OC::QueryParamsMap query;
            OC::OCRepresentation rep;

            std::function< OCStackResult(void) > requestPropertyDataStatus = [&]
            {   return m_ocResource->get(m_ocResource->getResourceTypes().at(0),
                        BATCH_INTERFACE, query, std::function<void(const HeaderOptions& headerOptions,
                        const OCRepresentation& rep, const int eCode) >(
                                std::bind(&EnrolleeResource::onRequestPropertyDataResponse, this,
                                        std::placeholders::_1, std::placeholders::_2,
                                        std::placeholders::_3)));
            };

            OCStackResult result = requestPropertyDataStatus();

            if (result != OCStackResult::OC_STACK_OK)
            {
                PropertyData propertyData;
                std::shared_ptr< RequestPropertyDataStatus > requestPropertyDataStatus = std::make_shared<
                        RequestPropertyDataStatus >(ESResult::ES_ERROR, propertyData);
                m_RequestPropertyDataStatusCb(requestPropertyDataStatus);
                return;
            }
        }

        void EnrolleeResource::provisionEnrollee(const DataProvInfo& dataProvInfo)

        {
            if (m_ocResource == nullptr)
            {
                throw ESBadRequestException("Resource is not initialized");
            }

            m_dataProvInfo = dataProvInfo;

            OC::QueryParamsMap query;
            OC::OCRepresentation provisioningRepresentation;

            provisioningRepresentation.setValue(OC_RSRVD_ES_SSID, m_dataProvInfo.WIFI.ssid);
            provisioningRepresentation.setValue(OC_RSRVD_ES_CRED, m_dataProvInfo.WIFI.pwd);
            provisioningRepresentation.setValue(OC_RSRVD_ES_AUTHTYPE, m_dataProvInfo.WIFI.authtype);
            provisioningRepresentation.setValue(OC_RSRVD_ES_ENCTYPE, m_dataProvInfo.WIFI.enctype);
            provisioningRepresentation.setValue(OC_RSRVD_ES_LANGUAGE, m_dataProvInfo.Device.language);
            provisioningRepresentation.setValue(OC_RSRVD_ES_COUNTRY, m_dataProvInfo.Device.country);

            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getProvStatusResponse : ssid - %s",
                    (m_dataProvInfo.WIFI.ssid).c_str());
            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getProvStatusResponse : pwd - %s",
                    (m_dataProvInfo.WIFI.pwd).c_str());
            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getProvStatusResponse : authtype - %d",
                    m_dataProvInfo.WIFI.authtype);
            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getProvStatusResponse : enctype - %d",
                    m_dataProvInfo.WIFI.enctype);
            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getProvStatusResponse : language - %s",
                    (m_dataProvInfo.Device.language).c_str());
            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getProvStatusResponse : country - %s",
                    (m_dataProvInfo.Device.country).c_str());

            m_ocResource->post(OC_RSRVD_ES_RES_TYPE_PROV, BATCH_INTERFACE,
                    provisioningRepresentation, QueryParamsMap(),
                    std::function<
                            void(const HeaderOptions& headerOptions,
                                    const OCRepresentation& rep, const int eCode) >(
                    std::bind(&EnrolleeResource::checkProvInformationCb, this,
                    std::placeholders::_1, std::placeholders::_2,
                    std::placeholders::_3)));
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

        PropertyData EnrolleeResource::parsePropertyDataFromRepresentation(const OCRepresentation& rep)
        {
            OIC_LOG(DEBUG,ES_REMOTE_ENROLLEE_RES_TAG, "Enter parsePropertyDataFromRepresentation");

            DeviceConfig devInfo;
            NetworkInfo netInfo;
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
                        std::vector<int> types = prop->getValue<std::vector<int>>(OC_RSRVD_ES_SUPPORTEDWIFIMODE);

                        for(auto type = types.begin(); type != types.end(); ++type)
                        {
                            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "OC_RSRVD_ES_SUPPORTEDWIFIMODE = %d",
                                *type);
                            netInfo.types.push_back(static_cast<WIFI_MODE>(*type));
                        }

                        netInfo.freq = static_cast<WIFI_FREQ>(prop->getValue<int>(OC_RSRVD_ES_SUPPORTEDWIFIFREQ));


                        OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "OC_RSRVD_ES_SUPPORTEDWIFIFREQ = %d",
                                netInfo.freq);
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
                        devInfo.name = prop->getValue<std::string>(OC_RSRVD_ES_DEVNAME);
                        devInfo.language = prop->getValue<std::string>(OC_RSRVD_ES_LANGUAGE);
                        devInfo.country = prop->getValue<std::string>(OC_RSRVD_ES_COUNTRY);

                        OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "OC_RSRVD_ES_DEVNAME = %s",
                                devInfo.name.c_str());
                        OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "OC_RSRVD_ES_LANGUAGE = %s",
                                devInfo.language.c_str());
                        OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "OC_RSRVD_ES_COUNTRY = %s",
                                devInfo.country.c_str());
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

            return PropertyData(devInfo, netInfo, cloudable);
        }

    }
}
