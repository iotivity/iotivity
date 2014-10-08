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

/// @file Group.cpp

/// @brief

#include "../inc/Group.h"
#include "../inc/Exception.h"

namespace OC
{

    unsigned int Group::addActionSet(ActionSet* set)
    {
        m_listActionSet.push_back(set);
        return m_listActionSet.size();
    }

    vector< ActionSet* >& Group::getActionSet()
    {
        return m_listActionSet;
    }

    ActionSet& Group::findActionSet(ASID asid)
    {
        vector< ActionSet* >::iterator it;

        for (it = m_listActionSet.begin(); it != m_listActionSet.end(); ++it)
        {
            if ((*it)->m_actuationSetId == asid)
            {
                return *(*it);
            }
        }

        throw GraphException("COULD NOT FIND ACTIONSET.");
    }

    vector< ActionSet* >::iterator Group::findIteratorOfActionSet(ASID asid)
    {
        vector< ActionSet* >::iterator it;

        for (it = m_listActionSet.begin(); it != m_listActionSet.end(); ++it)
        {
            if ((*it)->m_actuationSetId == asid)
                break;
        }

        return it;
    }
}

