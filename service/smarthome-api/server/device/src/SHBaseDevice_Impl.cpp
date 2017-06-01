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
#include <SHBaseDevice_Impl.h>
#include <octypes.h>
#include <ocstack.h>
#include "logger.h"

#define TAG "SH_SERVER_BASE_DEVICE"

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            SHBaseDevice_Impl::SHBaseDevice_Impl()
            {
            }

            SHBaseDevice_Impl::~SHBaseDevice_Impl()
            {
            }

            std::string SHBaseDevice_Impl::getType()
            {
                return m_deviceType;
            }

            void SHBaseDevice_Impl::setType(std::string deviceType)
            {
                OCResourceHandle resourceHandle = OCGetResourceHandleAtUri(OC_RSRVD_DEVICE_URI);
                if (!resourceHandle)
                {
                    OIC_LOG(ERROR, TAG, "Device Resource does not exist");
                    return;
                }

                OCStackResult ret = OCBindResourceTypeToResource(resourceHandle,
                                                                 deviceType.c_str());
                if (OC_STACK_OK != ret)
                {
                    OIC_LOG(ERROR, TAG, "Failed to bind deviceType");
                    return;
                }

                m_deviceType = deviceType;
            }

            std::string SHBaseDevice_Impl::getName()
            {
                return m_deviceName;
            }

            void SHBaseDevice_Impl::setName(std::string deviceName)
            {
                OCStackResult ret = OCSetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_DEVICE_NAME,
                                                       deviceName.c_str());
                if (OC_STACK_OK != ret)
                {
                    OIC_LOG(ERROR, TAG, "Failed to set deviceName");
                    return;
                }

                m_deviceName = deviceName;
            }
        }
    }
}
