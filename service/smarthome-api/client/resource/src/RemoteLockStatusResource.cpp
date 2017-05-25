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
#include "logger.h"

#define TAG "OIC_SH_CLIENT_REMOTELOCKSTATUS"

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
                bundle.setValue(KEY_LOCKSTATE, VALUE_LOCKED);
                setPropertyBundle(bundle);

                OIC_LOG(DEBUG, TAG, "[RemoteLockStatusResource] lock requested..");
            }

            void RemoteLockStatusResource::unlock()
            {
                PropertyBundle bundle;
                bundle.setValue(KEY_LOCKSTATE, VALUE_UNLOCKED);
                setPropertyBundle(bundle);

                OIC_LOG(DEBUG, TAG, "[RemoteLockStatusResource] unlock requested..");;
            }

            void RemoteLockStatusResource::getStatus()
            {
                getPropertyBundle();
                OIC_LOG(DEBUG, TAG, "[RemoteLockStatusResource] getStatus requested..");
            }

            void RemoteLockStatusResource::setRemoteLockStatusResourceDelegate(
                    RemoteLockStatusResourceDelegate* delegate)
            {
                SHBaseRemoteResource::setDelegate(this);

                this->m_delegate = delegate;
            }

            void RemoteLockStatusResource::onGet(PropertyBundle bundle, ResultCode ret)
            {
                if (NULL == m_delegate)
                {
                    OIC_LOG(ERROR, TAG, "[RemoteLockStatusResource] delegate not set");
                    return;
                }

                if (bundle.contains(KEY_LOCKSTATE))
                {
                    std::string value;
                    bundle.getValue(KEY_LOCKSTATE, value);
                    m_delegate->onGetStatus(value, ret);
                }
                else
                {
                    throw CommonException("Exception on get KEY_LOCKSTATE");
                }
            }

            void RemoteLockStatusResource::onSet(PropertyBundle bundle, ResultCode ret)
            {
                if (NULL == m_delegate)
                {
                    OIC_LOG(ERROR, TAG, "[RemoteLockStatusResource] delegate not set");
                    return;
                }

                if (bundle.contains(KEY_LOCKSTATE))
                {
                    std::string value;
                    bundle.getValue(KEY_LOCKSTATE, value);

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
                        OIC_LOG(ERROR, TAG, "[RemoteLockStatusResource]Invalid Property Value");
                    }
                }
                else
                {
                    throw CommonException("Exception on get KEY_LOCKSTATE");
                }
            }

            void RemoteLockStatusResource::onObserve(PropertyBundle bundle,
                        const ObserveResponse obsType,
                        ResultCode ret)
            {
                std::string value = "";

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
                    bundle.getValue(KEY_LOCKSTATE , value);
                    break;
                default:
                    OIC_LOG(ERROR, TAG, "not supported observe type");
                    //TODO
                    //error handler
                    break;
                }

                m_delegate->onGetStatus(value, obsResult);
            }
        }
    }
}
