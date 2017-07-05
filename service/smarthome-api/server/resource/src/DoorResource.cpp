/* ****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/
#include <DoorResource.h>
#include <PropertyBundle.h>
#include <ResourceQuery.h>
#include "logger.h"

#define TAG "SH_SERVER_DOOR_RESOURCE"

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            const static std::string DOOR_URI = "/door";
            const static std::string OPEN_STATE_PROP_KEY = "openState";
            const static std::string OPEN_DURATION_PROP_KEY = "openDuration";
            const static std::string OPEN_ALARM_PROP_KEY = "openAlarm";

            namespace OpenState
            {
                const std::string Open = "Open";
                const std::string Closed = "Closed";
            };

            DoorResource::DoorResource() :
                    m_delegate(NULL), SHBaseResource(DOOR_URI,
                            RESOURCE_TYPE::DOOR)
            {
                bool ret = false;
                PropertyBundle bundle;

                // Set string property for openState.
                std::string openState = OpenState::Closed;
                bundle.setValue(OPEN_STATE_PROP_KEY, openState);

                // Set string property for openDuration.
                std::string openDuration;
                bundle.setValue(OPEN_DURATION_PROP_KEY, openDuration);

                // Set bool property for openAlarm.
                bool openAlarm = false;
                bundle.setValue(OPEN_ALARM_PROP_KEY, openAlarm);
                
                setPropertyBundle(bundle);
            }

            DoorResource::~DoorResource()
            {
            }

            void DoorResource::setDelegate(DoorResourceDelegate* delegate)
            {
                OIC_LOG(DEBUG, TAG, "Entered setDelegate");
                this->m_delegate = delegate;

                SHBaseResource::setDelegate(this);
            }

            bool DoorResource::changeOpenState(DoorState state)
            {
                PropertyBundle storedBundle = getPropertyBundle();

                std::string openState;
                if (state == OPEN)
                {
                    openState = OpenState::Open;
                }
                else if (state == CLOSED)
                {
                    openState = OpenState::Closed;
                }
                else
                {
                    return false;
                }

                storedBundle.setValue(OPEN_STATE_PROP_KEY, openState);
                setPropertyBundle(storedBundle);                

                return true;
            }

            DoorState DoorResource::getOpenState()
            {
                PropertyBundle storedBundle = getPropertyBundle();

                std::string openState;
                storedBundle.getValue(OPEN_STATE_PROP_KEY, openState);
                
                DoorState state = (openState == OpenState::Open) ? OPEN : CLOSED;

                return state;
            }

            bool DoorResource::changeOpenAlarmState(OpenAlarmState state)
            {
                PropertyBundle storedBundle = getPropertyBundle();

                bool openAlarm = (state == ACTIVATE) ? true : false;
                storedBundle.setValue(OPEN_ALARM_PROP_KEY, openAlarm);
                setPropertyBundle(storedBundle);
                
                return true;
            }

            OpenAlarmState DoorResource::getOpenAlarmState()
            {
                // Get "openAlarm" property value from bundle.
                bool openAlarm;
                PropertyBundle storedBundle = getPropertyBundle();
                storedBundle.getValue(OPEN_ALARM_PROP_KEY, openAlarm);
                
                if (!openAlarm)
                {
                    return DEACTIVATE;
                }
                return ACTIVATE;
            }


            ResultCode DoorResource::onGet(RequestId requestId, const ResourceQuery& query)
            {
                OIC_LOG(DEBUG, TAG, "Entered onGet");

                PropertyBundle storedBundle = getPropertyBundle();
                bool openState = false;
                storedBundle.getValue(OPEN_STATE_PROP_KEY, openState);
                
                return SUCCESS;
            }

            ResultCode DoorResource::onSet(RequestId requestId, const PropertyBundle& bundle,
                                                 const ResourceQuery& query)
            {
                OIC_LOG(DEBUG, TAG, "Entered onSet");

                std::string openState;
                bundle.getValue(OPEN_STATE_PROP_KEY, openState);

                std::string openDuration;
                bundle.getValue(OPEN_DURATION_PROP_KEY, openDuration);

                ResultCode retCode = FAIL;
                bool openAlarm;
                bundle.getValue(OPEN_ALARM_PROP_KEY, openAlarm);

                if (openAlarm)
                {
                    retCode = this->m_delegate->onActivateOpenAlarm();
                }
                else
                {
                    retCode = this->m_delegate->onDeactivateOpenAlarm();
                }

                if (retCode == SUCCESS)
                {
                    // Update property with the requested value.
                    PropertyBundle storedBundle = getPropertyBundle();
                    storedBundle.setValue(OPEN_ALARM_PROP_KEY, openAlarm);                    
                }

                return retCode;
            }
        }
    }
}
