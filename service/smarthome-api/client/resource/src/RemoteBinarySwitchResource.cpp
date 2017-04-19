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
#include <iostream>

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
                bool value;

                if (NULL != m_delegate && bundle.getValue(BINARY_SWITCH_VALUE_KEY, value))
                {
                    m_delegate->onGetState(value, ret);
                }
            }

            void RemoteBinarySwitchResource::onSet(PropertyBundle bundle, ResultCode ret)
            {
                bool value;

                if (NULL != m_delegate && bundle.getValue(BINARY_SWITCH_VALUE_KEY, value))
                {
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

            void RemoteBinarySwitchResource::on()
            {
                PropertyBundle bundle;

                std::cout << "[RemoteBinarySwitchResource] turnOn requested.." << std::endl;

                bool value = true;
                bundle.setValue(BINARY_SWITCH_VALUE_KEY, value);
                setPropertyBundle(bundle);
            }

            void RemoteBinarySwitchResource::off()
            {
                PropertyBundle bundle;

                std::cout << "[RemoteBinarySwitchResource] turnOff requested.." << std::endl;

                bool value = false;
                bundle.setValue(BINARY_SWITCH_VALUE_KEY, value);
                setPropertyBundle(bundle);
            }

            void RemoteBinarySwitchResource::getState()
            {
                std::cout << "[RemoteBinarySwitchResource] getState Requested.." << std::endl;
                getPropertyBundle();
            }
        }
    }
}
