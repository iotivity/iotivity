//******************************************************************
//
// Copyright 2015 Jaemin Jo (Seoul National University) All Rights Reserved.
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

#ifndef ENERGYSAVER_H_
#define ENERGYSAVER_H_

#include <string>
#include <map>
#include <time.h>

/**
 * Energy 
 *
 * light > 80 and peopleCount == 0 : WASTING_ENERGY
 * light > 50 and peopleCount == 0 : ATTENTION_REQUIRED
 * otherwise : NORMAL
 */

#define OUTOFDATE     0
#define NORMAL    1
#define ATTENTION_REQUIRED    2
#define WASTING_ENERGY   3

#define POSSIBLY_TURNED_ON_LIGHT_VAL     50
#define DEFINITELY_TURNED_ON_LIGHT_VAL      80

#define DIFFTIME      15     // valid time difference. (seconds)

namespace EnergySaverName
{
    typedef enum
    {
        SUCCESS = 0, ERROR
    } EnergySaverResult;

    class EnergySaver
    {
        public:
            EnergySaver();
            ~EnergySaver();

            int executeEnergySaverLogic(std::map<std::string, std::string> *pInputData,
                                      std::string *pOutput);
            EnergySaverResult calculateEnergyWaste(void);

        private:
            std::string m_light;
            std::string m_peopleCount;
            std::string m_energySaverResult;

            time_t  m_timestampL;
            time_t  m_timestampP;
    };
};

#endif /* ENERGYSAVER_H_ */
