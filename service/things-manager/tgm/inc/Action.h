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

/// @file Action.h

/// @brief

#ifndef __OC_ACTION__
#define __OC_ACTION__

#include <vector>

#include "Device.h"
#include "Capability.h"
#include "ActionRule.h"

namespace OC
{

    using namespace std;

    typedef unsigned int ACTION_ID;
    typedef ACTION_ID AID;

    class Action
    {

    public:
        Action()
        {
            m_capability = NULL;
            m_rule = NULL;
        }
        ~Action()
        {

        }

        DEVICE_ID m_duid;
        Capability* m_capability;
        ActionRule* m_rule;
    };
}

#endif /* __OC_ACTION__ */
