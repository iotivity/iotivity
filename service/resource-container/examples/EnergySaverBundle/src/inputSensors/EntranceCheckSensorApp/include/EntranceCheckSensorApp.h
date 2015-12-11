//******************************************************************
//
// Copyright 2015 Sehi L'Yi (Seoul National University) All Rights Reserved.
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

#ifndef ENTRANCECHECKSENSORAPP_H_
#define ENTRANCECHECKSENSORAPP_H_

#include <functional>
#include <pthread.h>
#include "OCPlatform.h"
#include "OCApi.h"

#include <string>
#include <cstdlib>


#define COAP_IP                 "0.0.0.0"
#define COAP_PORT               0
#define COAP_MODE               OC::ModeType::Server
#define COAP_SRVTYPE            OC::ServiceType::InProc

#define RESOURCE_TYPE_NAME          "oic.r.sensor.entrancecheck"

//  testing case
#define INTERVAL_FOR_CHECK          5   // seconds
#define INTERVAL_FOR_MEASUREMENT    3   // seconds
#define INIT_VAL                    0   // initial # of people
#define DIFF_VAL                    1   // # of people increased for each measurement
#define MAX_VAL                     2   // max # of people

class EntranceCheckResource
{
    public:
        // Access this property from a TB client
        int m_peopleCount;
        std::string m_resourceUri;
        std::vector<std::string> m_resourceTypes;
        std::vector<std::string> m_resourceInterfaces;
        OCResourceHandle m_resourceHandle;
        OC::OCRepresentation m_resourceRep;
        OC::ObservationIds m_interestedObservers;

    public:
        // Constructor
        EntranceCheckResource() : m_peopleCount(INIT_VAL), m_resourceHandle(0)
        {
            m_resourceUri = "/Thing_EntranceCheckSensor";
            m_resourceTypes.push_back(RESOURCE_TYPE_NAME);
            m_resourceInterfaces.push_back(OC::DEFAULT_INTERFACE);

            printf("Running thing as %s\n", m_resourceUri.c_str());
            m_resourceRep.setUri(m_resourceUri);
            m_resourceRep.setResourceTypes(m_resourceTypes);
            m_resourceRep.setResourceInterfaces(m_resourceInterfaces);
        }

        ~EntranceCheckResource()
        {
        }

        void registerResource();

        OCResourceHandle getHandle();

        void setResourceRepresentation(OC::OCRepresentation &rep);

        OC::OCRepresentation getResourceRepresentation();
};

#endif /* ENTRANCECHECKSENSORAPP_H_ */
