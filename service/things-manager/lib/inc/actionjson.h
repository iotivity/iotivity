//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

/// @file actionjson.h

/// @brief

#ifndef __OC_ACTIONJSON__
#define __OC_ACTIONJSON__

#include "json.h"

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include <Group.h>
#include <Capability.h>
#include <ActionSet.h>



std::string escapeJsonString(const std::string& input)
{
    std::ostringstream ss;
    for (auto iter = input.cbegin(); iter != input.cend(); iter++)
    {
        //C++98/03:
        //for (std::string::const_iterator iter = input.begin(); iter != input.end(); iter++) {
        switch (*iter)
        {
            case '\\':
                ss << "\\\\";
                break;
            case '"':
                ss << "\\\"";
                break;
            case '/':
                ss << "\\/";
                break;
            case '\b':
                ss << "\\b";
                break;
            case '\f':
                ss << "\\f";
                break;
            case '\n':
                ss << "\\n";
                break;
            case '\r':
                ss << "\\r";
                break;
            case '\t':
                ss << "\\t";
                break;
            default:
                ss << *iter;
                break;
        }
    }
    return ss.str();
}

std::string JsonPackCapability(OC::Capability *capa)
{
    //string, enum(int), string, enum(int)
    std::stringstream ss;
    ss << "{";

    ss << "\"name\":\"" << capa->m_name << "\", ";
    ss << "\"type\":" << capa->m_type << ", ";
    ss << "\"data\":\"" << capa->m_data << "\", ";
    ss << "\"perm\":" << capa->m_permission << "";

    ss << "}";

    return ss.str();

}
std::string JsonPackAction(OC::Action *ac)
{

    //string, object(null), object
    std::stringstream ss;
    ss << "{";
    ss << "\"duid\":\"" << ac->m_duid << "\", ";
    ss << "\"rule\":\"\",";
    ss << "\"capa\": " << JsonPackCapability(ac->m_capability);

    ss << "}";

    return ss.str();

}
std::string JsonPackActionSet(OC::ActionSet *as)
{
    //string, int,  object
    std::stringstream ss;
    ss << "{";
    ss << "\"name\":\"" << as->m_name << "\", ";
    ss << "\"asid\":" << as->m_actuationSetId << ",";
    ss << "\"acts\":[";
    for (unsigned int i = 0; i < as->m_listActions.size(); ++i)
    {
        if (i != 0)
        {
            ss << ", ";
        }

        ss << JsonPackAction(as->m_listActions.at(i));
    }
    ss << "]";
    ss << "}";

    return ss.str();
}
std::string JsonPackActionSetList(std::vector<OC::ActionSet*> asList)
{
    std::stringstream ss;
    ss << "{";
    ss << "\"actionset_list\":[";

    for(unsigned int i = 0 ; i < asList.size() ; ++i)
    {
        if (i != 0)
        {
            ss << ", ";
        }
        ss << JsonPackActionSet(asList.at(i));
    }
    ss << "]";
    ss << "}";

    return ss.str();
}


OC::ActionSet* JsonUnpackActionSet(const char* jsonStr)
{
    Json::Value root;
    Json::Reader reader;
        bool isParsed = reader.parse(jsonStr, root);

    if (!isParsed)
    {
        std::cout << "PARSE FAILED" << std::endl;
        return NULL;
    }

    OC::ActionSet* actionSet = new OC::ActionSet();
    actionSet->m_name = root["name"].asCString();
    actionSet->m_actuationSetId = root["asid"].asInt();

    const Json::Value array = root["acts"];
    for (unsigned int i = 0; i < array.size(); ++i)
    {
        OC::Action* action = new OC::Action();
        OC::Capability* capa =
                new OC::Capability(array[i].get("capa", "").get("name", "").asCString(),
                        static_cast< OC::CAPABILITY_TYPE >(array[i].get("capa", "").get("type", "").asInt()),
                        array[i].get("capa", "").get("data", "").asCString(),
                        static_cast< OC::PERMISSION >(array[i].get("capa", "").get("perm", "").asInt()));

        action->m_duid = array[i].get("duid", "").asCString();

        action->m_capability = capa;

        //TODO:rule
        action->m_rule = NULL;
        actionSet->m_listActions.push_back(action);
    }

    return actionSet;
}
std::vector<OC::ActionSet*> JsonUnpackActionSetList(const char* jsonStr)
{

    Json::Value root;
    Json::Reader reader;

    std::vector<OC::ActionSet*> returnList;

    bool isParsed = reader.parse(jsonStr, root);

    if (!isParsed)
    {
        std::cout << "PARSE FAILED" << std::endl;
        return returnList;
    }

    const Json::Value array = root["actionset_list"];
    for (unsigned int i = 0; i < array.size(); ++i)
    {
        OC::ActionSet* newActionSet = new OC::ActionSet();
        newActionSet = JsonUnpackActionSet(array[i].toStyledString().c_str());

        if(newActionSet != NULL)
        {
            returnList.push_back(newActionSet);
        }
    }

    return returnList;
}
std::string JsonPackDevice(OC::Device* device)
{
    std::stringstream ss;
    std::vector< OC::Capability* > capas;
    std::vector< OC::Capability* >::iterator it;

    ss << "{";
    ss << "\"name\":\"" << device->m_name << "\", ";
    ss << "\"duid\":\"" << device->m_duid << "\", ";
    ss << "\"type\":" << device->m_type << ", ";
    ss << "\"address\":\"" << device->m_address << "\", ";
    ss << "\"capas\":" << "[";

    capas = device->m_listCapabilities;
    for (it = capas.begin(); it != capas.end(); ++it)
    {
        /*
         ss << "\"name\":\"" << (*it)->name << "\", ";
         ss << "\"type\":" <<  (*it)->type  << ", ";
         ss << "\"data\":\"" << (*it)->data  << "\", ";
         ss << "\"perm\":" <<  (*it)->permission  << "";
         */
        ss << JsonPackCapability((*it));
        if ((it + 1) != capas.end())
            ss << ", ";
    }
    ss << "]";
    ss << "}";

    return ss.str();
}

OC::Device* JsonUnpackDevice(const char* jsonStr)
{

    Json::Value root;
    Json::Reader reader;

    bool isParsed = reader.parse(jsonStr, root);

    cout << jsonStr << endl;

    if (!isParsed)
    {
        std::cout << "PARSE FAILED" << std::endl;
        return NULL;
    }

    OC::Device* device = new OC::Device();
    device->m_name = root["name"].asCString();
    device->m_duid = root["duid"].asCString();
    device->m_type = static_cast< OC::DEVICE_TYPE >(root["type"].asInt());
    device->m_address = root["address"].asCString();

    const Json::Value array = root["capas"];
    for (unsigned int i = 0; i < array.size(); ++i)
    {

//	    cout << array[i].get("name","").asCString() << endl;

        OC::Capability* capa = new OC::Capability(array[i].get("name", "").asCString(),
                static_cast< OC::CAPABILITY_TYPE >(array[i].get("type", "0").asInt()),
                array[i].get("data", "").asCString(),
                static_cast< OC::PERMISSION >(array[i].get("perm", "").asInt()));

        device->m_listCapabilities.push_back(capa);
    }

    return device;

}

bool JsonPackDeviceList(std::vector< OC::Device* > deviceList, std::string& packString )
{
    std::stringstream ss;
    ss << "{";
    ss << "\"devices\":[";

    for(unsigned int i = 0 ; i < deviceList.size() ; ++i)
    {
        if (i != 0)
        {
            ss << ", ";
        }
        ss << JsonPackDevice(deviceList.at(i));
    }
    ss << "]";
    ss << "}";

    packString = ss.str();

    return true;
}
bool JsonUnpackDeviceList(const char* jsonStr, std::vector< OC::Device* >& deviceList)
{

    //std::cout << jsonStr << std::endl;
    Json::Value root;
    Json::Reader reader;

    bool isParsed = reader.parse(jsonStr, root);

    if (!isParsed)
    {
        std::cout << "PARSE FAILED" << std::endl;
        return false;
    }
    //std::vector<OC::DEVICE*> deviceList;

    const Json::Value deviceArray = root["devices"];

    for (unsigned int j = 0; j < deviceArray.size(); ++j)
    {

        OC::Device* device = new OC::Device();
        device->m_name = deviceArray[j].get("name", "").asCString();
        device->m_duid = deviceArray[j].get("duid", "").asCString();
        device->m_type = static_cast< OC::DEVICE_TYPE >(deviceArray[j].get("type", "").asInt());
        device->m_address = deviceArray[j].get("address", "").asCString();

        const Json::Value array = deviceArray[j].get("capas", "");
        for (unsigned int i = 0; i < array.size(); ++i)
        {
            OC::Capability* capa = new OC::Capability(array[i].get("name", "").asCString(),
                    static_cast< OC::CAPABILITY_TYPE >(array[i].get("type", "").asInt()),
                    array[i].get("data", "").asCString(),
                    static_cast< OC::PERMISSION >(array[i].get("perm", "").asInt()));

            device->m_listCapabilities.push_back(capa);
        }
        deviceList.push_back(device);
    }
    return true;
}

std::string JsonPackDeviceListReturn(std::vector< OC::Device* > deviceList )
{
    std::string returnStr;
    JsonPackDeviceList(deviceList,returnStr);
    return returnStr;
}
std::vector< OC::Device* > JsonUnpackDeviceListReturn(const char* jsonStr)
{
    std::vector< OC::Device* > deviceList;
    JsonUnpackDeviceList(jsonStr,deviceList);
    return deviceList;
}

std::string JsonPackGroup(OC::Group* group)
{

    std::stringstream ss;
    ss << "{";
    ss << "\"guid\":\"" << group->m_guid << "\", ";
    ss << "\"name\":\"" << group->m_name << "\", ";

    ss << "\"actionset_ids\": [";
    for (auto it = group->m_listActionSet.begin(); it != group->m_listActionSet.end(); ++it)
    {
        ss << (*it)->m_actuationSetId;

        if ((it + 1) != group->m_listActionSet.end())
        {
            ss << ",";
        }

    }
    ss << "],";
    ss << "\"mandatory_ids\": [";
    for (auto it = group->m_listMandatoryDevices.begin(); it != group->m_listMandatoryDevices.end(); ++it)
    {
        ss << "\"" << (*it)->m_duid << "\"";

        if ((it + 1) != group->m_listMandatoryDevices.end())
        {
            ss << ",";
        }

    }
    ss << "],";
    ss << "\"optional_ids\": [";
    for (auto it = group->m_listOptionalDevices.begin(); it != group->m_listOptionalDevices.end(); ++it)
    {
        ss << "\"" << (*it)->m_duid << "\"";

        if ((it + 1) != group->m_listOptionalDevices.end())
        {
            ss << ",";
        }

    }
    ss << "]";
    ss << "}";

    return ss.str();
}

OC::GroupInfo* JsonUnpackGroup(const char* jsonStr)
{
    std::cout << jsonStr << std::endl;
    Json::Value root;
    Json::Reader reader;

    OC::GroupInfo* info = new OC::GroupInfo();

    bool isParsed = reader.parse(jsonStr, root);

    if (!isParsed)
    {
        std::cout << "PARSE FAILED" << std::endl;
        delete info;
        return NULL;
    }

    info->m_guid = root["guid"].asString();
    info->m_name = root["name"].asString();

    const Json::Value actions = root["actionset_ids"];
    for (unsigned int i = 0; i < actions.size(); ++i)
    {
        info->m_listActuationSets.push_back(actions[i].asUInt());
    }

    const Json::Value mandantory = root["mandatory_ids"];
    for (unsigned int i = 0; i < mandantory.size(); ++i)
    {
        info->m_listMandatoryDevices.push_back(mandantory[i].asString());
    }

    const Json::Value optional = root["optional_ids"];
    for (unsigned int i = 0; i < optional.size(); ++i)
    {
        info->m_listOptionalDevices.push_back(optional[i].asString());
    }

    return info;
}

std::string ReplaceAll(std::string originalStr, const std::string& oldStr,
        const std::string& newStr)
{

    unsigned int start = 0;

    while ((start = originalStr.find(oldStr, start)) != std::string::npos)
    {
        originalStr.replace(start, oldStr.length(), newStr);
        start += newStr.length();
    }

    return originalStr;
}
std::string ActionSetToEncodedString(OC::ActionSet* as)
{

    std::string jsonStr = JsonPackActionSet(as);
    std::string encodeStr = escapeJsonString(jsonStr);

    return encodeStr;
}

OC::ActionSet* EncodedStringToActionSet(std::string str)
{

    OC::ActionSet* as = JsonUnpackActionSet(str.c_str());

    return as;
}
std::vector< std::string > &split(const std::string &s, char delim,
        std::vector< std::string > &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        elems.push_back(item);
    }
    return elems;
}

std::vector< std::string > split(const std::string &s, char delim)
{
    std::vector< std::string > elems;
    split(s, delim, elems);
    return elems;
}
#endif
