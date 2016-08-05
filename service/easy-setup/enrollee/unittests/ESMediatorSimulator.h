//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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

#ifndef _ES_MEDIATOR_SIMULATOR_H_
#define _ES_MEDIATOR_SIMULATOR_H_

#include <iostream>

#include "OCPlatform.h"
#include "OCApi.h"
#include "oic_malloc.h"

#include "EasySetup.h"
#include "ESRichCommon.h"

#define PROV_RESOURCE_TYPE "ocf.wk.prov"

using namespace OIC::Service;

class ESMediatorSimulator
{
private:
    std::function<void(std::shared_ptr<OC::OCResource> resource)> m_discoveryCb;
    std::function<void(std::shared_ptr< GetConfigurationStatus > status)> m_getConfigurationCb;
    std::function<void(std::shared_ptr< GetEnrolleeStatus >)> m_getStatusCb;
    std::function<void(std::shared_ptr< DevicePropProvisioningStatus >)> m_DevicePropProvisioningCb;
    std::function<void(std::shared_ptr< CloudPropProvisioningStatus >)> m_CloudPropProvisioningCb;

    std::shared_ptr<RemoteEnrollee> m_remoteEnrollee;

public:
    ESMediatorSimulator()
    : m_remoteEnrollee(), m_discoveryCb(), m_getConfigurationCb(), m_getStatusCb(),
    m_DevicePropProvisioningCb(), m_CloudPropProvisioningCb()
    {
    };
    ~ESMediatorSimulator() = default;

    ESMediatorSimulator(const ESMediatorSimulator &) = delete;
    ESMediatorSimulator & operator = (const ESMediatorSimulator &) = delete;

    ESMediatorSimulator(ESMediatorSimulator &&) = delete;
    ESMediatorSimulator & operator = (ESMediatorSimulator &&) = delete;

    void discoverRemoteEnrollee(std::function<void(std::shared_ptr<OC::OCResource> resource)> cb)
    {
        m_discoveryCb = cb;
        std::string uri = std::string("/oic/res?rt=") + PROV_RESOURCE_TYPE;
        OC::OCPlatform::findResource("", uri,
                OCConnectivityType::CT_DEFAULT,
                std::bind(&ESMediatorSimulator::discoverRemoteEnrolleeCb,
                                                                    this, std::placeholders::_1));

    }

    void getConfiguration(std::function<void(std::shared_ptr< GetConfigurationStatus >)> cb)
    {
        m_getConfigurationCb = cb;
        m_remoteEnrollee = NULL;
        std::string uri = std::string("/oic/res?rt=") + PROV_RESOURCE_TYPE;
        OC::OCPlatform::findResource("", uri,
                OCConnectivityType::CT_DEFAULT,
                std::bind(&ESMediatorSimulator::discoverRemoteEnrolleeCbToGetConfiguration,
                                                                    this, std::placeholders::_1));
    }

    void getStatus(std::function<void(std::shared_ptr< GetEnrolleeStatus >)> cb)
    {
        m_getStatusCb = cb;
        m_remoteEnrollee = NULL;
        std::string uri = std::string("/oic/res?rt=") + PROV_RESOURCE_TYPE;
        OC::OCPlatform::findResource("", uri,
                OCConnectivityType::CT_DEFAULT,
                std::bind(&ESMediatorSimulator::discoverRemoteEnrolleeCbToGetStatus,
                                                                    this, std::placeholders::_1));
    }

    void provisionDeviceProperties(std::function<void(std::shared_ptr< DevicePropProvisioningStatus >)> cb)
    {
        m_DevicePropProvisioningCb = cb;
        m_remoteEnrollee = NULL;
        std::string uri = std::string("/oic/res?rt=") + PROV_RESOURCE_TYPE;
        OC::OCPlatform::findResource("", uri,
                OCConnectivityType::CT_DEFAULT,
                std::bind(&ESMediatorSimulator::discoverRemoteEnrolleeCbToProvisionDeviceProperties,
                                                                    this, std::placeholders::_1));
    }

    void provisionCloudProperties(std::function<void(std::shared_ptr< CloudPropProvisioningStatus >)> cb)
    {
        m_CloudPropProvisioningCb = cb;
        m_remoteEnrollee = NULL;
        std::string uri = std::string("/oic/res?rt=") + PROV_RESOURCE_TYPE;
        OC::OCPlatform::findResource("", uri,
                OCConnectivityType::CT_DEFAULT,
                std::bind(&ESMediatorSimulator::discoverRemoteEnrolleeCbToProvisionCloudProperties,
                                                                    this, std::placeholders::_1));
    }

private:

    void discoverRemoteEnrolleeCb(std::shared_ptr<OC::OCResource> resource)
    {
        if(!resource->getResourceTypes().at(0).compare(PROV_RESOURCE_TYPE) && m_discoveryCb)
        {
            m_discoveryCb(resource);
            m_discoveryCb = NULL;
        }
    }

    void getConfigurationCallback(std::shared_ptr< GetConfigurationStatus > getConfigurationStatus)
    {
        if(m_getConfigurationCb != NULL)
        {
            m_getConfigurationCb(getConfigurationStatus);
            m_getConfigurationCb = NULL;
        }
    }

    void discoverRemoteEnrolleeCbToGetConfiguration(std::shared_ptr<OC::OCResource> resource)
    {
        if(!resource->getResourceTypes().at(0).compare(PROV_RESOURCE_TYPE) && m_getConfigurationCb
                                                                           && !m_remoteEnrollee)
        {
            m_remoteEnrollee = EasySetup::getInstance()->createRemoteEnrollee(resource);

            if(m_remoteEnrollee != NULL)
            {
              m_remoteEnrollee->getConfiguration(std::bind(
                    &ESMediatorSimulator::getConfigurationCallback, this, std::placeholders::_1));
            }
        }
    }

    void getStatusCallback(std::shared_ptr< GetEnrolleeStatus > getEnrolleeStatus)
    {
        if(m_getStatusCb != NULL)
        {
            m_getStatusCb(getEnrolleeStatus);
            m_getStatusCb = NULL;
        }
    }

    void discoverRemoteEnrolleeCbToGetStatus(std::shared_ptr<OC::OCResource> resource)
    {
        if(!resource->getResourceTypes().at(0).compare(PROV_RESOURCE_TYPE) && m_getStatusCb
                                                                            && !m_remoteEnrollee)
        {
            m_remoteEnrollee = EasySetup::getInstance()->createRemoteEnrollee(resource);

            if(m_remoteEnrollee != NULL)
            {
                m_remoteEnrollee->getStatus(std::bind(
                    &ESMediatorSimulator::getStatusCallback, this, std::placeholders::_1));
            }
        }
    }

    void deviceProvisioningStatusCallback(std::shared_ptr< DevicePropProvisioningStatus >
                                                                    devicePropProvisioningStatus)
    {
        if(m_DevicePropProvisioningCb != NULL)
        {
            m_DevicePropProvisioningCb(devicePropProvisioningStatus);
            m_DevicePropProvisioningCb = NULL;
        }
    }

    void discoverRemoteEnrolleeCbToProvisionDeviceProperties(std::shared_ptr<OC::OCResource> resource)
    {
        if(!resource->getResourceTypes().at(0).compare(PROV_RESOURCE_TYPE) &&
                                                m_DevicePropProvisioningCb && !m_remoteEnrollee)
        {
            m_remoteEnrollee = EasySetup::getInstance()->createRemoteEnrollee(resource);

            if(m_remoteEnrollee != NULL)
            {
                DeviceProp devProp;
                devProp.setWiFiProp("Iotivity_SSID", "Iotivity_PWD", WPA2_PSK, TKIP_AES);
                devProp.setDevConfProp("korean", "Korea");

                m_remoteEnrollee->provisionDeviceProperties(devProp,
                    std::bind(&ESMediatorSimulator::deviceProvisioningStatusCallback,
                                                            this, std::placeholders::_1));
            }
        }
    }

    void cloudProvisioningStatusCallback(std::shared_ptr< CloudPropProvisioningStatus >
                                                                    cloudPropProvisioningStatus)
    {
        if(m_CloudPropProvisioningCb != NULL)
        {
            m_CloudPropProvisioningCb(cloudPropProvisioningStatus);
            if(cloudPropProvisioningStatus->getESCloudState() == ES_CLOUD_PROVISIONING_SUCCESS)
            {
                m_CloudPropProvisioningCb = NULL;
            }
        }
    }

    void discoverRemoteEnrolleeCbToProvisionCloudProperties(std::shared_ptr<OC::OCResource> resource)
    {
        if(!resource->getResourceTypes().at(0).compare(PROV_RESOURCE_TYPE) &&
                                                m_CloudPropProvisioningCb && !m_remoteEnrollee)
        {
            m_remoteEnrollee = EasySetup::getInstance()->createRemoteEnrollee(resource);

            if(m_remoteEnrollee != NULL)
            {
                CloudProp cloudProp;
                cloudProp.setCloudProp("authCode", "authProvider", "ciServer");

                m_remoteEnrollee->provisionCloudProperties(cloudProp,
                    std::bind(&ESMediatorSimulator::cloudProvisioningStatusCallback,
                                                            this, std::placeholders::_1));
            }
        }
    }
};


#endif //_NS_CONSUMER_SIMULATOR_H_

