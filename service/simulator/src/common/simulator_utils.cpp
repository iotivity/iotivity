/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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

#include "simulator_utils.h"
#include "OCRepresentation.h"

std::string getPayloadString(const OC::OCRepresentation &rep)
{
    OCRepPayload *payload = rep.getPayload();
    if (!payload)
    {
        return "Empty payload";
    }
    std::ostringstream data;
    std::ostringstream dummy;

    std::string payloadType;
    OCStringLL *types;
    OCStringLL *interfaces;
    OCRepPayloadValue *values;

    // Iterate the payload list
    while (payload)
    {
        // Payload type
        payloadType = getPayloadTypeString(payload->base.type);
        if (!payloadType.empty())
            data << "Payload type: " << payloadType << std::endl;

        // URI
        if (NULL != payload->uri && strlen(payload->uri) > 0)
            data << "URI: " << payload->uri << std::endl;

        // Types
        types = payload->types;
        while (types)
        {
            if (NULL != types->value && strlen(types->value) > 0)
            {
                dummy << types->value;
                if (types->next)
                    dummy << ", ";
            }
            types = types->next;
        }
        if (!dummy.str().empty())
        {
            data << "Types: " << dummy.str() << std::endl;
            dummy.str("");
        }

        // Interfaces
        interfaces = payload->interfaces;
        while (interfaces)
        {
            if (NULL != interfaces->value && strlen(interfaces->value) > 0)
            {
                dummy << interfaces->value;
                if (interfaces->next)
                    dummy << ", ";
            }
            interfaces = interfaces->next;
        }
        if (!dummy.str().empty())
        {
            data << "Interfaces: " << dummy.str() << std::endl;
            dummy.str("");
        }

        // Values
        values = payload->values;
        while (values)
        {
            dummy << "\t" << values->name << ":" << rep.getValueToString(values->name) << std::endl;
            values = values->next;
        }
        if (!dummy.str().empty())
        {
            data << "Values:-" << std::endl;
            data << dummy.str();
            dummy.str("");
        }
        payload = payload->next;
        if (payload)
            data << "----------------" << std::endl;
    }

    return data.str();
}

std::string getPayloadTypeString(OCPayloadType type)
{
    std::string typeStr;
    switch (type)
    {
        case PAYLOAD_TYPE_INVALID:
            typeStr = "PAYLOAD_TYPE_INVALID";
            break;
        case PAYLOAD_TYPE_DISCOVERY:
            typeStr = "PAYLOAD_TYPE_DISCOVERY";
            break;
        case PAYLOAD_TYPE_DEVICE:
            typeStr = "PAYLOAD_TYPE_DEVICE";
            break;
        case PAYLOAD_TYPE_PLATFORM:
            typeStr = "PAYLOAD_TYPE_PLATFORM";
            break;
        case PAYLOAD_TYPE_REPRESENTATION:
            typeStr = "PAYLOAD_TYPE_REPRESENTATION";
            break;
        case PAYLOAD_TYPE_SECURITY:
            typeStr = "PAYLOAD_TYPE_SECURITY";
            break;
        case PAYLOAD_TYPE_PRESENCE:
            typeStr = "PAYLOAD_TYPE_PRESENCE";
            break;
    }
    return typeStr;
}

std::string getRequestString(const std::map<std::string, std::string> &queryParams,
                             const OC::OCRepresentation &rep)
{
    std::ostringstream data;
    std::ostringstream dummy;
    if (queryParams.size() > 0)
    {
        for (auto &qp : queryParams)
            dummy << qp.second << ",";
    }
    if (!dummy.str().empty())
    {
        data << "qp: " << dummy.str() << std::endl;
    }
    data << getPayloadString(rep);
    return data.str();
}

std::string getRequestString(const std::map<std::string, std::string> &queryParams)
{
    std::ostringstream data;
    std::ostringstream dummy;
    if (queryParams.size() > 0)
    {
        for (auto &qp : queryParams)
            dummy << qp.second << ",";
    }
    if (!dummy.str().empty())
    {
        data << "qp: " << dummy.str() << std::endl;
    }
    data << "Payload:  No payload";
    return data.str();
}
