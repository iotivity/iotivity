//******************************************************************
//
// Copyright 2015 Young Eun Cho, Hwan Suk Choi (Seoul National University) All Rights Reserved.
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

#ifndef SYNCHLOCALIZATIONSENSOR_H_
#define SYNCHLOCALIZATIONSENSOR_H_

#include <string>
#include <map>
#include <time.h>

namespace SynchLocalizationSensorName
{
    typedef enum
    {
        SUCCESS = 0, ERROR
    } SynchLocalizationResult;

    class SynchLocalizationSensor
    {
        public:
            SynchLocalizationSensor();
            ~SynchLocalizationSensor();

            int executeSynchLocalizationSensorLogic(std::map<std::string, std::string> *pInputData,
                                      std::string *pOutput);
            SynchLocalizationResult calculateLocation(void);

        private:
            std::string m_dist_from_node1;
            std::string m_dist_from_node2;
            std::string m_dist_from_node3;

            time_t  m_timepstampRF_node1;
            time_t  m_timepstampUS_node1;
            time_t  m_timepstampRF_node2;
            time_t  m_timepstampRF_node2;
            time_t  m_timepstampUS_node3;
            time_t  m_timepstampUS_node3;
    };
};

#endif /* SYNCHLOCALIZATIONSENSOR_BUNDLE_H_ */
