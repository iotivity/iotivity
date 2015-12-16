//******************************************************************
// 	
// Copyright 2015 Jake Martin Choi All Rights Reserved.
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

#include "ActivitySensorResource.h"


ActivitySensorResource::ActivitySensorResource()
{
    m_pActivitySensor = new ActivitySensor();
}

ActivitySensorResource::~ActivitySensorResource()
{
    delete m_pActivitySensor;
}

void ActivitySensorResource::handleSetAttributesRequest(
    RCSResourceAttributes &value)
{
    BundleResource::setAttributes(value);
}

RCSResourceAttributes &ActivitySensorResource::handleGetAttributesRequest()
{
    return BundleResource::getAttributes();
}

void ActivitySensorResource::executeLogic()
{
	std::string activityResult;
		
	if(m_pActivitySensor->executeActivitySensorLogic(&m_mapInputData, &activityResult) != -1)
		setAttribute("activityresult", RCSResourceAttributes::Value(activityResult.c_str()));

}

void ActivitySensorResource::onUpdatedInputResource(const std::string attributeName,
        std::vector<RCSResourceAttributes::Value> values)
{

	m_mapInputData.clear();


	if(!attributeName.compare("heartrate"))
		m_mapInputData.insert(std::make_pair("heartrate", values.back().toString()));
	if(!attributeName.compare("gps"))
		m_mapInputData.insert(std::make_pair("gps", values.back().toString()));
	if(!attributeName.compare("acceleration"))
		m_mapInputData.insert(std::make_pair("acceleration", values.back().toString()));

	executeLogic();
}
