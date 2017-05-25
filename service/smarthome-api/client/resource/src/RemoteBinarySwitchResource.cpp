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
#include <RemoteBinarySwitchResource.h>
#include <PropertyBundle.h>
#include <logger.h>

#define TAG "OIC_SH_CLIENT_REMOTEBINARYSWITCH"

const static std::string BINARY_SWITCH_VALUE_KEY = "value";

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            RemoteBinarySwitchResource::RemoteBinarySwitchResource() :
                    m_delegate(NULL)
            {
            }

            RemoteBinarySwitchResource::~RemoteBinarySwitchResource()
            {
            }

            void RemoteBinarySwitchResource::setRemoteBinarySwitchResourceDelegate(
                    RemoteBinarySwitchResourceDelegate *delegate)
            {
                setDelegate(this);

                this->m_delegate = delegate;
            }

            void RemoteBinarySwitchResource::onGet(PropertyBundle bundle, ResultCode ret)
            {
                if (NULL != m_delegate && bundle.contains(BINARY_SWITCH_VALUE_KEY))
                {
                    bool value;
                    bundle.getValue(BINARY_SWITCH_VALUE_KEY, value);
                    m_delegate->onGetState(value, ret);
                }
            }

            void RemoteBinarySwitchResource::onSet(PropertyBundle bundle, ResultCode ret)
            {
                if (NULL != m_delegate && bundle.contains(BINARY_SWITCH_VALUE_KEY))
                {
                    bool value;
                    bundle.getValue(BINARY_SWITCH_VALUE_KEY, value);

                    if (true == value)
                    {
                        m_delegate->onTurnOn(ret);
                    }
                    else
                    {
                        m_delegate->onTurnOff(ret);
                    }
                }
            }

            void RemoteBinarySwitchResource::onObserve(PropertyBundle bundle,
                        const ObserveResponse obsType,
                        ResultCode ret)
            {
                bool value = false;

                OIC_LOG(ERROR, TAG, "[RemoteBinarySwitchResource] onObserve");
                if (NULL == m_delegate)
                {
                    OIC_LOG(ERROR, TAG, "[RemoteBinarySwitchResource] delegate not set");
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
                    bundle.getValue(BINARY_SWITCH_VALUE_KEY, value);
                    break;
                default:
                    OIC_LOG(ERROR, TAG, "not supported observe type");
                    //TODO
                    //error handler
                    break;
                }

                m_delegate->onGetState(value, obsResult);
            }

            void RemoteBinarySwitchResource::on()
            {
                PropertyBundle bundle;

                OIC_LOG(DEBUG, TAG,"[RemoteBinarySwitchResource] turnOn requested..");

                bool value = true;
                bundle.setValue(BINARY_SWITCH_VALUE_KEY, value);
                setPropertyBundle(bundle);
            }

            void RemoteBinarySwitchResource::off()
            {
                PropertyBundle bundle;

                OIC_LOG(DEBUG, TAG, "[RemoteBinarySwitchResource] turnOff requested..");

                bool value = false;
                bundle.setValue(BINARY_SWITCH_VALUE_KEY, value);
                setPropertyBundle(bundle);
            }

            void RemoteBinarySwitchResource::getState()
            {
                OIC_LOG(DEBUG, TAG, "[RemoteBinarySwitchResource] getState Requested..");
                getPropertyBundle();
            }
        }
    }
}
