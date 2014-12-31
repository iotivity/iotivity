/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
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

/**
 * This file contains the exported symbol.
 */
#include <stdlib.h>
#include <map>
#include <string>
#include <sstream>
#include <iostream>

#include "DiscomfortIndexSensor.h"
#include "SysTimer.h"

#ifdef __ANDROID__
#include "OCAndroid.h"
#endif

using namespace DiscomfortIndexSensorName;

#define SENSOR_NAME "DiscomfortIndexSensor"

char *inputName[2] =
{ (char *)"temperature", (char *)"humidity" };

physicalInput DiscomfortIndexSensor::s_PHYSICAL_SOFTSENSORs[PHYSICAL_EA] =
{
    { (char *)"Thing_TempHumSensor", 2, (void *) &inputName },
    { (char *)"Thing_TempHumSensor1", 2, (void *) &inputName }
};

ICtxDelegate *g_pDelegate;

void InitializeContext(ICtxDelegate *pDelegate)
{
    std::vector < ContextData > contextData;

    DiscomfortIndexSensor *eventCls = new DiscomfortIndexSensor();
    pDelegate->registerCallback(eventCls);
    g_pDelegate = pDelegate;

    std::cout << "DiscomfortIndexSensor loaded" << std::endl;

    return;
}

DiscomfortIndexSensor::DiscomfortIndexSensor()
{
    m_result.m_timestamp = "";
    m_result.m_humidity = "";
    m_result.m_temperature = "";
    m_result.m_discomfortIndex = "";
}

void DiscomfortIndexSensor::onCtxEvent(enum CTX_EVENT_TYPE eventType,
                                       std::vector< ContextData > contextDataList)
{
    switch (eventType)
    {
        case SPF_START:
            runLogic(contextDataList);
            break;
        default:
            std::cout << "Not support onCtxEvent" << std::endl;
            break;
    }
}

int DiscomfortIndexSensor::runLogic(std::vector< ContextData > &contextDataList)
{
    std::cout << "[DiscomfortIndexSensor] DiscomfortIndexSensor::" << __func__ << " is called."
              << std::endl;

    DIResult result;

    if (getInput(contextDataList, m_DI) == SUCCESS)
    {
        if ((result = makeDiscomfortIndex(m_DI)) != SUCCESS)
        {
            std::cout << "Error : makeDiscomfortIndex() result = " << result << std::endl;
            return -1;
        }

        std::vector < ContextData > outList;
        ContextData out = setOutput(4, m_DI);

        outList.push_back(out);
        g_pDelegate->addOutput(outList);

        return 0;
    }

    return -1;
}

/**
 * Get Input data (temperature, humidity) using resource Client of Iotivity base.
 */
DIResult DiscomfortIndexSensor::getInput(std::vector< ContextData > &contextDataList, InValue *data)
{
    int result_flag = 0;
    int contextSize = 0;

    if ((contextSize = contextDataList.size()) == 0)
    {
        std::cout << "Physical Context data is not exist." << std::endl;
        return ERROR;
    }

    for (int i = 0; i < contextSize; i++)
    {
        for (int k = 0; k < PHYSICAL_EA; k++)
        {
            if (contextDataList[i].rootName == s_PHYSICAL_SOFTSENSORs[k].m_thingName)
            {
                std::vector < std::map< std::string, std::string > > lVector =
                    contextDataList[i].outputProperty;
                int requiredInputNum = s_PHYSICAL_SOFTSENSORs[k].m_inputNum;
                char **pchar = (char **) (s_PHYSICAL_SOFTSENSORs[k].m_pInputStruct);
                if (requiredInputNum == 0)
                {
                    std::cout << "No input List." << std::endl;
                    return ERROR;
                }

                for (unsigned int j = 0; j < lVector.size(); j++)
                {
                    std::string name = lVector[j]["name"];

                    if (name.compare(*pchar) == 0)
                    {
                        data->m_temperature = lVector[j]["value"];
                        requiredInputNum--;
                    }
                    else if (name.compare(*(++pchar)) == 0)
                    {
                        data->m_humidity = lVector[j]["value"];
                        requiredInputNum--;
                    }
                }

                if (requiredInputNum == 0)
                {
                    data++;
                    result_flag++;
                }
                break;
            } // if
        } // for
    }

    if (result_flag == PHYSICAL_EA)
    {
        std::cout << "Success : getInput()" << std::endl;
        return SUCCESS;
    }

    return ERROR;
}

/**
 * Calculation of DiscomfortIndex with TEMP&HUMI of InValue.
 */
DIResult DiscomfortIndexSensor::makeDiscomfortIndex(InValue *data)
{
    int discomfortIndex = (int) ERROR;
    double sumDI = 0.0;

    m_result.m_temperature = "";
    m_result.m_humidity = "";

    for (int i = 0; i < PHYSICAL_EA; i++)
    {
        if (i != 0)
        {
            m_result.m_temperature += ", ";
            m_result.m_humidity += ", ";
        }

        double dI = 0.0;
        int t = std::stoi((data + i)->m_temperature);
        int h = std::stoi((data + i)->m_humidity);
        double F = (9.0 * (double) t) / 5.0 + 32.0;

        std::cout << "Device Number : " << i << std::endl;

        dI = F - (F - 58.0) * (double) ((100 - h) * 55) / 10000.0;

        std::cout << "Discomfort level : " << dI << ", Temperature :" << t << ", Humidity :" << h
                  << std::endl;

        (data + i)->m_discomfortIndex = std::to_string(0);
        m_result.m_temperature += std::to_string(t) + ", ";
        m_result.m_humidity += std::to_string(h) + ", ";
        sumDI += dI;
    }

    sumDI = sumDI / PHYSICAL_EA;
    std::cout << "[result] Avg. DI level : " << sumDI << std::endl;
    if (sumDI >= 80.0)
    {
        discomfortIndex = (int) ALL_DISCOMPORT;
        std::cout << "DI : " << discomfortIndex << " : All person discomfort. : " << sumDI
                  << std::endl;
    }
    else if (sumDI >= 75.0)
    {
        discomfortIndex = (int) HALF_DISCOMPORT;
        std::cout << "DI : " << discomfortIndex << " : Half of person discomfort. : " << sumDI
                  << std::endl;
    }
    else if (sumDI >= 68.0)
    {
        discomfortIndex = (int) LITTLE_DISCOMPORT;
        std::cout << "DI : " << discomfortIndex << " : A little person discomfort. : " << sumDI
                  << std::endl;
    }
    else
    {
        discomfortIndex = (int) ALL_COMPORT;
        std::cout << "DI : " << discomfortIndex << " : All person comfort. : " << sumDI
                  << std::endl;
    }

    m_result.m_discomfortIndex = std::to_string(discomfortIndex);
    std::cout << "[result] Discomfort Index : " << m_result.m_discomfortIndex << std::endl;

    return SUCCESS;
}

ContextData DiscomfortIndexSensor::setOutput(int property_count, InValue *data)
{
    ContextData out;

    std::map < std::string, std::string > output_property;

    out.rootName = SENSOR_NAME;
    out.outputPropertyCount = property_count;

    output_property.insert(std::make_pair("name", "timestamp"));
    output_property.insert(std::make_pair("type", "string"));
    output_property.insert(std::make_pair("value", m_result.m_timestamp));

    out.outputProperty.push_back(output_property);

    output_property.clear();
    output_property.insert(std::make_pair("name", "temperature"));
    output_property.insert(std::make_pair("type", "string"));
    output_property.insert(std::make_pair("value", m_result.m_temperature));

    out.outputProperty.push_back(output_property);

    output_property.clear();
    output_property.insert(std::make_pair("name", "humidity"));
    output_property.insert(std::make_pair("type", "string"));
    output_property.insert(std::make_pair("value", m_result.m_humidity));

    out.outputProperty.push_back(output_property);

    output_property.clear();
    output_property.insert(std::make_pair("name", "discomfortIndex"));
    output_property.insert(std::make_pair("type", "int"));
    output_property.insert(std::make_pair("value", m_result.m_discomfortIndex));

    out.outputProperty.push_back(output_property);

    return out;
}

