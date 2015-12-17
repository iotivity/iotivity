//******************************************************************
//
// Copyright 2015 Hyunwoo Lee (Seoul National University) All Rights Reserved.
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

#include "ACNESensorResource.h"

ACNESensorResource::ACNESensorResource()
{
    m_pACNESensor = new ACNESensor();
}

ACNESensorResource::~ACNESensorResource()
{
    delete m_pACNESensor;
}

void ACNESensorResource::handleSetAttributesRequest(RCSResourceAttributes &value)
{
    BundleResource::setAttributes(value);
}

RCSResourceAttributes &ACNESensorResource::handleGetAttributesRequest()
{
    return BundleResource::getAttributes();
}

void ACNESensorResource::executeLogic()
{
    std::string strACNEResult;

    if (m_pACNESensor->executeACNESensorLogic(&m_mapInputData, &strACNEResult) != -1)
        setAttribute("ACNEresult", RCSResourceAttributes::Value(strACNEResult.c_str()));
}

void ACNESensorResource::onUpdatedInputResource(const std::string attributeName,
        std::vector<RCSResourceAttributes::Value> values)
{
    // remove all existing data
    m_mapInputData.clear();

    if (!attributeName.compare("temp"))
        m_mapInputData.insert(std::make_pair("temp", values.back().toString()));

    if (!attributeName.compare("oil"))
        m_mapInputData.insert(std::make_pair("oil", values.back().toString()));

    if (!attributeName.compare("moist"))
        m_mapInputData.insert(std::make_pair("moist", values.back().toString()));

    executeLogic();
}

