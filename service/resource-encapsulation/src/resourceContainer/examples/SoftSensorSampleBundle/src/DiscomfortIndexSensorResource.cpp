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
}

DiscomfortIndexSensorResource::~DiscomfortIndexSensorResource()
{
    delete m_pDiscomfortIndexSensor;
}

void DiscomfortIndexSensorResource::initAttributes()
{
    SoftSensorResource::initAttributes();
}

RCSResourceAttributes &DiscomfortIndexSensorResource::getAttributes()
{
    return SoftSensorResource::getAttributes();
}

void DiscomfortIndexSensorResource::setAttribute(std::string key,
        RCSResourceAttributes::Value &&value)
{
    SoftSensorResource::setAttribute(key, std::move(value));
}

RCSResourceAttributes::Value DiscomfortIndexSensorResource::getAttribute(const std::string &key)
{
    return SoftSensorResource::getAttribute(key);
}

void DiscomfortIndexSensorResource::executeLogic()
{
    std::map<std::string, std::string> mapInputData;
    std::string strTemp = getAttribute("temperature").toString();
    std::string strHumid = getAttribute("humidity").toString();
    std::string strDiscomfortIndex;

    if (!strTemp.empty() && !strHumid.empty())
    {
        mapInputData.insert(std::make_pair("temperature", strTemp));
        mapInputData.insert(std::make_pair("humidity", strHumid));

        m_pDiscomfortIndexSensor->executeDISensorLogic(&mapInputData, &strDiscomfortIndex);

        setAttribute("discomfortIndex", RCSResourceAttributes::Value(strDiscomfortIndex.c_str()));
    }
}