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

/// @file Device.h

/// @brief

#ifndef __OC_DEVICE__
#define __OC_DEVICE__

#include <string>
#include <vector>
#include <cstdio>

#include "Capability.h"

using namespace std;

namespace OC
{

    typedef std::string DEVICE_ID;
    typedef std::string ADDR;

    enum DEVICE_TYPE
    {
        NONE, PHONE, TV, AIR_CONDITIONER, TEMPERATURE_SENSOR, LIGHT, BOOKMARK, MUSIC_PLAYER
    };

    enum DEVICE_STATE
    {
        DEVICE_ADDED, DEVICE_REMOVED, DEVICE_UPDATED
    };

    static std::string s_deviceTypes[] =
    { "core.none", "core.phone", "core.tv", "core.air_conditioner", "core.temperature_sensor",
            "core.light", "core.bookmark", "core.music_player" };

    static unsigned int s_countOfDeviceTypes = (sizeof(s_deviceTypes) / sizeof(s_deviceTypes[0]));

    inline DEVICE_TYPE convertDeviceTypeToEnum(std::string type)
    {
        for (unsigned int i = 0; i < s_countOfDeviceTypes; ++i)
        {
            if (type == s_deviceTypes[i])
            {
                return DEVICE_TYPE(i);
            }
        }

        return DEVICE_TYPE::NONE;
    }

    class Device
    {
    public:

        string m_name;
        DEVICE_ID m_duid;
        DEVICE_TYPE m_type;
        ADDR m_address;

        vector< Capability* > m_listCapabilities;

        void printDevice()
        {
            printf("\n==========PRINT DEVICE=========\n");
            printf("name : %s // uid : %s  //  deviceType : %d \n", m_name.c_str(), m_duid.c_str(),
                    m_type);
            for (unsigned int i = 0; i < m_listCapabilities.size(); ++i)
            {
                printf("Capability %d : %s\n", i + 1, m_listCapabilities.at(i)->m_name.c_str());
            }
            printf("===============================\n");
        }
    };

}

#endif //__OC_DEVICE__
