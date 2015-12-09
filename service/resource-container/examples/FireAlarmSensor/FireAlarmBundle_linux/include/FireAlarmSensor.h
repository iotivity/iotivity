//******************************************************************
//
// Copyright 2015 Junhyeok Choi (Seoul National University) All Rights Reserved.
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

#ifndef FireAlarmSENSOR_H_
#define FireAlarmSENSOR_H_

#include <map>
#include <string>

#define FIRE_BASE_TEMPERATURE 60
#define FIRE_TEMPERATURE_LEVEL_DELTA 100

namespace FireAlarmSensorName
{

    typedef enum
    {
        SUCCESS=0, ERROR,
    } FAResult;

    class FireAlarmSensor
    {
        public:
            FireAlarmSensor();
            ~FireAlarmSensor();

            int executeFASensorLogic(std::map<std::string, std::string> *pInputData,
                                     std::string *pOutput);
            FAResult makeFireAlarm();

        private:
            std::string m_humidity;
            std::string m_temperature;
            std::string m_light;
            std::string m_fireAlarmLevel;
    };
};

#endif /* FireAlarmSENSOR_H_ */
