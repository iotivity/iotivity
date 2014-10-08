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


/// @file    TGMClient.cpp
///  @brief   
 
#include <OCApi.h>
#include <OCPlatform.h>

#include <cstdlib>

#include "TGMClient.h"
#include "actionjson.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

using namespace OC;

std::mutex TGMClient::s_mutex;
ITGMClientListener* TGMClient::s_tgmClientListener;
std::shared_ptr< OCResource > TGMClient::s_tgmResource = NULL;
OCPlatform* g_platform;
TGMClient* g_client;

CallInfo g_callInfo;
CallInfo g_findDeviceCallInfo;

TGMClient::TGMClient(void)
{
    _findResource();
}

/**
 * Virtual destructor
 */
TGMClient::~TGMClient(void)
{
    if (s_tgmResource != NULL)
    {
        s_tgmResource->cancelObserve();
    }
}


TGM_RESULT TGMClient::bindListener(ITGMClientListener* listener)
{
    s_tgmClientListener = listener;
    return TGM_SUCCESS;
}

void TGMClient::actionPutCallbackFromRealData(const OCRepresentation& rep, const int eCode)
{
    //ExecuteActionSet();

}

void TGMClient::onObserveForDeviceFound(const OCRepresentation rep, const int& eCode,
        const int& sequenceNumber)
{
    //static int callbackFlag = 0;

    if (eCode == SUCCESS_RESPONSE)
    {
        //std::cout << "ActionPutCallback >> " << std::endl;
        //std::string outStr;

        AttributeMap attributeMap = rep.getAttributeMap();
        std::string yesStr = g_findDeviceCallInfo.m_sessionId;
        std::string noStr = g_findDeviceCallInfo.m_sessionId;

        yesStr.append("|YES");
        noStr.append("|NO");
        auto yesCheck = attributeMap.find(yesStr);
        auto noCheck = attributeMap.find(noStr);

        if (yesCheck == attributeMap.end() && noCheck == attributeMap.end())
        {
            //std::cout << "NO DATA OBSERVE CALLBACK \n\n";
            return;
        }
        else if (yesCheck == attributeMap.end())
        {
            std::cout << g_findDeviceCallInfo.m_sessionId << " : MANDATORY IS NOT ENOUGH \n\n";
            return;
        }

        std::cout << "OnObserveForCallback" << std::endl;
        std::cout << "OBSERVE RESULT:" << std::endl;
        std::cout << "\tSequenceNumber: " << sequenceNumber << endl;
        std::string deviceString;
        for (auto it = attributeMap.begin(); it != attributeMap.end(); ++it)
        {

            std::cout << "\tAttribute" << std::endl;
            std::cout << "\t name : " << it->first << " --- value: ";

            for (auto valueItr = it->second.begin(); valueItr != it->second.end(); ++valueItr)
            {
                std::cout << "\t" << *valueItr << " ";
                //ss << "\t"<< *valueItr << " ";
                if (it->first.compare(yesStr) == 0)
                {
                    //this is mandatory devices
                    deviceString = *valueItr;

                }
            }

            std::cout << std::endl;
            // ss << std::endl;
        }

        /* MAKE ACTION SET FROM RECEIVED DATA*/
        QueryParamsMap qp;
        if (deviceString != "")
        {
            
            std::vector< OC::Device* > deviceList;
            JsonUnpackDeviceList(deviceString.c_str(), deviceList);

            std::cout << " DEVICE LIST IS COMMING!!!!!!" << std::endl;

            //ActionSet* actionset = new ActionSet();

            for (unsigned int i = 0; i < deviceList.size(); ++i)
            {
                deviceList.at(i)->printDevice();
            }

            //for Test
            //CreateActionSetFromRealData(actionset);
            SESSION_ID recv_SessionID = g_findDeviceCallInfo.m_sessionId;
            s_tgmClientListener->onFindDevice(recv_SessionID, deviceList, deviceList);
        }

        //m_TGMResource->get(qp,&onGetFromObserve);

    }
    else
    {
        std::cout << "onPut Response error: " << eCode << std::endl;
        std::exit(-1);
    }

}

SESSION_ID TGMClient::findDevice(std::vector< DEVICE_TYPE > mandatory,
        std::vector< DEVICE_TYPE > optional)
{
    static bool observeFlag = false;

    //observe for device found
    if (mandatory.size() == 0)
    {
        return SESSION_CREATION_ERROR;
    }
    if (observeFlag == false)
    {
        QueryParamsMap qp;
        observeFlag = true;
        s_tgmResource->observe(OC::ObserveType::Observe, qp, &onObserveForDeviceFound);
    }
    AttributeMap attributeMap;
    AttributeValues attr_method;

    attr_method.push_back("FindGroupMemberCandidatesByResourceType");
    attributeMap["method"] = attr_method;

    SESSION_ID sessionid = NEW_SESSION_ID;
    AttributeValues attr_sessionID;
    attr_sessionID.push_back(sessionid);
    attributeMap["sessionid"] = attr_sessionID;

    std::vector< DEVICE_TYPE >::iterator it;
    AttributeValues mandatories;
    std::string mandatory_devices = "";
    for (it = mandatory.begin(); it != mandatory.end(); ++it)
    {
        //TODO:using this after bug fix
        //mandatories.push_back(std::string(szDeviceTypes[(*it)]));

        //for bug
        if (it != mandatory.begin())
        {
            mandatory_devices.append("|");
        }

        mandatory_devices.append(std::string(s_deviceTypes[(*it)]));
    }
    mandatories.push_back(mandatory_devices);
    attributeMap["mandatory"] = mandatories;

    AttributeValues options;
//    mandatories.push_back(std::string("core.tv"));
    for (it = optional.begin(); it != optional.end(); ++it)
    {
        options.push_back(std::string(s_deviceTypes[(*it)]));
    }
    if (optional.size() == 0)
    {
        options.push_back(std::string(""));
    }

    attributeMap["optional"] = options;

    QueryParamsMap queryParamsMap;
    OCRepresentation rep;
    rep.setAttributeMap(attributeMap);
    if (s_tgmResource != NULL)
    {
        if (s_tgmResource->put(rep, queryParamsMap, &onPut) == 0)
        {
            g_findDeviceCallInfo.m_sessionId = sessionid;
            g_findDeviceCallInfo.m_caller = __FUNCTION__;

            return sessionid;
        }
    }
    return SESSION_CREATION_ERROR;
}


SESSION_ID TGMClient::createGroup(std::string groupName)
{

    // if (mandatoryDeviceList.size() < 1)
    // {
    //     return SESSION_CREATION_ERROR;
    // }
    // std::vector< Device* > mandatoryDeviceListOne;
    // std::vector< Device* > mandatoryDeviceListOther;

    // for (unsigned int i = 0 ; i < mandatoryDeviceList.size() ; ++i)
    // {
    //     if( i == 0 )
    //     {
    //         mandatoryDeviceListOne.push_back(mandatoryDeviceList.at(i));
    //     }
    //     else
    //     {
    //         mandatoryDeviceListOther.push_back(mandatoryDeviceList.at(i));
    //     }
    // }

    // mandatoryDeviceList = mandatoryDeviceListOne;

    // std::vector< Device* >::iterator it;

    AttributeMap attributeMap;
    AttributeValues attr_method;    

    attr_method.push_back("CreateGroup");
    attributeMap["method"] = attr_method;

    SESSION_ID sessionid = NEW_SESSION_ID;
    AttributeValues attr_sessionID;
    attr_sessionID.push_back(sessionid);
    attributeMap["sessionid"] = attr_sessionID;

    AttributeValues attr_groupName;
    attr_groupName.push_back(groupName);
    attributeMap["groupname"] = attr_groupName;

    boost::uuids::uuid uuid;
    try
    {
        uuid = boost::uuids::random_generator()();
    }
    catch (boost::exception &e)
    {
        std::cout << "UUID Generation Error" << std::endl;
        return SESSION_CREATION_ERROR;
    }
    AttributeValues attr_groupID;
    attr_groupID.push_back(boost::uuids::to_string(uuid));
    attributeMap["guid"] = attr_groupID;

    // AttributeValues attr_mandatory;
    /*
    for (it = mandatoryDeviceList.begin(); it != mandatoryDeviceList.end(); ++it)
    {
        std::string temp = escapeJsonString(std::string(JsonPackDevice((*it))));
        attr_mandatory.push_back(temp);
    }
    &*/
    // std::string deviceListStr = escapeJsonString(std::string(JsonPackDeviceListReturn(mandatoryDeviceList) ) );
    // attr_mandatory.push_back(deviceListStr);
    // attributeMap["mandatory"] = attr_mandatory;


    // AttributeValues optional;
    /*
    for (it = optionalDeviceList.begin(); it != optionalDeviceList.end(); ++it)
    {
        std::string temp = escapeJsonString(std::string(JsonPackDevice((*it))));
        cout << "dump : " << temp << endl;
        optional.push_back(temp);
    }
    */
    
    //std::string optDeviceListStr = escapeJsonString(std::string(JsonPackDeviceListReturn(optionalDeviceList) ) );
    //optional.push_back(optDeviceListStr);
    // optional.push_back(std::string(""));
    // attributeMap["optional"] = optional;

    QueryParamsMap queryParamsMap;
    OCRepresentation rep;
    rep.setAttributeMap(attributeMap);
    if (s_tgmResource != NULL)
    {
        if (s_tgmResource->put(rep, queryParamsMap, &onPut) == 0)
        {
            g_callInfo.m_sessionId = sessionid;
            g_callInfo.m_caller = __FUNCTION__;

            //for M1, payload length
            // sleep(1);
            // attributeMap["method"][0] = "AddGroupMember" ;
            // g_callInfo.m_caller = "addGroupMember";
            // for(unsigned int i = 0 ; i < mandatoryDeviceListOther.size() ; ++i)
            // {
            //     std::vector< Device* > mandatoryDeviceListTemp;
            //     mandatoryDeviceListTemp.push_back(mandatoryDeviceListOther.at(i));

            //     AttributeValues attr_mandatory;
            //     std::string deviceListStr = escapeJsonString(std::string(JsonPackDeviceListReturn(mandatoryDeviceListTemp) ) );
            //     attr_mandatory.push_back(deviceListStr);
            //     attributeMap["mandatory"] = attr_mandatory;
            //     rep.setAttributeMap(attributeMap);
            //     s_tgmResource->put(rep, queryParamsMap, &onPut);
            // }

            return sessionid;
        }
    }
    return SESSION_CREATION_ERROR;
}


SESSION_ID TGMClient::addGroupMember(GROUP_ID groupId, std::vector< Device* > mandatoryDeviceList,
        std::vector< Device* > optionalDeviceList)
{
    AttributeMap attributeMap;
    AttributeValues method;

    if (mandatoryDeviceList.size() < 1)
    {
        return SESSION_CREATION_ERROR;
    }

    std::vector< Device* >::iterator it;

    method.push_back("AddGroupMember");
    attributeMap["method"] = method;

    SESSION_ID sessionid = NEW_SESSION_ID;
    AttributeValues attr_sessionID;
    attr_sessionID.push_back(sessionid);
    attributeMap["sessionid"] = attr_sessionID;

    AttributeValues gID;
    gID.push_back(groupId);
    attributeMap["guid"] = gID;

    AttributeValues mandatory;
    /*
    for (it = mandatoryDeviceList.begin(); it != mandatoryDeviceList.end(); ++it)
    {
        mandatory.push_back(escapeJsonString(std::string(JsonPackDevice((*it)))));
    }
    */
    mandatory.push_back(escapeJsonString(std::string(JsonPackDeviceListReturn(mandatoryDeviceList))));
    attributeMap["mandatory"] = mandatory;


    AttributeValues optional;
    /*
    for (it = optionalDeviceList.begin(); it != optionalDeviceList.end(); ++it)
    {
        optional.push_back(escapeJsonString(std::string(JsonPackDevice((*it)))));
    }
    */
    optional.push_back(escapeJsonString(std::string(JsonPackDeviceListReturn(optionalDeviceList))));
    //TODO: To be deleted.
    optional.push_back("");
    attributeMap["optional"] = optional;

    QueryParamsMap queryParamsMap;
    OCRepresentation rep;
    rep.setAttributeMap(attributeMap);
    if (s_tgmResource != NULL)
    {
        if (s_tgmResource->put(rep, queryParamsMap, &onPut) == 0)
        {
            g_callInfo.m_sessionId = sessionid;
            g_callInfo.m_caller = __FUNCTION__;

            return sessionid;
        }
    }
    return SESSION_CREATION_ERROR;
}


SESSION_ID TGMClient::deleteGroupMember(GROUP_ID groupId, DEVICE_ID deviceId)
{

    AttributeMap attributeMap;
    AttributeValues method;

    if (deviceId.compare("") == 0)
    {
        return SESSION_CREATION_ERROR;
    }
    if (groupId.compare("") == 0)
    {
        return SESSION_CREATION_ERROR;
    }

    std::vector< Device* >::iterator it;

    method.push_back("DeleteGroupMember");
    attributeMap["method"] = method;

    SESSION_ID sessionid = NEW_SESSION_ID;
    AttributeValues attr_sessionID;
    attr_sessionID.push_back(NEW_SESSION_ID);
    attributeMap["sessionid"] = attr_sessionID;

    AttributeValues gID;
    gID.push_back(groupId);
    attributeMap["guid"] = gID;

    AttributeValues deviceid;
    deviceid.push_back(deviceId);
    attributeMap["duid"] = deviceid;

    QueryParamsMap queryParamsMap;
    OCRepresentation rep;
    rep.setAttributeMap(attributeMap);
    if (s_tgmResource != NULL)
    {
        if (s_tgmResource->put(rep, queryParamsMap, &onPut) == 0)
        {
            g_callInfo.m_sessionId = sessionid;
            g_callInfo.m_caller = __FUNCTION__;

            return sessionid;
        }
    }
    return SESSION_CREATION_ERROR;

}


SESSION_ID TGMClient::removeGroup(GROUP_ID groupId)
{

    AttributeMap attributeMap;
    AttributeValues method;
    if (groupId == "")
    {
        return SESSION_CREATION_ERROR;
    }
    std::vector< Device* >::iterator it;

    method.push_back("RemoveGroup");
    attributeMap["method"] = method;

    SESSION_ID sessionid = NEW_SESSION_ID;
    AttributeValues attr_sessionID;
    attr_sessionID.push_back(NEW_SESSION_ID);
    attributeMap["sessionid"] = attr_sessionID;

    AttributeValues gID;
    gID.push_back(groupId);
    attributeMap["guid"] = gID;

    QueryParamsMap queryParamsMap;
    OCRepresentation rep;
    rep.setAttributeMap(attributeMap);
    if (s_tgmResource != NULL)
    {
        if (s_tgmResource->put(rep, queryParamsMap, &onPut) == 0)
        {
            g_callInfo.m_sessionId = sessionid;
            g_callInfo.m_caller = __FUNCTION__;

            return sessionid;
        }
    }
    return SESSION_CREATION_ERROR;
}

SESSION_ID TGMClient::getGroup(GROUP_ID groupId)
{

    AttributeMap attributeMap;
    AttributeValues method;
    if (groupId == "")
    {
        return SESSION_CREATION_ERROR;
    }

    std::vector< Device* >::iterator it;

    method.push_back("GetGroup");
    attributeMap["method"] = method;

    SESSION_ID sessionid = NEW_SESSION_ID;
    AttributeValues attr_sessionID;
    attr_sessionID.push_back(NEW_SESSION_ID);
    attributeMap["sessionid"] = attr_sessionID;

    AttributeValues gID;
    gID.push_back(groupId);
    attributeMap["guid"] = gID;

    QueryParamsMap queryParamsMap;
    OCRepresentation rep;
    rep.setAttributeMap(attributeMap);
    if (s_tgmResource != NULL)
    {
        if (s_tgmResource->put(rep, queryParamsMap, &onPut) == 0)
        {
            g_callInfo.m_sessionId = sessionid;
            g_callInfo.m_caller = __FUNCTION__;

            return sessionid;
        }
    }
    return SESSION_CREATION_ERROR;

}

SESSION_ID TGMClient::getDevice(DEVICE_ID deviceId)
{

    AttributeMap attributeMap;
    AttributeValues method;
    if (deviceId == "")
    {
        return SESSION_CREATION_ERROR;
    }
    std::vector< Device* >::iterator it;

    method.push_back("GetDevice");
    attributeMap["method"] = method;

    SESSION_ID sessionid = NEW_SESSION_ID;
    AttributeValues attr_sessionID;
    attr_sessionID.push_back(NEW_SESSION_ID);
    attributeMap["sessionid"] = attr_sessionID;

    AttributeValues dID;
    dID.push_back(deviceId);
    attributeMap["duid"] = dID;

    QueryParamsMap queryParamsMap;
    OCRepresentation rep;
    rep.setAttributeMap(attributeMap);
    if (s_tgmResource != NULL)
    {
        if (s_tgmResource->put(rep, queryParamsMap, &onPut) == 0)
        {
            g_callInfo.m_sessionId = sessionid;
            g_callInfo.m_caller = __FUNCTION__;

            return sessionid;
        }
    }
    return SESSION_CREATION_ERROR;
}

SESSION_ID TGMClient::getActionSet(GROUP_ID groupId, ACTIONSET_ID actionSetId)
{

    AttributeMap attributeMap;
    AttributeValues method;
    if (groupId == "")
    {
        return SESSION_CREATION_ERROR;
    }
    std::vector< Device* >::iterator it;

    method.push_back("ReadActionSet");
    attributeMap["method"] = method;

    SESSION_ID sessionid = NEW_SESSION_ID;
    AttributeValues attr_sessionID;
    attr_sessionID.push_back(sessionid);
    attributeMap["sessionid"] = attr_sessionID;

    AttributeValues attr_groupid;
    attr_groupid.push_back(groupId);
    attributeMap["guid"] = attr_groupid;

    AttributeValues ActionSetId;
    ActionSetId.push_back(to_string(actionSetId));
    attributeMap["asid"] = ActionSetId;

    QueryParamsMap queryParamsMap;
    OCRepresentation rep;
    rep.setAttributeMap(attributeMap);

    if (s_tgmResource != NULL)
    {
        if (s_tgmResource->put(rep, queryParamsMap, &onPut) == 0)
        {
            g_callInfo.m_sessionId = sessionid;
            g_callInfo.m_caller = __FUNCTION__;

            return sessionid;
        }
    }
    return SESSION_CREATION_ERROR;

}

SESSION_ID TGMClient::updateActionSet(GROUP_ID groupId, ActionSet *newActionSet,
        ACTIONSET_ID actionSetId)
{

    AttributeMap attributeMap;
    AttributeValues method;

    if (groupId == "")
    {
        return SESSION_CREATION_ERROR;
    }
    if (newActionSet == NULL || newActionSet->m_listActions.size() == 0)
    {
        return SESSION_CREATION_ERROR;
    }

    method.push_back("UpdateActionSet");
    attributeMap["method"] = method;

    SESSION_ID sessionid = NEW_SESSION_ID;
    AttributeValues attr_sessionID;
    attr_sessionID.push_back(sessionid);
    attributeMap["sessionid"] = attr_sessionID;

    AttributeValues attr_guid;
    attr_guid.push_back(groupId);
    attributeMap["guid"] = attr_guid;

    AttributeValues attr_newactionset;
    attr_newactionset.push_back(escapeJsonString(JsonPackActionSet(newActionSet)));
    attributeMap["actionset"] = attr_newactionset;

    AttributeValues attr_actionsetid;
    attr_actionsetid.push_back(to_string(actionSetId));
    attributeMap["asid"] = attr_actionsetid;

    QueryParamsMap queryParamsMap;
    OCRepresentation rep;
    rep.setAttributeMap(attributeMap);
    if (s_tgmResource != NULL)
    {
        if (s_tgmResource->put(rep, queryParamsMap, &onPut) == 0)
        {
            g_callInfo.m_sessionId = sessionid;
            g_callInfo.m_caller = __FUNCTION__;

            return sessionid;
        }
    }
    return SESSION_CREATION_ERROR;

}

SESSION_ID TGMClient::createActionSet(GROUP_ID groupId, ActionSet* actionSet)
{

    AttributeMap attributeMap;
    AttributeValues method;
    if (groupId == "")
    {
        return SESSION_CREATION_ERROR;
    }
    if (actionSet == NULL || actionSet->m_listActions.size() == 0)
    {
        return SESSION_CREATION_ERROR;
    }
    std::vector< Device* >::iterator it;

    method.push_back("CreateActionSet");
    attributeMap["method"] = method;

    SESSION_ID sessionid = NEW_SESSION_ID;
    AttributeValues attr_sessionID;
    attr_sessionID.push_back(sessionid);
    attributeMap["sessionid"] = attr_sessionID;

    AttributeValues attr_groupID;
    attr_groupID.push_back(groupId);
    attributeMap["guid"] = attr_groupID;

    AttributeValues ActionSet;
    ActionSet.push_back(escapeJsonString(JsonPackActionSet(actionSet)));
    attributeMap["actionset"] = ActionSet;

    QueryParamsMap queryParamsMap;
    OCRepresentation rep;
    rep.setAttributeMap(attributeMap);
    if (s_tgmResource != NULL)
    {
        if (s_tgmResource->put(rep, queryParamsMap, &onPut) == 0)
        {
            g_callInfo.m_sessionId = sessionid;
            g_callInfo.m_caller = __FUNCTION__;

            return sessionid;
        }
    }
    return SESSION_CREATION_ERROR;

}

SESSION_ID TGMClient::deleteActionSet(GROUP_ID groupId, ACTIONSET_ID actionSetId)
{

    AttributeMap attributeMap;
    AttributeValues method;
    if (groupId == "")
    {
        return SESSION_CREATION_ERROR;
    }
    std::vector< Device* >::iterator it;

    method.push_back("DeleteActionSet");
    attributeMap["method"] = method;

    SESSION_ID sessionid = NEW_SESSION_ID;
    AttributeValues attr_sessionID;
    attr_sessionID.push_back(sessionid);
    attributeMap["sessionid"] = attr_sessionID;

    AttributeValues attr_groupID;
    attr_groupID.push_back(groupId);
    attributeMap["guid"] = attr_groupID;

    AttributeValues actionsetid;
    actionsetid.push_back(to_string(actionSetId));
    attributeMap["asid"] = actionsetid;

    QueryParamsMap queryParamsMap;
    OCRepresentation rep;
    rep.setAttributeMap(attributeMap);
    if (s_tgmResource != NULL)
    {
        if (s_tgmResource->put(rep, queryParamsMap, &onPut) == 0)
        {
            g_callInfo.m_sessionId = sessionid;
            g_callInfo.m_caller = __FUNCTION__;

            return sessionid;
        }
    }
    return SESSION_CREATION_ERROR;

}

SESSION_ID TGMClient::executeActionSet(GROUP_ID groupId, ACTIONSET_ID actionSetId)
{

    AttributeMap attributeMap;
    AttributeValues method;
    if (groupId == "")
    {
        return SESSION_CREATION_ERROR;
    }
    std::vector< Device* >::iterator it;

    method.push_back("ExecuteActionSet");
    attributeMap["method"] = method;

    SESSION_ID sessionid = NEW_SESSION_ID;
    AttributeValues attr_sessionID;
    attr_sessionID.push_back(sessionid);
    attributeMap["sessionid"] = attr_sessionID;

    AttributeValues groupID;
    groupID.push_back(groupId);
    attributeMap["guid"] = groupID;

    AttributeValues ActionSet;
    ActionSet.push_back(to_string(actionSetId));
    attributeMap["asid"] = ActionSet;

    QueryParamsMap queryParamsMap;
    OCRepresentation rep;
    rep.setAttributeMap(attributeMap);
    if (s_tgmResource != NULL)
    {
        if (s_tgmResource->put(rep, queryParamsMap, &onPut) == 0)
        {
            g_callInfo.m_sessionId = sessionid;
            g_callInfo.m_caller = __FUNCTION__;

            return sessionid;
        }
    }
    return SESSION_CREATION_ERROR;

}

SESSION_ID TGMClient::executeActionSet(std::string desc)
{

    AttributeMap attributeMap;
    AttributeValues method;
    if (desc == "")
    {
        return SESSION_CREATION_ERROR;
    }
    std::vector< Device* >::iterator it;

    method.push_back("ExecuteActionSetByDescription");
    attributeMap["method"] = method;

    SESSION_ID ssid = NEW_SESSION_ID;
    AttributeValues attr_sessionID;
    attr_sessionID.push_back(ssid);
    attributeMap["sessionid"] = attr_sessionID;

    AttributeValues description;
    description.push_back(desc);
    attributeMap["description"] = description;

    QueryParamsMap queryParamsMap;
    OCRepresentation rep;
    rep.setAttributeMap(attributeMap);
    if (s_tgmResource != NULL)
    {
        if (s_tgmResource->put(rep, queryParamsMap, &onPut) == 0)
        {
            g_callInfo.m_sessionId = ssid;
            g_callInfo.m_caller = __FUNCTION__;

            return g_callInfo.m_sessionId;
        }
    }
    return SESSION_CREATION_ERROR;

}

void TGMClient::onPut(const OCRepresentation& rep, const int& eCode)
{
    if (eCode == SUCCESS_RESPONSE)
    {
        AttributeMap attributeMap = rep.getAttributeMap();
        cout << "Before" << endl;

        for (auto it = attributeMap.begin(); it != attributeMap.end(); ++it)
                {
                    //Create Capability from attribute
                    std::cout << "\tAttribute name: " << it->first << " value: ";
                    for (auto valueItr = it->second.begin(); valueItr != it->second.end();
                            ++valueItr)
                    {
                        std::cout << "\t" << *valueItr << " ";
                    }
                    std::cout << std::endl;
                }




        SESSION_ID recv_SessionID = attributeMap["sessionid"][0];
        cout << "After" << endl;

        //   cout << gcallinfo.sessionID << endl;
        //   cout << recv_SessionID << endl;
        if (g_callInfo.m_sessionId == recv_SessionID)
        {

            cout << g_callInfo.m_caller << endl;
            if (g_callInfo.m_caller == "createGroup" || g_callInfo.m_caller == "getGroup")
            {
                std::string groupid;
                OC::GroupInfo* groupinfo;

                groupid = attributeMap["guid"][0];
                groupinfo = JsonUnpackGroup(attributeMap["groupinfo"][0].c_str());

                s_tgmClientListener->onGroup(recv_SessionID, groupinfo);
            }
            else if (g_callInfo.m_caller == "removeGroup")
            {

                // TBD
            }
            else if (g_callInfo.m_caller == "addGroupMember")
            { // coap://10.0.2.15:54147/a/bookmark
                std::string groupid;
                std::string device_state;

                DEVICE_STATE state;

                groupid = attributeMap["guid"][0];
                device_state = attributeMap["result"][0];

                if (device_state == "DEVICE_ADDED")
                    state = DEVICE_STATE::DEVICE_ADDED;
                else
                    state = DEVICE_STATE::DEVICE_UPDATED;

                s_tgmClientListener->onGroupStateChanged(recv_SessionID, groupid, NULL, state);
            }
            else if (g_callInfo.m_caller == "deleteGroupMember")
            {
                std::string groupid;
                std::string device_state;

                DEVICE_STATE state;

                groupid = attributeMap["guid"][0];
                device_state = attributeMap["result"][0];

                if (device_state == "DEVICE_REMOVED")
                    state = DEVICE_STATE::DEVICE_REMOVED;
                else
                    state = DEVICE_STATE::DEVICE_UPDATED;

                s_tgmClientListener->onGroupStateChanged(recv_SessionID, groupid, NULL, state);
            }
            else if (g_callInfo.m_caller == "createActionSet" || g_callInfo.m_caller == "updateActionSet")

            {
                std::string groupid;
                unsigned int asid;
                std::string result;

                groupid = attributeMap["guid"][0];
                asid = atoi(attributeMap["asid"][0].c_str());
                result = attributeMap["result"][0];

                s_tgmClientListener->onActionSetOperation(recv_SessionID, groupid, asid,
                        (result == "true" ? TGM_SUCCESS : TGM_ERROR));

            }
            else if (g_callInfo.m_caller == "deleteActionSet")
            {
                std::string groupid;
                unsigned int asid;
                std::string result;

                groupid = attributeMap["guid"][0];
                asid = atoi(attributeMap["asid"][0].c_str());
                result = attributeMap["result"][0];

                s_tgmClientListener->onActionSetOperation(recv_SessionID, groupid, asid,
                        (result == "true" ? TGM_SUCCESS : TGM_ERROR));

            }
            else if (g_callInfo.m_caller == "executeActionSet")
            {
                std::string groupid;
                unsigned int asid;
                std::string result;

                if(attributeMap.find("description") != attributeMap.end())
                {
                    std::string description = attributeMap["description"][0];
                    result = attributeMap["result"][0];
                    s_tgmClientListener->onActionSetOperation(recv_SessionID, description, -1,
                            (result == "true" ? TGM_SUCCESS : TGM_ERROR));
                }
                else
                {
                    groupid = attributeMap["guid"][0];
                    asid = atoi(attributeMap["asid"][0].c_str());
                    result = attributeMap["result"][0];
                    s_tgmClientListener->onActionSetOperation(recv_SessionID, groupid, asid,
                            (result == "true" ? TGM_SUCCESS : TGM_ERROR));
                }
                
            }
            else if (g_callInfo.m_caller == "getActionSet")
            {
                std::string groupid;
                unsigned int asid;
                std::string result;

                OC::ActionSet* actionset;

                groupid = attributeMap["guid"][0];
                asid = atoi(attributeMap["asid"][0].c_str());
                result = attributeMap["result"][0];

                actionset = JsonUnpackActionSet(attributeMap["actionset"][0].c_str());

                s_tgmClientListener->onGetActionSet(recv_SessionID, groupid, asid, actionset,
                        (result == "true" ? TGM_SUCCESS : TGM_ERROR));
            }
        }
    }
}

OCPlatform* TGMClient::getPlatform()
{
    return g_platform;
}

void TGMClient::_findResource()
{
    g_client = this;

    std::cout << "[Debug]:" << __func__ << ": Called." << std::endl;

    PlatformConfig cfg(
        OC::ServiceType::InProc,
        OC::ModeType::Client,
        "192.168.1.10",
        5683,
        OC::QualityOfService::NonConfirmable );

    //cfg.ipAddress = "192.168.1.10";
    //cfg.port = 5683;
    //cfg.mode = ModeType::Client;
    //cfg.serviceType = ServiceType::InProc;

    try
    {
        g_platform = new OCPlatform(cfg);
        g_platform->findResource("", "coap://224.0.1.187/oc/core?rt=core.groupmanager",
                &onFoundResource);

    }
    catch (OCException &e)
    {
        std::cerr << "Resource is invalid." << std::endl;
    }
}

void TGMClient::onFoundResource(std::shared_ptr< OCResource > resource)
{
    cout << __FUNCTION__ << " Called(" << __LINE__ << ").\n" << endl;

    std::string resourceURI;
    std::string hostAddress;

    try
    {
        if (resource)
        {
            // Get the resource URI
            resourceURI = resource->uri();

            // Get the resource host address
            hostAddress = resource->host();
            //std::cout<<"\n\n ON FOUND RESOURCE" << resourceURI <<" \n\n\n\n" ;
            if (resourceURI.compare("/a/groupmanager") == 0)
            {
                s_tgmResource = resource;
                s_tgmClientListener->onCreated();
            }
        }
    }
    catch (std::exception& e)
    {

    }
}
