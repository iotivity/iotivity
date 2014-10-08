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

/// @file Capability.h

/// @brief

#ifndef __OC_CAPABILITY__
#define __OC_CAPABILITY__

#include <string>

namespace OC
{

    using namespace std;

    enum CAPABILITY_TYPE
    {
        CAPABILITY_NONE,
        CAPABILITY_POWER_STATE,
        CAPABILITY_TEMPERATURE,
        CAPABILITY_DESIRED_TEMPERATURE,
        CAPABILITY_LEVEL
    };

    enum PERMISSION
    {
        PERMISSION_RO, PERMISSION_RW, PERMISSION_WO
    };

    static std::string s_capabilityTypes[] =
    { "none", "powerstate", "temperature", "desired_temperature", "level" };

    static unsigned int countofcapability_types = (sizeof(s_capabilityTypes)
            / sizeof(s_capabilityTypes[0]));

    inline CAPABILITY_TYPE convertCapabilityToEnum(std::string type)
    {
        for (unsigned int i = 0; i < countofcapability_types; ++i)
        {
            if (type == s_capabilityTypes[i])
            {
                return CAPABILITY_TYPE(i);
            }
        }

        return CAPABILITY_TYPE::CAPABILITY_NONE;
    }

    class Capability
    {
    public:
        Capability(void)
        {
            m_type = CAPABILITY_NONE;
            m_permission = PERMISSION_RO;
        }
        Capability(std::string name, CAPABILITY_TYPE type, std::string data, PERMISSION permission)
        {
            this->m_name = name;
            this->m_type = type;
            this->m_data = data;
            this->m_permission = permission;
        }

        ~Capability(void)
        {            
        }

        std::string m_name;
        CAPABILITY_TYPE m_type;
        std::string m_data;
        PERMISSION m_permission;
    };

}

#endif // __OC_CAPABILITY__
