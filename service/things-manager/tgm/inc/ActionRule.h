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

/// @file ActionRule.h

/// @brief

#ifndef __OC_ACTION_RULE__
#define __OC_ACTION_RULE__

#include "Device.h"

namespace OC
{
    enum ACTION_TYPE
    {
        TIMER
    };

    class ActionRule
    {
    public:
        ActionRule(void)
        {
            type = TIMER;
            value = 0;
        }
        ~ActionRule(void)
        {
            
        }

        ACTION_TYPE type;
        DEVICE_ID m_targetDuid; // TBD
        unsigned int value;
    };
}

#endif /* __OC_ACTION_RULE__ */
