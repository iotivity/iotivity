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
#include <LockStatusResource.h>
#include <PropertyBundle.h>
#include <ResourceQuery.h>
#include "logger.h"

#define TAG "SH_SERVER_LOCKSTATUS_RESOURCE"

const static std::string URI_LOCKSTATUS = "/lockstatus";
const static std::string KEY_LOCKSTATE = "lockState";
const static std::string VALUE_LOCKED = "Locked";
const static std::string VALUE_UNLOCKED = "Unlocked";

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            LockStatusResource::LockStatusResource() :
                    m_delegate(NULL), SHBaseResource(URI_LOCKSTATUS,
                            RESOURCE_TYPE::LOCK_STATUS)
            {
                PropertyBundle bundle;
                bundle.setValue(KEY_LOCKSTATE, VALUE_LOCKED);
                setPropertyBundle(bundle);
            }

            LockStatusResource::~LockStatusResource()
            {
            }

            std::string LockStatusResource::getState()
            {
                OIC_LOG(DEBUG, TAG, "Entered getState");
                PropertyBundle storedBundle;
                storedBundle = getPropertyBundle();

                std::string state;
                storedBundle.getValue(KEY_LOCKSTATE, state);

                return state;
            }

            bool LockStatusResource::setState(std::string state)
            {
                OIC_LOG(DEBUG, TAG, "Entered setState");
                PropertyBundle bundle;
                bundle.setValue(KEY_LOCKSTATE, state);
                setPropertyBundle(bundle);
                return true;
            }

            void LockStatusResource::setDelegate(LockStatusResourceDelegate* delegate)
            {
                OIC_LOG(DEBUG, TAG, "Entered setDelegate");
                this->m_delegate = delegate;

                SHBaseResource::setDelegate(this);
            }

            ResultCode LockStatusResource::onGet(RequestId requestId, const ResourceQuery& query)
            {
                OIC_LOG(DEBUG, TAG, "Entered onGet");

                return SUCCESS;
            }

            ResultCode LockStatusResource::onSet(RequestId requestId, const PropertyBundle& bundle,
                                                 const ResourceQuery& query)
            {
                OIC_LOG(DEBUG, TAG, "Entered onSet");

                ResultCode retCode = FAIL;
                if (NULL != this->m_delegate && bundle.contains(KEY_LOCKSTATE))
                {
                    std::string value;
                    bundle.getValue(KEY_LOCKSTATE, value);
                    if (value.compare(VALUE_LOCKED) == 0)
                    {
                        retCode = this->m_delegate->onLock();
                    }
                    else if (value.compare(VALUE_UNLOCKED) == 0)
                    {
                        retCode = this->m_delegate->onUnLock();
                    }
                    else
                    {
                        OIC_LOG(ERROR, TAG, "Invaild property value");
                        return FAIL;
                    }
                }

                return retCode;
            }
        }
    }
}
