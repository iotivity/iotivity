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

#ifndef ACNESENSOR_H_
#define ACNESENSOR_H_

#include <string>
#include <map>
#include <time.h>
#include <cstdlib>

#include <functional>
#include <pthread.h>
#include "OCPlatform.h"
#include "OCApi.h"

#define COAP_IP                 "147.46.215.152"
#define COAP_PORT               4321
#define COAP_MODE               OC::ModeType::Server
#define COAP_SRVTYPE            OC::ServiceType::InProc

#define RESOURCE_TYPE_NAME_TEMP          "oic.r.temperature"
#define RESOURCE_TYPE_NAME_HUMID          "oic.r.humidity"

#define INVALID  0
#define TOOHOT   1
#define VERYHOT  2
#define HOT      3
#define WARM     4
#define COOL     5
#define COLD     6
#define VERYCOLD 7
#define TOOCOLD  8

#define DIFFTIME      5     // valid time difference. (seconds)


namespace AcneSensorName
{
    typedef enum
    {
        SUCCESS = 0, ERROR
    } AcneResult;

    class AcneSensor
    {
        public:
            AcneSensor();
            ~AcneSensor();

            int executeAcneSensorLogic(std::map<std::string, std::string> *pInputData,
                                      std::string *pOutput);
            AcneResult makeAcne();

            std::string m_temperature;
            std::string m_humidity;
            std::string m_AcneResult;
            
            // int m_humid;
            // int m_temp;

            std::string m_resourceUri;
            std::vector<std::string> m_resourceTypes;
            std::vector<std::string> m_resourceInterfaces;
            OCResourceHandle m_resourceHandle;
            OC::OCRepresentation m_resourceRep;
            OC::ObservationIds m_interestedObservers;

            time_t  m_timepstampT;
            time_t  m_timepstampH;
            void registerResource();

            OCResourceHandle getHandle();

            void setResourceRepresentation(OC::OCRepresentation &rep);

            OC::OCRepresentation getResourceRepresentation();
    };
};

#endif /* ACNESENSOR_H_ */
