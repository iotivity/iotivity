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

#include "ResourceTypeEnum.h"

using namespace std;

#include <string>

string getResourceString(const ResourceTypeEnum &resourceType)
{
    string outPut = "invalid";
    switch (resourceType)
    {
        case RT_AccessPoint:
            outPut = "AccessPoint";
            break;
        case RT_AccessPoints:
            outPut = "AccessPoints";
            break;
        case RT_Alarm:
            outPut = "Alarm";
            break;
        case RT_Alarms:
            outPut = "Alarms";
            break;
        case RT_Capability:
            outPut = "Capability";
            break;
        case RT_Configuration:
            outPut = "Configuration";
            break;
        case RT_Detection:
            outPut = "Detection";
            break;
        case RT_Device:
            outPut = "Device";
            break;
        case RT_Devices:
            outPut = "Devices";
            break;
        case RT_Doors:
            outPut = "Doors";
            break;
        case RT_Door:
            outPut = "Door";
            break;
        case RT_DRLC:
            outPut = "DRLC";
            break;
        case RT_Energy:
            outPut = "Energy";
            break;
        case RT_EnergyConsumption:
            outPut = "EnergyConsumption";
            break;
        case RT_Humidity:
            outPut = "Humidity";
            break;
        case RT_Information:
            outPut = "Information";
            break;
        case RT_Level:
            outPut = "Level";
            break;
        case RT_Light:
            outPut = "Light";
            break;
        case RT_LongPollingNotification:
            outPut = "LongPollingNotification";
            break;
        case RT_Messages:
            outPut = "Messages";
            break;
        case RT_Message:
            outPut = "Message";
            break;
        case RT_Mode:
            outPut = "Mode";
            break;
        case RT_Movement:
            outPut = "Movement";
            break;
        case RT_Network:
            outPut = "Network";
            break;
        case RT_Networks:
            outPut = "Networks";
            break;
        case RT_Operation:
            outPut = "Operation";
            break;
        case RT_Record:
            outPut = "Record";
            break;
        case RT_Records:
            outPut = "Records";
            break;
        case RT_Remote:
            outPut = "Remote";
            break;
        case RT_Temperatures:
            outPut = "Temperatures";
            break;
        case RT_Temperature:
            outPut = "Temperature";
            break;
        case RT_Thermostat:
            outPut = "Thermostat";
            break;
        case RT_Time:
            outPut = "Time";
            break;
        case RT_UsageLimit:
            outPut = "UsageLimit";
            break;
        case RT_UsageLimits:
            outPut = "UsageLimits";
            break;
        case RT_Usages:
            outPut = "Usages";
            break;
        case RT_Version:
            outPut = "Version";
            break;
        case RT_Versions:
            outPut = "Versions";
            break;
        case RT_WiFi:
            outPut = "WiFi";
            break;
        default:
            outPut = "invalid";
    }
    return outPut;
}
