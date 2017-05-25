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
#include <RemoteModeResource.h>
#include <PropertyBundle.h>
#include "logger.h"

#define TAG "OIC_SH_CLIENT_REMOTEMODE"

const static std::string MODE_VALUE_MODES = "modes";
const static std::string MODE_VALUE_SUPPORTED_MODES = "supportedModes";

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            RemoteModeResource::RemoteModeResource() :
                    m_delegate(NULL)
            {
            }

            RemoteModeResource::~RemoteModeResource()
            {
            }

            void RemoteModeResource::setRemoteModeResourceDelegate(
                    RemoteModeResourceDelegate *delegate)
            {
                setDelegate(this);
                this->m_delegate = delegate;
                return;
            }

            void RemoteModeResource::onGet(PropertyBundle bundle, ResultCode ret)
            {
                OIC_LOG(DEBUG, TAG, "Entered onGet");

                if (NULL != m_delegate)
                {
                    OIC_LOG(ERROR, TAG, "m_delegate is NULL");
                    return;
                }

                if (bundle.contains(MODE_VALUE_MODES))
                {
                    std::list< std::string > supportedMode;
                    std::list< std::string > currentMode;
                    bundle.getValue(MODE_VALUE_MODES, currentMode);
                    bundle.getValue(MODE_VALUE_SUPPORTED_MODES, supportedMode);
                    m_delegate->onGetMode(supportedMode, currentMode, ret);
                }
                else
                {
                    OIC_LOG(ERROR, TAG, "Empty response");
                }
                return;
            }

            void RemoteModeResource::onSet(PropertyBundle bundle, ResultCode ret)
            {
                OIC_LOG(DEBUG, TAG, "Entered onSet");

                if (NULL != m_delegate)
                {
                    OIC_LOG(ERROR, TAG, "m_delegate is NULL");
                    return;
                }

                if (bundle.contains(MODE_VALUE_MODES))
                {
                    std::list< std::string > supportedMode;
                    std::list< std::string > currentMode;
                    bundle.getValue(MODE_VALUE_MODES, currentMode);
                    bundle.getValue(MODE_VALUE_SUPPORTED_MODES, supportedMode);
                    m_delegate->onSetMode(supportedMode, currentMode, ret);
                }
                else
                {
                    OIC_LOG(ERROR, TAG, "Empty response");
                }
                return;
            }

            void RemoteModeResource::onObserve(PropertyBundle bundle,
                        const ObserveResponse obsType,
                        ResultCode ret)
            {
                std::list< std::string > supportedMode;
                std::list< std::string > currentMode;

                if (NULL == m_delegate)
                {
                    OIC_LOG(ERROR, TAG, "delegate not set");
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
                    bundle.getValue(MODE_VALUE_MODES, currentMode);
                    bundle.getValue(MODE_VALUE_SUPPORTED_MODES, supportedMode);
                    break;
                default:
                    OIC_LOG(ERROR, TAG, "not supported observe type");
                    //TODO
                    //error handler
                    break;
                }

                m_delegate->onSetMode(supportedMode, currentMode, ret);
                return;
            }

            void RemoteModeResource::getMode()
            {
                OIC_LOG(DEBUG, TAG, "Entered getMode");
                getPropertyBundle();
                return;
            }

            void RemoteModeResource::setMode(std::list< std::string > mode)
            {
                OIC_LOG(DEBUG, TAG, "Entered setMode");

                if (mode.empty())
                {
                    return;
                }

                PropertyBundle bundle;
                bundle.setValue(MODE_VALUE_MODES, mode);
                setPropertyBundle(bundle);
                return;
            }
        }
    }
}