//******************************************************************
//
// Copyright 2015 Duck Ju Kim (Seoul National University) All Rights Reserved.
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

#include "AcneSensorResource.h"


AcneSensorResource::AcneSensorResource()
{
    m_pAcneSensor = new AcneSensor();
}

AcneSensorResource::~AcneSensorResource()
{
    delete m_pAcneSensor;
}

void AcneSensorResource::handleSetAttributesRequest(
    RCSResourceAttributes &value)
{
    BundleResource::setAttributes(value);
}

RCSResourceAttributes &AcneSensorResource::handleGetAttributesRequest()
{
    return BundleResource::getAttributes();
}

void AcneSensorResource::executeLogic()
{
    std::string strAcneResult;

    if (m_pAcneSensor->executeAcneSensorLogic(&m_mapInputData, &strAcneResult) != -1)
        setAttribute("Acneresult", RCSResourceAttributes::Value(strAcneResult.c_str()));
}

void AcneSensorResource::onUpdatedInputResource(const std::string attributeName,
        std::vector<RCSResourceAttributes::Value> values)
{
    // remove all existing data
    m_mapInputData.clear();

    if (!attributeName.compare("temperature"))
        m_mapInputData.insert(std::make_pair("temperature", values.back().toString()));

    if (!attributeName.compare("humidity"))
        m_mapInputData.insert(std::make_pair("humidity", values.back().toString()));

    executeLogic();
}
