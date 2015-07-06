/******************************************************************
*
* Copyright 2015 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

/*
* DiscomfortIndexSensor.h
*/

#ifndef DISCOMFORTINDEXSENSOR_H_
#define DISCOMFORTINDEXSENSOR_H_

/**
* This header file is included to define _EXPORT_.
*/

#include <iostream>

#include "SoftSensorResource.h"

using namespace OIC::Service;

namespace DiscomfortIndexSensorName
{
#define PHYSICAL_EA 2

    typedef struct _physicalInput_
    {
        char *m_thingName;
        int m_inputNum;
        void *m_pInputStruct;
    } physicalInput;

    typedef enum
    {
        SUCCESS = 0, ERROR, ALL_DISCOMPORT, HALF_DISCOMPORT, LITTLE_DISCOMPORT, ALL_COMPORT
    } DIResult;

    class DiscomfortIndexSensor
    {
        private:

            physicalInput s_PHYSICAL_SOFTSENSORs[PHYSICAL_EA];

            class InValue
            {
                public:
                    std::string m_timestamp; // .
                    std::string m_discomfortIndex; // Discomfort Index. ( 2 ~ 5 )
                    std::string m_humidity; // relative humidity.
                    std::string m_temperature; // celsius temperature.
            };

            InValue m_DI[PHYSICAL_EA];
            InValue m_result;

        public:
            DiscomfortIndexSensor(vector <string> inputs);

            int runLogic(std::vector< SoftSensorResource::SensorData > &sensorData);
            DIResult getInput(std::vector< SoftSensorResource::SensorData > &contextDataList,
                              InValue *data);
            DIResult makeDiscomfortIndex(InValue *data);
            SoftSensorResource::SensorData setOutput(int property_count, InValue *data);

            SoftSensorResource::SensorData m_output;
    };
};

#endif /* DISCOMFORTINDEXSENSOR_H_ */
