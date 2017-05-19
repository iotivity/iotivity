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
#include <DeviceDiscovery.h>
#include <SHPlatform_Impl.h>
#include <SHPlatformConfig.h>
#include <CommonException.h>
#include <ClientCallbackContext.h>
#include <ClientCallbackWrapper.h>
#include <ocstack.h>
#include "logger.h"

#define TAG "OIC_SH_CLIENT_DEVICEDISCOVERY"

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            DeviceDiscovery::DeviceDiscovery() :
                    m_delegate(NULL)
            {
                SH_Impl::start();
            }

            DeviceDiscovery::~DeviceDiscovery()
            {
                SH_Impl::stop();
            }

            void DeviceDiscovery::setDelegate(DeviceDiscoveryDelegate *delegate)
            {
                this->m_delegate = delegate;
            }

            void DeviceDiscovery::findRemoteDevices()
            {
                OIC_LOG(DEBUG, TAG, "findRemoteDevices call!");

                if (!m_delegate)
                {
                    throw CommonException("delegate is not set");
                    return;
                }

                std::string uri = OC_RSRVD_WELL_KNOWN_URI;

                ClientCallbackContext *ctx = new ClientCallbackContext(this->m_delegate);

                OCCallbackData cbdata;
                cbdata.context = static_cast<void*>(ctx);
                cbdata.cb = ClientCallbackWrapper::DeviceDiscoveryCallback::findRemoteDevices;
                cbdata.cd = ClientCallbackWrapper::CallbackHelper::CallbackContextDeleter;

                SHPlatformConfig cfg = SH_Impl::getConfig();

                //TODO
                //csdk lock?
                //discovery need handle?

                OCStackResult result = OCDoRequest(NULL, OC_REST_DISCOVER,
                        uri.c_str(),
                        NULL, NULL,
                        CT_DEFAULT/*connectivityType*/,
                        static_cast<OCQualityOfService>(cfg.qos),
                        &cbdata,
                        NULL, 0);

                if (OC_STACK_OK != result)
                {
                    throw CommonException("OCDoRequest failed!");
                }
            }

            void DeviceDiscovery::findRemoteDevices(const DiscoveryQuery &query)
            {
                OIC_LOG_V(DEBUG, TAG, "findRemoteDevices call! query :%s",
                        query.getDeviceType().c_str());

                if (!m_delegate)
                {
                    throw CommonException("delegate is not set");
                    return;
                }

                std::string uri = OC_RSRVD_WELL_KNOWN_URI;
                std::string deviceType = query.getDeviceType();

                ClientCallbackContext *ctx = NULL;
                if (deviceType.empty())
                {
                    ctx = new ClientCallbackContext(this->m_delegate);
                }
                else
                {
                    ctx = new ClientCallbackContext(this->m_delegate, deviceType);
                }

                OCCallbackData cbdata;
                cbdata.context = static_cast<void*>(ctx);
                cbdata.cb = ClientCallbackWrapper::DeviceDiscoveryCallback::findRemoteDevices;
                cbdata.cd = ClientCallbackWrapper::CallbackHelper::CallbackContextDeleter;

                SHPlatformConfig cfg = SH_Impl::getConfig();

                //TODO
                //csdk lock?
                //discovery need handle?

                OCStackResult result = OCDoRequest(NULL, OC_REST_DISCOVER,
                        uri.c_str(),
                        NULL, NULL,
                        CT_DEFAULT/*connectivityType*/,
                        static_cast<OCQualityOfService>(cfg.qos),
                        &cbdata,
                        NULL, 0);

                if (OC_STACK_OK != result)
                {
                    throw CommonException("OCDoRequest failed!");
                }
            }
        }
    }
}
