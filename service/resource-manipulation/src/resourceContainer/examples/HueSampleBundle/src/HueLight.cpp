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

using namespace OIC::Service;

HueLight::HueLight()
{

}

HueLight::HueLight(HueConnector *connector, std::string address)
{
    m_address = address;
    m_connector = connector;
    initAttributes();
}

HueLight::~HueLight()
{

}

string HueLight::getAttribute(string attributeName)
{
    return BundleResource::getAttribute(attributeName);
}

void HueLight::initAttributes()
{
    BundleResource::setAttribute("on-off", "false");
    BundleResource::setAttribute("dim", "0");
    BundleResource::setAttribute("color", "0");
    /*m_mapAttributes.insert(std::pair<string, string>("on-off", "false"));
    m_mapAttributes.insert(std::pair<string, string>("dim", "0"));
    m_mapAttributes.insert(std::pair<string, string>("color", "0"));*/
}

void HueLight::setAttribute(string attributeName, string value)
{
    cout << "HueLight::setAttribute setting " << attributeName << " to " << value << std::endl;


    if (attributeName == "on-off")
    {
        m_connector->transmit(this->m_address + "/state", "{\"on\":" + value + "}");
    }

    if (attributeName == "dim")
    {
        //m_connector->transmit(this->m_address + "/state", "{\"bri\":" + (value * 2.5) + "}");
        m_connector->transmit(this->m_address + "/state", "{\"bri\":" + value + "}");
    }

    if (attributeName == "color")
    {
        //m_connector->transmit(this->m_address+ "/state", "{\"hue\":" + (value * 650)  + "}");
        m_connector->transmit(this->m_address + "/state", "{\"hue\":" + value   + "}");
    }
}

