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

/// @file Group.h

/// @brief

#ifndef __OC_GROUP__
#define __OC_GROUP__

#include <vector>

#include "Device.h"
#include "ActionSet.h"
#include "Rule.h"

namespace OC
{

    using namespace std;

    typedef std::string UID;
    typedef std::string GID;
    typedef GID GROUP_ID;
    typedef unsigned int APPID;
    typedef unsigned int RESULT;

    class Group
    {

    public:
        typedef vector< OC::ActionSet* >::iterator const_iterator;

        GID m_guid;
        string m_name;
        string m_description;

        vector< OC::ActionSet* > m_listActionSet;

        vector< OC::Device* > m_listMandatoryDevices;
        vector< OC::Device* > m_listOptionalDevices;

        unsigned int addActionSet(OC::ActionSet* set);
        vector< OC::ActionSet* >& getActionSet();
        OC::ActionSet& findActionSet(ASID asid);
        const_iterator findIteratorOfActionSet(ASID asid);
    };

    /**
     *   @brief  GroupList is a data class for representing Group List.
     *
     */
    class GroupInfo
    {

    public:
        /**
         * Constructor for GROUP_LIST.
         */
        GroupInfo()
        {

        }
        /**
         * Virtual destructor
         */
        ~GroupInfo()
        {

        }

        string m_name;
        GROUP_ID m_guid;
        vector< ACTIONSET_ID > m_listActuationSets;
        vector< DEVICE_ID > m_listMandatoryDevices;
        vector< DEVICE_ID > m_listOptionalDevices;
    };

}

#endif // /* GROUP_H_ */
