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

/**
 * This file contains the exported symbol.
 */
#include "EnergySaver.h"

#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include "SysTimer.h"

using namespace EnergySaverName;

EnergySaver::EnergySaver()
{
    m_light = "";
    m_peopleCount = "";
    m_energySaverResult = "";
    time(&m_timestampL);
    time(&m_timestampP);
}

EnergySaver::~EnergySaver()
{

}

int EnergySaver::executeEnergySaverLogic(std::map<std::string, std::string> *pInputData,
                                     std::string *pOutput)
{
    EnergySaverResult result;

    if (pInputData->find("light") != pInputData->end())
    {
        m_light = pInputData->at("light");
        time(&m_timestampL);
    }

    if (pInputData->find("peopleCount") != pInputData->end())
    {
        m_peopleCount = pInputData->at("peopleCount");
        time(&m_timestampP);
    }

    if ((result = calculateEnergyWaste()) != SUCCESS)
    {
        return -1;
    }

    (*pOutput) = m_energySaverResult;

    return 0;
}

/**
 * Calculation of Energy Waste
 */
EnergySaverResult EnergySaver::calculateEnergyWaste(void)
{
    double timediffsecond;
    double dLight;
    int nPeopleCount;

    int result;

    if (!m_light.empty() && !m_peopleCount.empty())
    {
        // remove doublequotes
        m_light.erase(std::remove(m_light.begin(), m_light.end(), '\"' ), m_light.end());

        dLight = std::stod(m_light);

        // remove doublequotes
        m_peopleCount.erase(std::remove(m_peopleCount.begin(), m_peopleCount.end(), '\"' ), m_peopleCount.end());

        nPeopleCount = std::stoi(m_peopleCount);

        timediffsecond = abs(difftime(m_timestampL, m_timestampP));

        // check if time difference between light data and peopleCount data is valid
        if (timediffsecond > DIFFTIME)
        {
            result = OUTOFDATE;
            std::cout << "[EnergySaver] : OUTOFDATE: " << result << std::endl;
        }
        else if (dLight > DEFINITELY_TURNED_ON_LIGHT_VAL && nPeopleCount == 0)
        {
            result = (int)WASTING_ENERGY;
            std::cout << "[EnergySaver] : EnergySaverResult:" << result << " WASTING ENERGY " << std::endl;
        }
        else if (dLight > POSSIBLY_TURNED_ON_LIGHT_VAL && nPeopleCount == 0)
        {
            result = (int)ATTENTION_REQUIRED;
            std::cout << "[EnergySaver] : EnergySaverResult:" << result << " ENERGY REQUIRED " << std::endl;
        }
        else 
        {
            result = (int)NORMAL;
            std::cout << "[EnergySaver] : EnergySaverResult:" << result << " NORMAL " << std::endl;
        }

        std::cout << std::endl;

        m_energySaverResult = std::to_string(result);

        return SUCCESS;
    }

    return ERROR;
}
