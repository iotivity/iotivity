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
#include <BinarySwitchResource.h>
#include <ResourceQuery.h>
#include <iostream>

const static std::string URI_BINARYSWITCH = "/binaryswitch";
const static std::string KEY_VALUE = "value";

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            BinarySwitchResource::BinarySwitchResource() :
                    m_userDelegate(NULL), SHBaseResource(URI_BINARYSWITCH)
            {
                std::cout << "[BinarySwitchResource] constructor" << std::endl;

                std::list< std::string > types;
                types.push_back(RESOURCE_TYPE::BINARYSWITCH);
                setTypes(types);

                bool state = false;
                PropertyBundle bundle;
                if (!bundle.setValue(KEY_VALUE, state))
                {
                    throw CommonException("Exception on set BINARY_SWITCH_PROP_KEY");
                }

                if (!setPropertyBundle(bundle))
                {
                    throw CommonException("Exception on set PropertyBundle");
                }
            }

            BinarySwitchResource::~BinarySwitchResource()
            {
                if (!deletePropertyBundle())
                {
                    throw CommonException("Exception on delete PropertyBundle");
                }
            }

            bool BinarySwitchResource::getState()
            {
                std::cout << "[BinarySwitchResource] getState" << std::endl;

                // Get "value" property value from bundle.
                bool state = false;
                if (!getPropertyBundle().getValue(KEY_VALUE, state))
                {
                    throw CommonException("Exception on get BINARY_SWITCH_PROP_KEY");
                }
                return state;
            }

            void BinarySwitchResource::setState(bool state)
            {
                std::cout << "[BinarySwitchResource] setState" << std::endl;

                PropertyBundle bundle;
                if (!bundle.setValue(KEY_VALUE, state))
                {
                    throw CommonException("Exception on set BINARY_SWITCH_PROP_KEY");
                }

                if (!setPropertyBundle(bundle))
                {
                    throw CommonException("Exception on set PropertyBundle");
                }
            }

            void BinarySwitchResource::setBinarySwitchResourceDelegate(
                    BinarySwitchResourceDelegate *switchDelegate)
            {
                std::cout << "[BinarySwitchResource] setBinarySwitchResourceDelegate" << std::endl;
                m_userDelegate = switchDelegate;
                setDelegate(this);
            }

            ResultCode BinarySwitchResource::onGet(int requestId, const ResourceQuery& query)
            {
                std::cout << "[BinarySwitchResource] onGet" << std::endl;
                return SUCCESS;
            }

            ResultCode BinarySwitchResource::onSet(int requestId, const PropertyBundle& bundle,
                                                   const ResourceQuery& query)
            {
                std::cout << "[BinarySwitchResource] onSet" << std::endl;

                if (!this->m_userDelegate)
                {
                    std::cout << "[BinarySwitchResource] delegate is null" << std::endl;
                    return FAIL;
                }

                PropertyBundle storedBundle = getPropertyBundle();
                bool value = false;
                if (bundle.getValue(KEY_VALUE, value))
                {
                    // Call application callback.
                    ResultCode retCode = FAIL;
                    if (value)
                    {
                        retCode = this->m_userDelegate->turnOnCallback();
                    }
                    else
                    {
                        retCode = this->m_userDelegate->turnOffCallback();
                    }

                    bool ret = false;
                    switch (retCode)
                    {
                        case SUCCESS:
                            // Update property with the requested value.
                            ret = storedBundle.setValue(KEY_VALUE, value);
                            if (!ret)
                            {
                                // Error response will be sent by Resource Class.
                                std::cout << "Failed to update value property" << std::endl;
                                return FAIL;
                            }
                            ret = setPropertyBundle(storedBundle);
                            if (!ret)
                            {
                                // Error response will be sent by Resource Class.
                                std::cout << "Failed to replace PropertyBundle" << std::endl;
                                return FAIL;
                            }
                            return SUCCESS;
                        case FAIL:
                            return FAIL;
                        case KEEP:
                            return KEEP;
                        default:
                            return KEEP;
                    }
                }
                else
                {
                    // "value" is mandatory property.
                    return FAIL;
                }
                return SUCCESS;
            }
        }
    }
}
