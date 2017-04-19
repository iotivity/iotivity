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
#include <RemoteLockStatusResource.h>
#include <CommonException.h>
#include <PropertyBundle.h>
#include <iostream>

const static std::string KEY_LOCKSTATE = "lockState";
const static std::string VALUE_LOCKED = "Locked";
const static std::string VALUE_UNLOCKED = "Unlocked";

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            RemoteLockStatusResource::RemoteLockStatusResource() :
                    m_delegate(NULL)
            {
            }

            RemoteLockStatusResource::~RemoteLockStatusResource()
            {
            }

            void RemoteLockStatusResource::lock()
            {
                PropertyBundle bundle;
                if (!bundle.setValue(KEY_LOCKSTATE, VALUE_LOCKED))
                {
                    throw CommonException("Exception on set VALUE_LOCKED");
                }

                setPropertyBundle(bundle);

                std::cout << "[RemoteLockStatusResource] lock requested.." << std::endl;
            }

            void RemoteLockStatusResource::unlock()
            {
                PropertyBundle bundle;
                if (!bundle.setValue(KEY_LOCKSTATE, VALUE_UNLOCKED))
                {
                    throw CommonException("Exception on set VALUE_UNLOCKED");
                }

                setPropertyBundle(bundle);

                std::cout << "[RemoteLockStatusResource] unlock requested.." << std::endl;
            }

            void RemoteLockStatusResource::getStatus()
            {
                getPropertyBundle();
                std::cout << "[RemoteLockStatusResource] getStatus requested.." << std::endl;
            }

            void RemoteLockStatusResource::setRemoteLockStatusResourceDelegate(
                    RemoteLockStatusResourceDelegate* delegate)
            {
                SHBaseRemoteResource::setDelegate(this);

                this->m_delegate = delegate;
            }

            void RemoteLockStatusResource::onGet(PropertyBundle bundle, ResultCode ret)
            {
                std::string value;

                if (NULL != m_delegate)
                {
                    std::cout << "[RemoteLockStatusResource]m_delegate is NULL" << std::endl;
                    return;
                }

                if (bundle.getValue(KEY_LOCKSTATE, value))
                {
                    m_delegate->onGetStatus(value, ret);
                }
                else
                {
                    throw CommonException("Exception on get KEY_LOCKSTATE");
                }
            }

            void RemoteLockStatusResource::onSet(PropertyBundle bundle, ResultCode ret)
            {
                if (NULL != m_delegate)
                {
                    std::cout << "[RemoteLockStatusResource]m_delegate is NULL" << std::endl;
                    return;
                }

                std::string value;
                if (bundle.getValue(KEY_LOCKSTATE, value))
                {
                    if (value.compare(VALUE_LOCKED) == 0)
                    {
                        m_delegate->onLock(ret);
                    }
                    else if (value.compare(VALUE_UNLOCKED) == 0)
                    {
                        m_delegate->onUnLock(ret);
                    }
                    else
                    {
                        std::cout << "[RemoteLockStatusResource]Invalid Property Value"
                                << std::endl;
                    }
                }
                else
                {
                    throw CommonException("Exception on get KEY_LOCKSTATE");
                }
            }
        }
    }
}
