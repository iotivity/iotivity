//******************************************************************
//
// Copyright 2015 Han Joo Chae, Jae Ho Jeon (Seoul National University) All Rights Reserved.
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

#include "IASensorResource.h"


IASensorResource::IASensorResource()
{
    m_pIASensor = new IASensor();
}

IASensorResource::~IASensorResource()
{
    delete m_pIASensor;
}

void IASensorResource::handleSetAttributesRequest(
    RCSResourceAttributes &value)
{
    BundleResource::setAttributes(value);
}

RCSResourceAttributes &IASensorResource::handleGetAttributesRequest()
{
    return BundleResource::getAttributes();
}

void IASensorResource::executeLogic()
{
    std::string strIAResult;

    if (m_pIASensor->executeIASensorLogic(&m_mapInputData, &strIAResult) != -1)
        setAttribute("IAresult", RCSResourceAttributes::Value(strIAResult.c_str()));
}

void IASensorResource::onUpdatedInputResource(const std::string attributeName,
        std::vector<RCSResourceAttributes::Value> values)
{
    // remove all existing data
    m_mapInputData.clear();

    if (!attributeName.compare("motion"))
        m_mapInputData.insert(std::make_pair("motion", values.back().toString()));

    if (!attributeName.compare("wireless"))
        m_mapInputData.insert(std::make_pair("wireless", values.back().toString()));

    executeLogic();
}
