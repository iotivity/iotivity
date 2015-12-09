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

/**
 * This file contains the exported symbol.
 */

#include "FireAlarmSensor.h"

#include <iostream>
#include "SysTimer.h"

#ifdef __ANDROID__
#include "OCAndroid.h"
#endif

using namespace FireAlarmSensorName;

FireAlarmSensor::FireAlarmSensor()
{
    m_humidity = "";
    m_temperature = "";
    m_light = "";
    m_fireAlarmLevel = "";
}

FireAlarmSensor::~FireAlarmSensor()
{

}

int FireAlarmSensor::executeFASensorLogic(std::map<std::string, std::string> *pInputData,
        std::string *pOutput)
{
    std::cout << "[FireAlarmSensor] FireAlarmSensor::" << __func__ << " is called."
              << std::endl;

    FAResult result;

    m_temperature = pInputData->at("temperature");
    m_humidity = pInputData->at("humidity");
    m_light = pInputData->at("light");

    if ((result = makeFireAlarm()) != SUCCESS)
    {
        std::cout << "Error : makeFireAlarm() result = " << result << std::endl;
        return -1;
    }

    (*pOutput) = m_fireAlarmLevel;

    return 0;
}

/**
 * Calculation of FireAlarm with TEMP&HUMI.
 */
FAResult FireAlarmSensor::makeFireAlarm()
{
    int FALevel = 0;


    double temperature = std::stod(m_temperature);
    double humidity = std::stod(m_humidity);
    double light = std::stod(m_light);


    if (temperature < FIRE_BASE_TEMPERATURE) {
        FALevel = 0;
    } else if (temperature >= FIRE_BASE_TEMPERATURE) {
        FALevel += 1;
    } else if (temperature > FIRE_BASE_TEMPERATURE + FIRE_TEMPERATURE_LEVEL_DELTA) {
        FALevel += 2;
    } else if (temperature > FIRE_BASE_TEMPERATURE + 2 * FIRE_TEMPERATURE_LEVEL_DELTA) {
        FALevel += 3;
    } 

    if (humidity < 10) {
        FALevel += 1;
    }

    // TODO: Implement proper light sensing mechanism for fire detection
    /*
    if (light < ?) {
      FALevel += 1;
    }
    */

    if (FALevel > 3) {
        FALevel = 3;
    }


    m_fireAlarmLevel = std::to_string(FALevel);

    return SUCCESS;
}
