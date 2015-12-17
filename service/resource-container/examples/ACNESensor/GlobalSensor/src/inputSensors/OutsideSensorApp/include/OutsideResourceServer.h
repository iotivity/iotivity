//******************************************************************
//
// Copyright 2015 Duck Ju Kim (Seoul National University) All Rights Reserved.
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

#ifndef OUTSIDERESOURCESERVER_H_
#define OUTSIDERESOURCESERVER_H_

#include <functional>
#include <pthread.h>
#include "OCPlatform.h"
#include "OCApi.h"

#include <string>
#include <cstdlib>

#define COAP_IP                 "147.46.215.152"
#define COAP_PORT               4322
#define COAP_MODE               OC::ModeType::Server
#define COAP_SRVTYPE            OC::ServiceType::InProc

#define RESOURCE_TYPE_NAME_TEMP          "oic.r.sensor.temperature"
#define RESOURCE_TYPE_NAME_HUMID         "oic.r.sensor.humidity"

// Forward declaring the entityHandler

class TemphumidResource
{
    public:
        /// Access this property from a TB client
        int m_humid;
        int m_temp;
        // std::string m_name
        std::string m_resourceUri;
        std::vector<std::string> m_resourceTypes;
        std::vector<std::string> m_resourceInterfaces;
        OCResourceHandle m_resourceHandle;
        OC::OCRepresentation m_resourceRep;
        OC::ObservationIds m_interestedObservers;

    public:
        TemphumidResource() :
            m_humid(60), m_temp(27), m_resourceHandle(0)
        {
            m_resourceUri = "/OutTempHumSensor";
            m_resourceTypes.push_back(RESOURCE_TYPE_NAME_TEMP);
            m_resourceTypes.push_back(RESOURCE_TYPE_NAME_HUMID);
            m_resourceInterfaces.push_back(OC::DEFAULT_INTERFACE);
            
//            m_resourceRep.setValue(RESOURCE_TYPE_NAME_TEMP, m_temp);
//            m_resourceRep.setValue(RESOURCE_TYPE_NAME_HUMID, m_humid);
            // m_lightRep.setValue("name", m_name);
            printf("temperature: %i\n", m_temp);
            printf("humidity: %i\n", m_humid);

            printf("Running thing as %s\n", m_resourceUri.c_str());
            m_resourceRep.setUri(m_resourceUri);
            m_resourceRep.setResourceTypes(m_resourceTypes);
            m_resourceRep.setResourceInterfaces(m_resourceInterfaces);
        }

        ~TemphumidResource()
        {
        }

        void registerResource();

        OCResourceHandle getHandle();

        void setResourceRepresentation(OC::OCRepresentation &rep);

        OC::OCRepresentation getResourceRepresentation();
};

#endif /* OUTSIDERESOURCESERVER_H_ */
