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
#include <PropertyBundle.h>
#include <ResourceQuery.h>
#include "logger.h"

#define TAG "SH_SERVER_BINARYSWITCH_RESOURCE"

const static std::string URI_BINARYSWITCH = "/binaryswitch";
const static std::string KEY_VALUE = "value";

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            BinarySwitchResource::BinarySwitchResource()
            : m_delegate(NULL),
              SHBaseResource(URI_BINARYSWITCH, RESOURCE_TYPE::BINARYSWITCH)
            {
                OIC_LOG(DEBUG, TAG, "Entered ctor");

                bool state = false;
                PropertyBundle bundle;
                bundle.setValue(KEY_VALUE, state);
                setPropertyBundle(bundle);
            }

            BinarySwitchResource::~BinarySwitchResource()
            {
            }

            bool BinarySwitchResource::getState()
            {
                OIC_LOG(DEBUG, TAG, "Entered getState");

                // Get "value" property value from bundle.
                bool state = false;
                getPropertyBundle().getValue(KEY_VALUE, state);
                return state;
            }

            void BinarySwitchResource::setState(bool state)
            {
                OIC_LOG(DEBUG, TAG, "Entered setState");

                PropertyBundle bundle;
                bundle.setValue(KEY_VALUE, state);
                setPropertyBundle(bundle);
            }

            void BinarySwitchResource::setDelegate(BinarySwitchResourceDelegate *delegate)
            {
                OIC_LOG(DEBUG, TAG, "Entered setDelegate");
                this->m_delegate = delegate;
                SHBaseResource::setDelegate(this);
            }

            ResultCode BinarySwitchResource::onGet(RequestId requestId, const ResourceQuery& query)
            {
                OIC_LOG(DEBUG, TAG, "Entered onGet");
                return SUCCESS;
            }

            ResultCode BinarySwitchResource::onSet(RequestId requestId,
                                                   const PropertyBundle& bundle,
                                                   const ResourceQuery& query)
            {
                OIC_LOG(DEBUG, TAG, "Entered onSet");

                if (!this->m_delegate)
                {
                    OIC_LOG(ERROR, TAG, "Entered delegate is null");
                    return FAIL;
                }

                if (bundle.contains(KEY_VALUE))
                {
                    PropertyBundle storedBundle = getPropertyBundle();
                    bool value = false;
                    bundle.getValue(KEY_VALUE, value);

                    // Call application callback.
                    ResultCode retCode = FAIL;
                    if (value)
                    {
                        retCode = this->m_delegate->turnOnCallback();
                    }
                    else
                    {
                        retCode = this->m_delegate->turnOffCallback();
                    }

                    bool ret = false;
                    switch (retCode)
                    {
                        case SUCCESS:
                            // Update property with the requested value.
                            storedBundle.setValue(KEY_VALUE, value);
                            setPropertyBundle(storedBundle);
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
