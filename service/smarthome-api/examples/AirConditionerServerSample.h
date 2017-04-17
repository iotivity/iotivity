/* ****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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
#ifndef SMARTHOME_API_EXAMPLES_AIRCONDITIONER_SERVER_SAMPLE_H_
#define SMARTHOME_API_EXAMPLES_AIRCONDITIONER_SERVER_SAMPLE_H_

#include <AirConditionerDevice.h>
#include <CommonApi.h>

using namespace OIC::Service::SH;

class MyBinarySwitchDelegate : public BinarySwitchResourceDelegate
{
public:
    virtual ResultCode turnOnCallback();
    virtual ResultCode turnOffCallback();
};

class MyTemperatureDelegate : public TemperatureResourceDelegate
{
public:
    virtual ResultCode setTemperatureCallback(double temperature, std::string units);
};

#endif /* SMARTHOME_API_EXAMPLES_AIRCONDITIONER_SERVER_SAMPLE_H_ */
