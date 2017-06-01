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
#ifndef SMARTHOME_API_EXAMPLES_LIGHTSERVERSAMPLE_H_
#define SMARTHOME_API_EXAMPLES_LIGHTSERVERSAMPLE_H_

#include <iostream>

#include <CommonApi.h>
#include <LightDevice.h>
#include <PropertyBundle.h>
#include <ResourceQuery.h>

using namespace OIC::Service::SH;
class MyColorDelegate: public SHBaseResourceDelegate
{
public:
    virtual ~MyColorDelegate() {};
public:
    ResultCode onGet(RequestId requestId, const ResourceQuery& query);
    ResultCode onSet(RequestId requestId, const PropertyBundle& bundle, const ResourceQuery& query);
};

class UserColorResource: public SHBaseResource
{
public:
    UserColorResource();
    virtual ~UserColorResource();

    void setColor(std::string color);
    std::string getColor();
    void setColorDelegate(MyColorDelegate* delegate);
};

class UserLight: public LightDevice
{
public:
    UserColorResource color;
};

class MyBinarySwitchDelegate: public BinarySwitchResourceDelegate
{
public:
    virtual ~MyBinarySwitchDelegate() {};
public:
    virtual ResultCode turnOnCallback();
    virtual ResultCode turnOffCallback();
};
#endif /* SMARTHOME_API_EXAMPLES_LIGHTSERVERSAMPLE_H_ */
