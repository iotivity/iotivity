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

#include "HueLight.h"

#include <iostream>

using namespace std;
using namespace OIC::Service;

HueLight::HueLight()
{
    m_connector = nullptr;
}

HueLight::HueLight(HueConnector *connector, std::string address)
{
    m_address = address;
    m_connector = connector;
    initAttributes();
}

HueLight::~HueLight()
{
    m_connector = nullptr;
}

void HueLight::initAttributes()
{

    BundleResource::setAttribute("on-off", false);
    BundleResource::setAttribute("dim", 0);
    BundleResource::setAttribute("color", 0);
}

RCSResourceAttributes &HueLight::getAttributes()
{
    return BundleResource::getAttributes();
}

RCSResourceAttributes::Value HueLight::getAttribute(const std::string &key)
{
    cout << "HueLight::getAttribute called for " << key <<  " called" << endl;
    return BundleResource::getAttribute(key);
}

void HueLight::setAttribute(std::string attributeName, RCSResourceAttributes::Value &&value)
{
    cout << "HueLight::setAttribute setting " << attributeName << " to " << value.toString() <<
         std::endl;

    if (attributeName == "on-off")
    {
        m_connector->transmit(this->m_address + "/state", "{\"on\":" + value.toString() + "}");
    }

    if (attributeName == "dim")
    {
        // needs conversion * 2.5
        m_connector->transmit(this->m_address + "/state", "{\"bri\":" + value.toString() + "}");
    }

    if (attributeName == "color")
    {
        // needs conversion *650
        m_connector->transmit(this->m_address + "/state", "{\"hue\":" + value.toString()   + "}");
    }

    BundleResource::setAttribute(attributeName, std::move(value));
}

