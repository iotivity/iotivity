//******************************************************************
//
// Copyright 2015 Hyunwoo Lee (Seoul National University) All Rights Reserved.
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

#include <functional>
#include <pthread.h>
#include "OCPlatform.h"
#include "OCApi.h"

#include <cstdlib>

/**
 * ACNE
 */

namespace ACNESensorName
{
    typedef enum
    {
        SUCCESS = 0, ERROR
    } ACNEResult;

    class ACNESensor
    {
        public:
            ACNESensor();
            ~ACNESensor();

            int executeACNESensorLogic(std::map<std::string, std::string> *pInputData, std::string *pOutput);

            ACNEResult makeACNE();

            // to get outside resource:djkim
           std::string m_resourceUri;
           std::vector<std::string> m_resourceTypes;
           std::vector<std::string> m_resourceInterfaces;
           OC::OCRepresentation m_resourceRep;
           OC::ObservationIds m_interestedObservers;

        public:
            std::string m_temp;      // the temperature of the skin.
            std::string m_moist;     // the moisture of the skin.
            std::string m_oil;       // the oil of the skin.
            std::string m_outTemp;   // the temperature of the outside.
            std::string m_outHumid;  // the humidity of the outside.

    };
};

#endif /* ACNESENSOR_H_ */
