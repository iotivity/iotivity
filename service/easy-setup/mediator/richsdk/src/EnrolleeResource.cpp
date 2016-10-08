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

        void EnrolleeResource::onProvisioningResponse(const HeaderOptions& /*headerOptions*/,
                const OCRepresentation& /*rep*/, const int eCode)
        {
            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "onProvisioningResponse : eCode = %d",
                        eCode);

            if (eCode > OCStackResult::OC_STACK_RESOURCE_CHANGED)
            {
                ESResult result = ESResult::ES_ERROR;

                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                            "onProvisioningResponse : Provisioning is failed ");

                if(eCode == OCStackResult::OC_STACK_COMM_ERROR)
                {
                    OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                        "can't receive any response from Enrollee by a timeout threshold.");
                    result = ESResult::ES_COMMUNICATION_ERROR;
                }

                std::shared_ptr< DevicePropProvisioningStatus > provStatus = std::make_shared<
                        DevicePropProvisioningStatus >(result);
                m_devicePropProvStatusCb(provStatus);
                return;
            }

            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                    "onProvisioningResponse : Provisioning is success. ");

            std::shared_ptr< DevicePropProvisioningStatus > provStatus = std::make_shared<
                    DevicePropProvisioningStatus >(ESResult::ES_OK);
            m_devicePropProvStatusCb(provStatus);
        }

        void EnrolleeResource::onGetStatusResponse(const HeaderOptions& /*headerOptions*/,
                const OCRepresentation& rep, const int eCode)
        {
            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "onGetStatusResponse : eCode = %d",
                        eCode);

            if (eCode > OCStackResult::OC_STACK_RESOURCE_CHANGED)
            {
                ESResult result = ESResult::ES_ERROR;

                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                            "onGetStatusResponse : onGetStatusResponse is failed ");

                if(eCode == OCStackResult::OC_STACK_COMM_ERROR)
                {
                    OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                        "can't receive any response from Enrollee by a timeout threshold.");
                    result = ESResult::ES_COMMUNICATION_ERROR;
                }

                EnrolleeStatus enrolleeStatus(rep);
                std::shared_ptr< GetEnrolleeStatus > getEnrolleeStatus = std::make_shared<
                        GetEnrolleeStatus >(result, enrolleeStatus);

                m_getStatusCb(getEnrolleeStatus);
            }
            else
            {
                EnrolleeStatus enrolleeStatus(rep);
                std::shared_ptr< GetEnrolleeStatus > getEnrolleeStatus = std::make_shared<
                        GetEnrolleeStatus >(ESResult::ES_OK, enrolleeStatus);

                m_getStatusCb(getEnrolleeStatus);
            }
        }

        void EnrolleeResource::onGetConfigurationResponse(const HeaderOptions& /*headerOptions*/,
                const OCRepresentation& rep, const int eCode)
        {
            OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "onGetConfigurationResponse : eCode = %d",
                        eCode);

            if (eCode > OCStackResult::OC_STACK_RESOURCE_CHANGED)
            {
                ESResult result = ESResult::ES_ERROR;

                OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                            "onGetConfigurationResponse : onGetConfigurationResponse is failed ");

                if(eCode == OCStackResult::OC_STACK_COMM_ERROR)
                {
                    OIC_LOG_V (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG,
                        "can't receive any response from Enrollee by a timeout threshold.");
                    result = ESResult::ES_COMMUNICATION_ERROR;
                }

                EnrolleeConf enrolleeConf(rep);
                std::shared_ptr< GetConfigurationStatus > getConfigurationStatus = std::make_shared<
                        GetConfigurationStatus >(result, enrolleeConf);
                m_getConfigurationStatusCb(getConfigurationStatus);
            }
            else
            {
                EnrolleeConf enrolleeConf(rep);

                std::shared_ptr< GetConfigurationStatus > getConfigurationStatus = std::make_shared<
                        GetConfigurationStatus >(ESResult::ES_OK, enrolleeConf);
                m_getConfigurationStatusCb(getConfigurationStatus);
            }
        }

        void EnrolleeResource::registerGetStatusCallback(
            const GetStatusCb callback)
        {
            m_getStatusCb = callback;
        }

        void EnrolleeResource::registerGetConfigurationStatusCallback(
            const GetConfigurationStatusCb callback)
        {
            m_getConfigurationStatusCb = callback;
        }

        void EnrolleeResource::registerDevicePropProvStatusCallback(
            const DevicePropProvStatusCb callback)
        {
            m_devicePropProvStatusCb = callback;
        }

        void EnrolleeResource::getStatus()
        {
            OIC_LOG (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getStatus IN");

            if (m_ocResource == nullptr)
            {
                throw ESBadRequestException("Resource is not initialized");
            }

            OC::QueryParamsMap query;
            OC::OCRepresentation rep;

            std::function< OCStackResult(void) > getStatus = [&]
            {
                return m_ocResource->get(m_ocResource->getResourceTypes().at(0),
                        DEFAULT_INTERFACE, query, std::function<void(const HeaderOptions& headerOptions,
                        const OCRepresentation& rep, const int eCode) >(
                                std::bind(&EnrolleeResource::onGetStatusResponse, this,
                                        std::placeholders::_1, std::placeholders::_2,
                                        std::placeholders::_3)), OC::QualityOfService::HighQos);
            };

            OCStackResult result = getStatus();

            if (result != OCStackResult::OC_STACK_OK)
            {
                EnrolleeStatus enrolleeStatus(rep);// = {ES_STATE_INIT, ES_ERRCODE_NO_ERROR};
                std::shared_ptr< GetEnrolleeStatus > getEnrolleeStatus = std::make_shared<
                        GetEnrolleeStatus >(ESResult::ES_ERROR, enrolleeStatus);

                m_getStatusCb(getEnrolleeStatus);

                return;
            }
            OIC_LOG (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getStatus OUT");
        }

        void EnrolleeResource::getConfiguration()
        {
            OIC_LOG (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getConfiguration IN");

            if (m_ocResource == nullptr)
            {
                throw ESBadRequestException("Resource is not initialized");
            }

            OC::QueryParamsMap query;
            OC::OCRepresentation rep;

            std::function< OCStackResult(void) > getConfigurationStatus = [&]
            {
                return m_ocResource->get(m_ocResource->getResourceTypes().at(0),
                        BATCH_INTERFACE, query, std::function<void(const HeaderOptions& headerOptions,
                        const OCRepresentation& rep, const int eCode) >(
                                std::bind(&EnrolleeResource::onGetConfigurationResponse, this,
                                        std::placeholders::_1, std::placeholders::_2,
                                        std::placeholders::_3)), OC::QualityOfService::HighQos);
            };

            OCStackResult result = getConfigurationStatus();

            if (result != OCStackResult::OC_STACK_OK)
            {
                EnrolleeConf enrolleeConf(rep);
                std::shared_ptr< GetConfigurationStatus > getConfigurationStatus = std::make_shared<
                        GetConfigurationStatus >(ESResult::ES_ERROR, enrolleeConf);
                m_getConfigurationStatusCb(getConfigurationStatus);
                return;
            }

            OIC_LOG (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "getConfiguration OUT");
        }

        void EnrolleeResource::provisionProperties(const DeviceProp& deviceProp)
        {
            OIC_LOG (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "provisionProperties IN");
            if (m_ocResource == nullptr)
            {
                throw ESBadRequestException("Resource is not initialized");
            }

            OC::QueryParamsMap query;
            OC::OCRepresentation provisioningRepresentation = deviceProp.toOCRepresentation();

            m_ocResource->post(OC_RSRVD_ES_RES_TYPE_PROV, BATCH_INTERFACE,
                    provisioningRepresentation, QueryParamsMap(),
                    std::function<
                            void(const HeaderOptions& headerOptions,
                                    const OCRepresentation& rep, const int eCode) >(
                    std::bind(&EnrolleeResource::onProvisioningResponse, this,
                    std::placeholders::_1, std::placeholders::_2,
                    std::placeholders::_3)), OC::QualityOfService::HighQos);

            OIC_LOG (DEBUG, ES_REMOTE_ENROLLEE_RES_TAG, "provisionProperties OUT");
        }
    }
}
