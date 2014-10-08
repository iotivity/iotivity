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

/// @file GroupManager.cpp

/// @brief

#include <cstring>
#include <algorithm>
#include <stdexcept>
#include "../inc/GroupManager.h"

namespace OC
{

    using namespace std;

    string GroupManager::LOG_TAGS = "GROUP MANAGER ";

    GroupManager* GroupManager::s_instance = NULL;

    GroupManager* GroupManager::getInstance()
    {
        if (s_instance == NULL)
        {
            s_instance = new GroupManager();

            if (s_instance != NULL)
                return s_instance;
        }
        return s_instance;
    }


    int GroupManager::createGroupResource(string guid, string gname)
    {

        Group *pGroup = new Group;

        if (pGroup == NULL)
        {
            throw GraphException("Not Enough Memory");
            return false;
        }

        try
        {
            pGroup->m_guid = guid;
            pGroup->m_description = gname;
            pGroup->m_name = gname;

            m_listGroups.push_back(pGroup);
        }
        catch (const std::bad_alloc& e)
        {
            cerr << "Allocator bad alloc : " << e.what() << endl;
        }

        return m_listGroups.size() - 1;
    }

    int GroupManager::removeGroupResource(string guid)
    {
        for (auto it = m_listGroups.begin(); it != m_listGroups.end(); ++it)
        {
            if ((*it)->m_guid == guid)
            {
                m_listGroups.erase(it);
                break;
            }
        }
        return 1;
    }
/*
    int GroupManager::findGroupResourceMembers(DEVICE_TYPE[])
    {
        return false;
    }
*/
    Device* GroupManager::findDeviceInGroups(DEVICE_ID duid)
    {
        vector< Device* > devices;

        vector< Group* >::iterator groupIter;
        vector< Device* >::iterator deviceIter;

        for (groupIter = m_listGroups.begin(); groupIter != m_listGroups.end(); ++groupIter)
        {
            // find device from mandatory devices.
            devices = (*groupIter)->m_listMandatoryDevices;
            for (deviceIter = devices.begin(); deviceIter != devices.end(); ++deviceIter)
            {
                if ((*deviceIter)->m_duid == duid)
                {
                    return (*deviceIter);
                }
            }

            // find device from optional devices.
            devices = (*groupIter)->m_listOptionalDevices;
            for (deviceIter = devices.begin(); deviceIter != devices.end(); ++deviceIter)
            {
                if ((*deviceIter)->m_duid == duid)
                {
                    return (*deviceIter);
                }
            }
        }

        return NULL;
    }

    unsigned int GroupManager::getNumberofGroupMembers(GID gid)
    {
        return false;
    }

    unsigned int GroupManager::addGroupMember(GID guid, DEVICE_ID duid)
    {
        /*
        Device* device = s_presenceMgr->getFoundDevice(duid);

        if (device == NULL)
        {
            throw GraphDeviceNotFoundException("COULD NOT FOUND DEVICE.");
            return -1;
        }

        return addGroupMember(guid, device);
        */

        return -1;
    }

    unsigned int GroupManager::addGroupMember(GID guid, Device* device)
    {
        Group* group;
        try
        {
            group = getGroup(guid);
            group->m_listMandatoryDevices.push_back(device);

            map< UID, vector< GID > >::iterator it = m_mapGroupListForDevice.find(device->m_duid);
            if (it == m_mapGroupListForDevice.end())
            {
                vector< GID > listGID;
                listGID.push_back(guid);

                m_mapGroupListForDevice.insert(
                        make_pair< UID, vector< GID > >((UID) device->m_duid,
                                (vector< GID > ) listGID));
            }
            else
            {
                it->second.push_back(guid);
            }
        }
        catch (out_of_range& e)
        {
            cerr << "" << e.what() << endl;
        }
        catch (std::bad_alloc& e)
        {
            throw GraphException("Not Enough Memory.");
        }
        catch (GraphInternalServiceException& e)
        {
            cerr << LOG_TAGS << e.what() << endl;
            throw GraphInternalServiceException(e.what());
        }
        catch (GraphDeviceNotFoundException& e)
        {
            cerr << LOG_TAGS << e.what() << endl;
        }

        return group->m_listMandatoryDevices.size();
    }

    unsigned int GroupManager::addGroupMember(string guid, vector< Device* > mandatory,
            vector< Device* > optional)
    {

        Group* group;
        Device* device;
        vector< Device* >::iterator it;
        try
        {
            group = getGroup(guid);

            if (group == NULL)
                return 0;

            for (it = mandatory.begin(); it != mandatory.end(); ++it)
            {
                device = (*it);
                if (device == NULL)
                    break;

                group->m_listMandatoryDevices.push_back(device);

                map< UID, vector< GID > >::iterator it = m_mapGroupListForDevice.find(device->m_duid);
                if (it == m_mapGroupListForDevice.end())
                {
                    vector< GID > listGID;
                    listGID.push_back(guid);

                    m_mapGroupListForDevice.insert(
                            make_pair< UID, vector< GID > >((UID) device->m_duid,
                                    (vector< GID > ) listGID));
                }
                else
                {
                    it->second.push_back(guid);
                }
            }

            for (it = optional.begin(); it != optional.end(); ++it)
            {

                device = (*it);
                if (device == NULL)
                    break;

                group->m_listOptionalDevices.push_back(*it);

                map< UID, vector< GID > >::iterator it = m_mapGroupListForDevice.find(device->m_duid);
                if (it == m_mapGroupListForDevice.end())
                {
                    vector< GID > listGID;
                    listGID.push_back(guid);

                    m_mapGroupListForDevice.insert(
                            make_pair< UID, vector< GID > >((UID) device->m_duid,
                                    (vector< GID > ) listGID));
                }
                else
                {
                    it->second.push_back(guid);
                }
            }

        }
        catch (out_of_range& e)
        {
            cerr << "" << e.what() << endl;
        }
        catch (std::bad_alloc& e)
        {
            throw GraphException("Not Enough Memory.");
        }
        catch (GraphInternalServiceException& e)
        {
            cerr << LOG_TAGS << e.what() << endl;
            throw GraphInternalServiceException(e.what());
        }
        catch (GraphDeviceNotFoundException& e)
        {
            cerr << LOG_TAGS << e.what() << endl;
        }

        return 1;
    }

    unsigned int GroupManager::removeGroupMember(GID guid, DEVICE_ID duid)
    {
        try
        {
            vector< Device* > &devices = getGroup(guid)->m_listMandatoryDevices;
            vector< Device* >::iterator it;

            for (it = devices.begin(); it != devices.end(); it++)
            {
                //if((*it)->duid == device->duid)
                if (strcmp((*it)->m_duid.c_str(), duid.c_str()) == 0)
                { // if found the device then delete device from the group.
                    devices.erase(it);
                    break;
                }
            }

            //
            map< UID, vector< GID > >::iterator mapIt = m_mapGroupListForDevice.find(duid);
            if (mapIt != m_mapGroupListForDevice.end())
            {
                if (!mapIt->second.empty())
                {
                    vector< GID >::iterator gidIt;
                    for (gidIt = mapIt->second.begin(); gidIt != mapIt->second.end(); ++gidIt)
                    {
                        if (*gidIt == guid)
                        {
                            break;
                        }
                    }

                    mapIt->second.erase(gidIt);
                }
            }

        }
        catch (GraphInternalServiceException& e)
        {
            cerr << LOG_TAGS << e.what() << endl;
        }

        return false;
    }
/*
    unsigned int GroupManager::changeGroupMember(GID guid, DEVICE_ID olddev, DEVICE_ID newdev)
    {
        this->removeGroupMember(guid, olddev);
        this->addGroupMember(guid, newdev);

        return false;
    }
*/
/*
    Device* GroupManager::getGroupMember(UID duid)
    {
        Device* device = NULL;
        return device;
    }
*/
///////////////////////////////////////////////////////////////////////////////////////////
    /* ************************ Actuation Set CRUD+Execution ******************************* */

    ASID GroupManager::createActionSet(GID gid, ActionSet* pset)
    {
        Group* group;
        try
        {
            group = getGroup(gid);
        }
        catch (out_of_range& oor)
        {
            cerr << LOG_TAGS << " GROUPS OUT OF RANGE" << oor.what() << endl;
            ////m_coreCallback->onActuationSetResultResponse(-1, RESULT_CREATE_ACTUATIONSET_FAIL);
            throw GraphInternalServiceException();
        }
        catch (GraphInternalServiceException& e)
        {
            cerr << LOG_TAGS << " GROUPS OUT OF RANGE" << e.what() << endl;
            //pCoreCallback->OnActuationSetResultResponse(-1, RESULT_CREATE_ACTUATIONSET_FAIL);
            throw GraphInternalServiceException();
        }

        unsigned int id;
        pset->m_actuationSetId = group->getActionSet().size();
        id = group->addActionSet(pset) - 1;

        //pCoreCallback->OnActuationSetResultResponse(id, RESULT_CREATE_ACTUATIONSET_SUCCESS );
        cout << "ACTUATION SET ADDED" << endl;

        return id;
    }
    ///////////////////////////////////////// CREATE
    ActionSet* GroupManager::readActionSetFromDesc(std::string desc)
    {
        Group* group;
        ActionSet* actuationset;
        vector< Action* >::iterator it;

        try
        {
            vector< Group* >::iterator groupIt;

            for (groupIt = m_listGroups.begin(); groupIt != m_listGroups.end(); ++groupIt)
            {
                group = *groupIt;

                for (unsigned int i = 0; i < group->m_listActionSet.size(); ++i)
                {
                    actuationset = group->m_listActionSet.at(i);
                    if (actuationset->m_name.compare(desc) == 0)
                    {
                        cout << "ACTUATION SET FOUND." << endl;
                        return actuationset;
                    }
                } // TBD

            }
        }
        catch (out_of_range& oor)
        {
            cerr << "OUT OF RANGE EXCEPTION" << oor.what() << endl;
            //pCoreCallback->OnActuationSetExecuteResultResponse(gid, asid, RESULT_EXECUTE_ACTUATIONSET_FAIL);
            throw GraphInternalServiceException();
        }

        return NULL;
    }
    ActionSet* GroupManager::readActionSet(GID gid, ASID asid)
    {
        ActionSet *pSet = NULL;
        Group* group = NULL;
        try
        {
            // find Actuation Set using GID and ASID(Actuation Set ID)
            group = getGroup(gid);
            //pSet = group->GetActuationSet().at(asid);
            vector< ActionSet* > sets = group->getActionSet();
            vector< ActionSet* >::iterator it;
            for (it = sets.begin(); it != sets.end(); ++it)
            {
                if ((*it)->m_actuationSetId == asid)
                {
                    pSet = *it;
                }
            }

        }
        catch (GraphInternalServiceException& e)
        {
            //pCoreCallback->OnActuationSetReadResultResponse(NULL, RESULT_READ_ACTUATIONSET_FAIL);
            throw GraphInternalServiceException();
        }

        //pCoreCallback->OnActuationSetReadResultResponse(pSet, RESULT_READ_ACTUATIONSET_SUCCESS);
        cout << "ACTUATION SET FOUND." << endl;
        return pSet;
    }

    ///////////////////////////////////////////////READ

    bool GroupManager::updateActionSet(GID gid, ActionSet *pset)
    {
        Group *group = NULL;
        ASID asid;

        try
        {
            group = getGroup(gid);
            asid = pset->m_actuationSetId;

            // 1st step : DELETE
            deleteActionSet(gid, asid);

            // 2nd step : ADD ALL
            group->getActionSet().push_back(pset);

        }
        catch (out_of_range& oor)
        {
            cerr << "GROUPS OUT OF RANGE" << oor.what();
            //pCoreCallback->OnActuationSetUpdateResultResponse(gid,asid,RESULT_UPDATE_ACTUATIONSET_FAIL);
            throw GraphInternalServiceException();
        }

        //pCoreCallback->OnActuationSetUpdateResultResponse(gid, asid, RESULT_UPDATE_ACTUATIONSET_SUCCESS);
        cout << "ACTUATION SET UPDATE" << endl;

        return true;
    }

    ////////////////////////////////////////////UPDATE

    bool GroupManager::deleteActionSet(GID gid, ASID asid)
    {
        Group* group;
        vector< ActionSet* >::iterator it;

        try
        {
            group = getGroup(gid);

            it = group->findIteratorOfActionSet(asid);

            group->getActionSet().erase(it);
        }
        catch (out_of_range& oor)
        {
            cerr << "OUT OF RANGE EXCEPTION" << oor.what() << endl;
            //pCoreCallback->OnActuationSetUpdateResultResponse(gid, asid, RESULT_DELETE_ACTUATIONSET_FAIL);
            throw GraphInternalServiceException();
        }
        //pCoreCallback->OnActuationSetUpdateResultResponse(gid, asid, RESULT_DELETE_ACTUATIONSET_SUCCESS);

        cout << "ACTUATION SET DELETED" << endl;

        return true;
    }

    bool GroupManager::deleteAction(GID gid, ASID asid, AID aid)
    {
        try
        {
            // T. B. D
            //group = getGroup(gid);
            //actuationset = group->FindActuationSet(asid);
            //actuationset.actuations;
        }
        catch (out_of_range& oor)
        {
            cerr << "OUT OF RANGE" << oor.what() << endl;
            return false;
        }

        return true;
    }
    ///////////////////////////////////////// DELETE

    bool GroupManager::deleteAction(GID gid, string desc, string details)
    {
        return deleteAction(gid, atoi(desc.c_str()), atoi(details.c_str()));
    }

    /**
     *
     */

    GID GroupManager::findRelatedGroup(APPID appId)
    {
        map< APPID, GID >::iterator it = m_mapGroupIdForApp.find(appId);

        if (it != m_mapGroupIdForApp.end())
        {
            return it->second;
        }

        throw GraphInternalServiceException("APPLICATION NOT FOUND");
    }
}

