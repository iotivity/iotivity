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

/// @file   ITGMClientListener.h

/// @brief  This file contains client listener interfaces.

#ifndef __OC_TGM_CLIENT_LISTENER__
#define __OC_TGM_CLIENT_LISTENER__


#include <string>

#include "OCPlatform.h"

#include "Device.h"
#include "Group.h"

using namespace OC;

/**
 * @brief This enum defines Common return types
 */
typedef unsigned int APP_ID;
typedef unsigned int RESULT;
typedef unsigned int ACTIONSET_ID;
typedef unsigned int ACTION_ID;

typedef std::string GROUP_ID;

typedef unsigned int TGM_RESULT;
const static TGM_RESULT TGM_SUCCESS = 100;
const static TGM_RESULT TGM_ERROR = 900;
const static TGM_RESULT TGM_ERROR_ONLY_ONE_MANDATORY_MEMBER = 901;

typedef std::string SESSION_ID;
const static SESSION_ID SESSION_CREATION_ERROR = "session creation error";

/** @ingroup ThingsGraph
 *   @brief ITGMClientListener
 */

/**
 *	@brief	ITGMClientListener is a listener interface from which application is derived to get callback from Things Graph Manager service
 */

class ITGMClientListener
{
public:

    /**
     * @brief	onCreated is a pure virtual operation which should be implemented in applications to get callback message about found ThingsGraphManager.
     */
    virtual void onCreated( ) = 0;

    /**
     * @brief	onFindDevice is a pure virtul operation which should be implemented in applications to get callback messages about found devices.
     * @param [in] sessionID - A session ID which used for recognizing identical operation in Things Graph Manager.
     * @param [in] mandatoryDeviceList - found mandatory devices' list.
     * @param [in] optionalDevice - found optional devices' list
     */
    virtual void onFindDevice(SESSION_ID sessionID, std::vector< Device* > mandatoryDeviceList,
            std::vector< Device* > optionalDevice) = 0;
    
    /**
     * @brief	onGroupStateChanged is a pure virtual operation which should be implemented in applications to get callback messages about group state changes.
     * @param [in] sessionID - A session ID which used for recognizing identical operation in Things Graph Manager.
     * @param [in] groupID - A group's ID which changed state.
     * @param [in] device - An object representation of device that changed state.
     * @param [in] state - device's final state	NOTE : DEVICE_STATE is defined in Device.h
     */
    virtual void onGroupStateChanged(SESSION_ID sessionID, GROUP_ID groupID, Device* device,
            DEVICE_STATE state) = 0;
    
    /**
     * @brief	onGroup is a pure virtual operation which should be implemented in applications to get callback messages about group state changes.
     * @param [in] sessionID - A session ID which used for recognizing identical operation in Things Graph Manager.
     * @param [in] groupinfo - A group's information.
     */
    virtual void onGroup(SESSION_ID sessionID, OC::GroupInfo* groupinfo) = 0;
    
    /**
     * @brief	onDevice is a pure virtual operation which should be implemented in applications to get callback messages about device state changes.
     * @param [in] sessionID -  A session ID which used for recognizing identical operation in Things Graph Manager.
     * @param [in] device - A Device's information.
     */
    virtual void onDevice(SESSION_ID sessionID, Device* device) = 0;
   
    /**
     * @brief	onGetActionSet is a pure virtual operation which should be implemented in applications to get callback messages about ActionSet related operations.
     * @param [in] sessionID - A session ID which used for recognizing identical operation in Things Graph Manager.
     * @param [in] groupID - A group's ID which needed operate action set.
     * @param [in] actionSetID - An action set ID which represents current action set that operation requested to Things Graph Manager.
     * @param [in] actionSet - A action set.
     * @param [in] result - result code from ActionSet related operations. TGM_RESULT is defined in ITGMClientListener.h
     */
    virtual void onGetActionSet(SESSION_ID sessionID, GROUP_ID groupID, ACTIONSET_ID actionSetID,
            ActionSet *actionSet, TGM_RESULT result) = 0;

    /**
     * @brief	onActionSetOperation is a pure virtual operation which should be implemented in applications to get callback messages about ActionSet related operations.
     * @param [in] sessionID - A session ID which used for recognizing identical operation in Things Graph Manager.
     * @param [in] groupID - A group's ID which needed operate action set.
     * @param [in] actionSetID - An action set ID which represents current action set that operation requested to Things Graph Manager.
     * @param [in] result - result code from ActionSet related operations. TGM_RESULT is defined in ITGMClientListener.h
     */
    virtual void onActionSetOperation(SESSION_ID sessionID, GROUP_ID groupID,
            ACTIONSET_ID actionSetID, TGM_RESULT result) = 0;
};

#endif
