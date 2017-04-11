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
#include <iostream>

const static std::string MODE_VALUE_MODES = "modes";

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
                return;
            }

            void RemoteModeResource::onSet(PropertyBundle bundle, ResultCode ret)
            {
                return;
            }

            void RemoteModeResource::getMode()
            {
                std::cout << "[RemoteModeResource] getMode.." << std::endl;
                getPropertyBundle();
                return;
            }

            void RemoteModeResource::setMode(std::list< std::string > mode)
            {
                std::cout << "[RemoteModeResource] setMode.." << std::endl;

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
