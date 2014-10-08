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

/// @file   TGMClient.h

/// @brief  This file contains the declaration of classes and its members related to TGMClient.

#ifndef __OC_TGMCLIENT__
#define __OC_TGMCLIENT__

#include <string>
#include <memory>
#include <mutex>

#include "OCResource.h"
#include "OCPlatform.h"
#include "OCRepresentation.h"

#include "ITGMClientListener.h"
#include "GroupManager.h"

#include "time.h"

#define TGM_RESOURCE_URI 		"/a/groupmanager"
#define TGM_QUERY 				"coap:://224.0.1.187/oc/core?rt=core.groupmanager"

/*
 #define LOG \
{ \
	std::cout << "[Debug]:" << __func__ <<": Called." << std::endl \
}
 */

const int SUCCESS_RESPONSE = 0;

#define NEW_SESSION_ID to_string(clock())

/** @defgroup ThingsGraph Things Graph Manager
 * This is Things Graph Manager group
 */

using namespace OC;

typedef struct
{
    SESSION_ID m_sessionId;
    std::string m_caller;
} CallInfo;

/**
 *	@brief	TGMClient is a wrapper class to provide Things Graph Manager functionality to Application.
 *			TGMClient abstracts the resource based operations from client applications and provides c++ style functions.
 */
class TGMClient
{
public:
    /**
     * Constructor for TGMClient. Constructs a new TGMClient
     */
    TGMClient(void);

    /**
     * Virtual destructor
     */
    ~TGMClient(void);

    /**
     * API for connecting listener to application
     * @param [in] listener - pointer of listener.
     * @return  TGM_RESULT - return value of this API. Returns TGM_SUCCESS if success. NOTE: TGM_RESULT is defined in ITGMClientListener.h
     */
    TGM_RESULT bindListener(ITGMClientListener* listener);

    /**
     * API for finding remote devices as candidate of member about group specified by application. This API finds remote devices by device type as criteria.
     *
     * @param [in] mandatory - required device type for mandatory devices. NOTE: If mandatory device does not exist, group can not be established.
     * @param [in] optional - required device type for optional devices. NOTE: Even optional device does not exist, group can be established. For optional devices, presence check of devices does not performed.
     * @return  SESSION_ID - return value of this API. Returns Session ID for this specific call. Callback listener function in ITGMClientListener.h will provide same SESSION_ID to recognize which function related with that lister function. NOTE: SESSION_ID is defined in ITGMClientListener.h
     */
    SESSION_ID findDevice(std::vector< DEVICE_TYPE > mandatory,
            std::vector< DEVICE_TYPE > optional);
   
    /**
     * This API creates devices' group using list of devices selected by application.
     *
     * @param [in] groupName - name of group that application want to make.
     * @return  SESSION_ID - return value of this API. Returns Session ID for this specific call. Callback listener function in ITGMClientListener.h will provide same SESSION_ID to recognize which function related with that lister function. NOTE: SESSION_ID is defined in ITGMClientListener.h
     */
    SESSION_ID createGroup(std::string groupName);
 
    /**
     * API adds particular devices to group.
     *
     * @param [in] groupID - ID of group
     * @param [in] mandatoryDeviceList - list of mandatory devices selected by application tp add to this group.
     * @param [in] optionalDeviceList  - list of optional devices selected by application tp add to this group.
     * @return  SESSION_ID - return value of this API. Returns Session ID for this specific call. Callback listener function in ITGMClientListener.h will provide same SESSION_ID to recognize which function related with that lister function. NOTE: SESSION_ID is defined in ITGMClientListener.h
     */
    SESSION_ID addGroupMember(GROUP_ID groupID, std::vector< Device* > mandatoryDeviceList,
            std::vector< Device* > optionalDeviceList);
   
    /**
     * API deletes particular device from a group.
     *
     * @param [in] groupID - ID of group
     * @param [in] deviceID - ID of device selected by application to delete from this group.
     * @return  SESSION_ID - return value of this API. Returns Session ID for this specific call. Callback listener function in ITGMClientListener.h will provide same SESSION_ID to recognize which function related with that lister function. NOTE: SESSION_ID is defined in ITGMClientListener.h
     */
    SESSION_ID deleteGroupMember(GROUP_ID groupID, DEVICE_ID deviceID);
    
    /**
     * API removes specific group.
     *
     * @param [in] groupID - ID of group
     * @return  SESSION_ID - return value of this API. Returns Session ID for this specific call. Callback listener function in ITGMClientListener.h will provide same SESSION_ID to recognize which function related with that lister function. NOTE: SESSION_ID is defined in ITGMClientListener.h
     */
    SESSION_ID removeGroup(GROUP_ID groupID);
    
    /**
     * API to get Group object. It includes group ID, list of mandatory and optional devices and so on.
     *
     * @param [in] groupID - ID of group
     * @return  SESSION_ID - return value of this API. Returns Session ID for this specific call. Callback listener function in ITGMClientListener.h will provide same SESSION_ID to recognize which function related with that lister function. NOTE: SESSION_ID is defined in ITGMClientListener.h
     */
    SESSION_ID getGroup(GROUP_ID groupID);
    
    /**
     * API to get Group object. It includes group ID, list of mandatory and optional devices and so on.
     *
     * @param [in] deviceID - ID of device
     * @return  SESSION_ID - return value of this API. Returns Session ID for this specific call. Callback listener function in ITGMClientListener.h will provide same SESSION_ID to recognize which function related with that lister function. NOTE: SESSION_ID is defined in ITGMClientListener.h
     */
    SESSION_ID getDevice(DEVICE_ID deviceID);
    
    /**
     * API to read Action Set.
     *
     * @param [in] groupID - ID of group that contains Action Sets
     * @param [in] actionSetID - ID of action set
     * @return  SESSION_ID - return value of this API. Returns Session ID for this specific call. Callback listener function in ITGMClientListener.h will provide same SESSION_ID to recognize which function related with that lister function. NOTE: SESSION_ID is defined in ITGMClientListener.h
     */
    SESSION_ID getActionSet(GROUP_ID groupID, ACTIONSET_ID actionSetID);
    
    /**
     * API to update Action Set.
     *
     * @param [in] groupID - ID of group that contains Action Sets
     * @param [in] newActionSet - ActionSet Object to be replaced
     * @param [in] actionSetID - ID of action set needed to be replaced
     * @return  SESSION_ID - return value of this API. Returns Session ID for this specific call. Callback listener function in ITGMClientListener.h will provide same SESSION_ID to recognize which function related with that lister function. NOTE: SESSION_ID is defined in ITGMClientListener.h
     */
    SESSION_ID updateActionSet(GROUP_ID groupID, ActionSet *newActionSet, ACTIONSET_ID actionSetID);
    
    /**
     * API to create an Action Set. ID of action set will be returned.
     *
     * @param [in] groupID - ID of group that will contain this Action Set.
     * @param [in] actionSet - actionSet definition. description should be included to this Action Set. Example of description : "All bulb off"
     * @return  SESSION_ID - return value of this API. Returns Session ID for this specific call. Callback listener function in ITGMClientListener.h will provide same SESSION_ID to recognize which function related with that lister function. NOTE: SESSION_ID is defined in ITGMClientListener.h
     */
    SESSION_ID createActionSet(GROUP_ID groupID, ActionSet* actionSet);
    
    /**
     * API to delete a specific Action Set.
     *
     * @param [in] groupID - ID of group that contains this Action Set
     * @param [in] actionSetID - ID of action set
     * @return  SESSION_ID - return value of this API. Returns Session ID for this specific call. Callback listener function in ITGMClientListener.h will provide same SESSION_ID to recognize which function related with that lister function. NOTE: SESSION_ID is defined in ITGMClientListener.h
     */
    SESSION_ID deleteActionSet(GROUP_ID groupID, ACTIONSET_ID actionSetID);
    
    /**
     * API to execute a specific Action Set. onExecuteActionSet will be called after execution of the Action Set.
     *
     * @param [in] groupID - ID of group that contains this Action Set
     * @param [in] actionSetID - ID of action set
     * @return  SESSION_ID - return value of this API. Returns Session ID for this specific call. Callback listener function in ITGMClientListener.h will provide same SESSION_ID to recognize which function related with that lister function. NOTE: SESSION_ID is defined in ITGMClientListener.h
     */
    SESSION_ID executeActionSet(GROUP_ID groupID, ACTIONSET_ID actionSetID);
    
    /**
     * API to execute a specific Action Set. onExecuteActionSet will be called after execution of the Action Set.
     * User friendly description is used as input parameter of this API for user convenience.
     *
     * @param [in] desc - user friendly description for action trigger. Example of description : "All bulb off"
     * @return  SESSION_ID - return value of this API. Returns Session ID for this specific call. Callback listener function in ITGMClientListener.h will provide same SESSION_ID to recognize which function related with that lister function. NOTE: SESSION_ID is defined in ITGMClientListener.h
     */
    SESSION_ID executeActionSet(std::string desc);

    /**
     * API to open low level api to user.
     *
     * @return  OCPlatform* - return value of this API. Returns OCPlatform.
     */
    OCPlatform* getPlatform();

private:

    static std::mutex s_mutex;

    /**
     * @brief app listener
     */
    static ITGMClientListener *s_tgmClientListener;

    /**
     * @brief Things Graph Manager Resource.
     */
    static std::shared_ptr< OCResource > s_tgmResource;

    /**
     * @brief call session identification
     */
    SESSION_ID m_sessionId;

    /**
     * @brief internal find resource function
     */
    void _findResource();

    /**
     * @brief call when found the resource
     */
    static void onFoundResource(std::shared_ptr< OCResource > resource);

    static void onPut(const OCRepresentation& rep, const int& eCode);

    static void onObserveForDeviceFound(const OCRepresentation rep, const int& eCode,
            const int& sequenceNumber);

    static void actionPutCallbackFromRealData(const OCRepresentation& rep, const int eCode);


};

#endif  /* __OC_TGMCLIENT__*/
