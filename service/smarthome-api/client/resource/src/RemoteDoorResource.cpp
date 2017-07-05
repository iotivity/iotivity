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
#include <RemoteDoorResource.h>
#include <CommonException.h>
#include <PropertyBundle.h>
#include "logger.h"

#define TAG "OIC_SH_CLIENT_REMOTEDOOR"

const static std::string OPEN_STATE_PROP_KEY = "openState";
const static std::string OPEN_DURATION_PROP_KEY = "openDuration";
const static std::string OPEN_ALARM_PROP_KEY = "openAlarm";

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            RemoteDoorResource::RemoteDoorResource() :
                    m_delegate(NULL)
            {
            }

            RemoteDoorResource::~RemoteDoorResource()
            {
            }

            void RemoteDoorResource::getOpenState()
            {
                getPropertyBundle();
            }

            void RemoteDoorResource::getOpenDuration()
            {
                getPropertyBundle();
            }

            void RemoteDoorResource::getOpenAlarmState()
            {
                getPropertyBundle();
            }

            void RemoteDoorResource::changeOpenAlarmState(OpenAlarmState state)
            {
                bool openAlarm = (state == ACTIVATE) ? true : false;
                PropertyBundle bundle;
                bundle.setValue(OPEN_ALARM_PROP_KEY, openAlarm);
                setPropertyBundle(bundle);
            }

            void RemoteDoorResource::setRemoteDoorResourceDelegate(
                    RemoteDoorResourceDelegate* delegate)
            {
                SHBaseRemoteResource::setDelegate(this);

                this->m_delegate = delegate;
            }

            void RemoteDoorResource::onGet(PropertyBundle bundle, ResultCode ret)
            {
                if (NULL == m_delegate)
                {
                    return;
                }

                std::string state;
                bundle.getValue(OPEN_STATE_PROP_KEY, state);
                
                m_delegate->onGetOpenState(state, ret);

                std::string duration;
                bundle.getValue(OPEN_DURATION_PROP_KEY, duration);
                
                m_delegate->onGetOpenDuration(duration, ret);

                OpenAlarmState alramState;
                bundle.getValue(OPEN_ALARM_PROP_KEY, alramState);
                
                m_delegate->onGetOpenAlarmState(alramState, ret);
            }

            void RemoteDoorResource::onSet(PropertyBundle bundle, ResultCode ret)
            {
                if (NULL == m_delegate)
                {
                    return;
                }

                OpenAlarmState alramState;
                bundle.getValue(OPEN_ALARM_PROP_KEY, alramState);                

                if (alramState == ACTIVATE)
                {
                    m_delegate->onActivateOpenAlarm(ret);
                }
                else if (alramState == DEACTIVATE)
                {
                    m_delegate->onDeActivateOpenAlarm(ret);
                }
                else
                {
                    throw CommonException("Invalid Property Value");
                }
            }

            void RemoteDoorResource::onObserve(PropertyBundle bundle,
                        const ObserveResponse obsType,
                        ResultCode ret)
            {
                std::string state = "";
                std::string duration = "";
                OpenAlarmState alramState;

                if (NULL == m_delegate)
                {
                    OIC_LOG(ERROR, TAG, "[RemoteLockStatusResource] delegate not set");
                    return;
                }

                ResultCode obsResult = SUCCESS;
                switch(obsType)
                {
                case REGISTER:
                    obsResult = (ret == SUCCESS) ?
                        OBSERVE_REGISTER_SUCCESS : OBSERVE_REGISTER_FAIL;
                    break;
                case UNREGISTER:
                    obsResult = (ret == SUCCESS) ?
                        OBSERVE_UNREGISTER_SUCCESS : OBSERVE_UNREGISTER_FAIL;
                    break;
                case NOTIFY:
                    obsResult = OBSERVE_NOTIFY;
                    bundle.getValue(OPEN_STATE_PROP_KEY, state);
                    bundle.getValue(OPEN_DURATION_PROP_KEY, duration);
                    bundle.getValue(OPEN_ALARM_PROP_KEY, alramState);
                    break;
                default:
                    OIC_LOG(ERROR, TAG, "not supported observe type");
                    //TODO
                    //error handler
                    break;
                }
                
                m_delegate->onGetOpenState(state, obsResult);
                m_delegate->onGetOpenDuration(duration, obsResult);
                m_delegate->onGetOpenAlarmState(alramState, obsResult);

            }
        }
    }
}
