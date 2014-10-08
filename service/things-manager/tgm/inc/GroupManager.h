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

/// @file GroupManager.h

/// @brief

#ifndef __OC_GROUP_MANAGER__
#define __OC_GROUP_MANAGER__

#include <vector>
#include <map>
#include <string>
#include <cstring>

#include <cstdio>
#include <iostream>

#include "Exception.h"
#include "Group.h"
#include "Device.h"
#include "Capability.h"

#include "ActionSet.h"


namespace OC
{

    using namespace std;

    class GroupManager
    {
    private:
        static string LOG_TAGS;

        vector< Group* > m_listGroups;
        static GroupManager* s_instance;

        bool m_isPresenceCheking;

        map< UID, vector< GID > > m_mapGroupListForDevice;
        map< GID, APPID > m_mapAppIdForGroup;
        map< APPID, GID > m_mapGroupIdForApp;

        GroupManager() :
                m_isPresenceCheking(true)
        {

            if (!m_listGroups.empty())
                m_listGroups.clear();

        }
        ~GroupManager()
        {
            if(m_isPresenceCheking)
            {
                m_isPresenceCheking = false;
            }
        }

    public:
        static GroupManager* getInstance();

        //about Resource

        int createGroupResource(string guid, string gname);
        int removeGroupResource(string guid);
//        int findGroupResourceMembers(DEVICE_TYPE[]);

        Device* findDeviceInGroups(DEVICE_ID);

        //CRUD
        unsigned int getNumberofGroupMembers(GID);
        unsigned int addGroupMember(GID, DEVICE_ID);
        unsigned int addGroupMember(GID, Device*);
        unsigned int addGroupMember(GID, vector< Device* > mandatory, vector< Device* > optional);
        //unsigned int changeGroupMember(GID, DEVICE_ID, DEVICE_ID);
        unsigned int removeGroupMember(GID, DEVICE_ID);


        //Device* getGroupMember(UID duid);
        Group* getGroup(GID gid)
        {
            vector< Group* >::iterator it;

            for (it = m_listGroups.begin(); it != m_listGroups.end(); ++it)
            {
                if ((*it)->m_guid == gid)
                {
                    return (*it);
                }
            }

            throw GraphInternalServiceException("GROUP ID NOT FOUND");
        }

        const vector< Group* > getGroups()
        {
            return m_listGroups;
        }


        ASID createActionSet(GID gid, ActionSet* pset);
        ActionSet* readActionSetFromDesc(std::string desc);
        ActionSet* readActionSet(GID gid, ASID asid);

        bool updateActionSet(GID gid, ActionSet* pset);
        bool deleteActionSet(GID gid, ASID asid);
        bool deleteAction(GID gid, ASID asid, AID aid);
       // bool executeActionSet(GID gid, ASID asid);
       // bool executeActionSet(string desc);

 //       ASID createActionSet(GID gid, string desc);
 //       bool updateActionSet(GID gid, string desc);
 //       bool deleteActionSet(GID gid, string desc);
        bool deleteAction(GID gid, string desc, string details);

        static CAPABILITY_TYPE searchCandidate(string capa)
        {
            if (strcmp(capa.c_str(), "CAPABILITY_TEMPERATURE") == 0)
            {
                return CAPABILITY_TYPE::CAPABILITY_TEMPERATURE;
            }
            else if (strcmp(capa.c_str(), "CAPABILITY_DESIRED_TEMPERATURE") == 0)
            {
                return CAPABILITY_TYPE::CAPABILITY_DESIRED_TEMPERATURE;
            }
            else if (strcmp(capa.c_str(), "CAPABILITY_POWER_STATE") == 0)
            {
                return CAPABILITY_TYPE::CAPABILITY_POWER_STATE;
            }

            return CAPABILITY_TYPE::CAPABILITY_NONE;
        }

        GID findRelatedGroup(APPID appId);


#ifdef __TEST__
        void bindTo(OC::OCServer& server, const std::string& base_URI)
        {
            using OC::OCReflect::method_signature;
            using OC::OCReflect::method_binding;

            using OC::OCReflect::property_signature;
            using OC::OCReflect::property_binding;
            using OC::OCReflect::property_type;

            using OC::OCReflect::bind_service;
            using OC::OCReflect::bind_property;

            server.registerResource(this, base_URI);

            // BindService
            bind_service(server,
                    this,
                    &GroupManager::getInstance,
                    "getInstance",
                    property_type::nil,
                    property_type::nil);

            bind_service(server,
                    this,
                    &GroupManager::createGroupResource,
                    "CreateGroupResource",
                    property_type::integer,// return type
                    property_type::integer,// first arguments, APPID
                    property_type::string// second arguments, Group Name
            );

            bind_service(server,
                    this,
                    &GroupManager::findGroupResourceMembers,
                    "FindGroupResourceMembers",
                    property_type::integer,
                    property_type::list
            );

            bind_service(server,
                    this,
                    &GroupManager::getNumberofGroupMembers,
                    "GetNumberofGroupMembers",
                    property_type::integer,
                    property_type::integer
            );

            bind_service(server,
                    this,
                    static_cast<unsigned int (GroupManager::*)(unsigned int, DID)>(&GroupManager::addGroupMember),
                    "AddGroupMember",
                    property_type::integer,
                    property_type::integer,
                    property_type::string
            );

            bind_service(server,
                    this,
                    &GroupManager::changeGroupMember,
                    "ChangeGroupMember",
                    property_type::integer,
                    property_type::integer,
                    property_type::string,
                    property_type::string,
            );

            bind_service(server,
                    this,
                    &GroupManager::removeGroupMember,
                    "RemoveGroupMember",
                    property_type::integer,
                    property_type::integer,
                    property_type::string,
            );

        }
#endif

        // Add by HyunJun.
        // Print to screen about member of group for debug.
        void printGroup(GID groupId)
        {
            Group* group = NULL;

            group = getGroup(groupId);

            cout << "\nGROUP ID : " << groupId << endl;
            cout << "====================================================================" << endl;
            for (unsigned int i = 0; i < group->m_listMandatoryDevices.size(); ++i)
            {
                group->m_listMandatoryDevices.at(i)->printDevice();
            }
            cout << "====================================================================" << endl;
        }
    };
}

#endif // __OC_GROUP_MANAGER__
