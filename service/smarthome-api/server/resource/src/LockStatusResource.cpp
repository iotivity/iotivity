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
#include <ResourceQuery.h>
#include <iostream>

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
                    m_delegate(NULL), SHBaseResource(URI_LOCKSTATUS)
            {
                std::list<std::string> types;
                types.push_back(RESOURCE_TYPE::LOCK_STATUS);
                setTypes(types);

                PropertyBundle bundle;
                if (!bundle.setValue(KEY_LOCKSTATE, VALUE_LOCKED))
                {
                    throw CommonException("Exception on set VALUE_LOCKED");
                }

                if (!setPropertyBundle(bundle))
                {
                    throw CommonException("Exception on set Bundle");
                }
            }

            LockStatusResource::~LockStatusResource()
            {
            }

            std::string LockStatusResource::getState()
            {
                PropertyBundle storedBundle;
                storedBundle = getPropertyBundle();

                std::string state;
                if (!storedBundle.getValue(KEY_LOCKSTATE, state))
                {
                    throw CommonException("Exception on get VALUE_LOCKED");
                }

                std::cout << "[LockStatusResource] getState" << std::endl;

                return state;
            }

            bool LockStatusResource::setState(std::string state)
            {
                PropertyBundle bundle;

                if (!bundle.setValue(KEY_LOCKSTATE, state))
                {
                    throw CommonException("Exception on set VALUE_LOCKED");
                }

                if (!setPropertyBundle(bundle))
                {
                    throw CommonException("Exception on set Bundle");
                }

                std::cout << "[LockStatusResource] setState" << std::endl;

                return true;
            }

            void LockStatusResource::setDelegate(LockStatusResourceDelegate* delegate)
            {
                this->m_delegate = delegate;

                SHBaseResource::setDelegate(this);

                std::cout << "[LockStatusResource] setDelegate" << std::endl;
            }

            ResultCode LockStatusResource::onGet(int requestId, const ResourceQuery& query)
            {
                std::cout << "[LockStatusResource] onGet" << std::endl;

                return SUCCESS;
            }

            ResultCode LockStatusResource::onSet(int requestId, const PropertyBundle& bundle,
                                                 const ResourceQuery& query)
            {
                std::cout << "[LockStatusResource] onSet" << std::endl;

                ResultCode retCode = FAIL;

                std::string value;
                if (NULL != this->m_delegate && bundle.getValue(KEY_LOCKSTATE, value))
                {
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
                        std::cout << "[LockStatusResource]Invaild property value" << std::endl;
                        return FAIL;
                    }
                }

                return retCode;
            }
        }
    }
}
