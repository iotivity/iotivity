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

/// @file Rule.h

/// @brief

#ifndef __OC_RULE__
#define __OC_RULE__

#include <string>
#include <vector>

#include "Action.h"

using namespace std;

namespace OC
{

    typedef string RULE_ID;

    class Rule
    {
    public:
        Rule(void)
        {

        }
        ~Rule(void)
        {
            
        }

        string m_name;
        RULE_ID m_ruleId;
        string m_conditionKey;
        string m_conditionValue;
        vector< Action* > m_listActions;
    };

}
#endif /* __OC_RULE__ */
