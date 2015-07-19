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

#ifndef DISCOMFORTINDEXSENSORRESOURCE_H_
#define DISCOMFORTINDEXSENSORRESOURCE_H_

#include <iostream>

#include "SoftSensorResource.h"
#include "DiscomfortIndexSensor.h"

using namespace DiscomfortIndexSensorName;
using namespace OIC::Service;

class DiscomfortIndexSensorResource : public SoftSensorResource
{
    public:
        DiscomfortIndexSensorResource();
        DiscomfortIndexSensorResource(vector<string> inputAttributes);
        ~DiscomfortIndexSensorResource();

        ResourceAttributes::Value getAttribute(string attributeName);
        void setAttribute(string attributeName, ResourceAttributes::Value value);
        virtual void initAttributes();

        void setInputAttribute(SensorData input);

    private:
        DiscomfortIndexSensor *m_pDiscomfortIndexSensor;
};

#endif
