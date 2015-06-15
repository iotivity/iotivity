//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

#include "DiscomfortIndexSensorResource.h"

DiscomfortIndexSensorResource::DiscomfortIndexSensorResource()
{
    m_pDiscomfortIndexSensor = new DiscomfortIndexSensor();

    m_mapAttributes.insert(std::pair<string, string>("temperature", "23"));
    m_mapAttributes.insert(std::pair<string, string>("humidity", "40"));
    m_mapAttributes.insert(std::pair<string, string>("discomfortIndex", "5"));

}

DiscomfortIndexSensorResource::~DiscomfortIndexSensorResource()
{
    delete m_pDiscomfortIndexSensor;
}

void DiscomfortIndexSensorResource::onGetRequest(std::map <std::string, std::string> *attributes)
{
    // TODO : make m_outputs into m_mapAttributes format
    *attributes = m_mapAttributes;
}

void DiscomfortIndexSensorResource::onSetRequest(std::map <std::string, std::string> attributes)
{
}

void DiscomfortIndexSensorResource::onInputDataReceived(std::vector <SensorData> inputs)
{
    m_vecInputs = inputs;
    m_pDiscomfortIndexSensor->runLogic(m_vecInputs);
}
