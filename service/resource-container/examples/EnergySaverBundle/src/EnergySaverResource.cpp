//******************************************************************
//
// Copyright 2015 Sehi L'Yi (Seoul National University) All Rights Reserved.
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

#include "EnergySaverResource.h"


EnergySaverResource::EnergySaverResource()
{
    m_pEnergySaver = new EnergySaver();
}

EnergySaverResource::~EnergySaverResource()
{
    delete m_pEnergySaver;
}

void EnergySaverResource::handleSetAttributesRequest(
    RCSResourceAttributes &value)
{
    BundleResource::setAttributes(value);
}

RCSResourceAttributes &EnergySaverResource::handleGetAttributesRequest()
{
    return BundleResource::getAttributes();
}

void EnergySaverResource::executeLogic()
{
    std::string strEnergySaverResult;

    if (m_pEnergySaver->executeEnergySaverLogic(&m_mapInputData, &strEnergySaverResult) != -1)
        setAttribute("EnergySaverResult", RCSResourceAttributes::Value(strEnergySaverResult.c_str()));
}

void EnergySaverResource::onUpdatedInputResource(const std::string attributeName,
        std::vector<RCSResourceAttributes::Value> values)
{
    // remove all existing data
    m_mapInputData.clear();

    if (!attributeName.compare("light"))
        m_mapInputData.insert(std::make_pair("light", values.back().toString()));

    if (!attributeName.compare("peopleCount"))
        m_mapInputData.insert(std::make_pair("peopleCount", values.back().toString()));

    executeLogic();
}
