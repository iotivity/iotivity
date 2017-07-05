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
#include <PropertyBundle.h>
#include <CommonException.h>
#include <RemoteAutomaticDocumentFeederResource.h>

const static std::string KEY_CURRENT_STATE = "currentAdfState";
const static std::string KEY_STATES = "adfStates";

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            RemoteAutomaticDocumentFeederResource::RemoteAutomaticDocumentFeederResource() :
                    m_delegate(NULL)
            {
            }

            RemoteAutomaticDocumentFeederResource::~RemoteAutomaticDocumentFeederResource()
            {
            }

            void RemoteAutomaticDocumentFeederResource::getState()
            {
                getPropertyBundle();
                std::cout << "[RemoteAutomaticDocumentFeeder] getStatus requested.."
                        << std::endl;
            }

            void RemoteAutomaticDocumentFeederResource::setDelegate(
                    RemoteAutomaticDocumentFeederResourceDelegate *delegate)
            {
                SHBaseRemoteResource::setDelegate(this);

                this->m_delegate = delegate;
            }

            void RemoteAutomaticDocumentFeederResource::onGet(PropertyBundle bundle,
                    ResultCode ret)
            {
                std::string currentState;
                bundle.getValue(KEY_CURRENT_STATE, currentState);

                std::list<std::string> possibleStates;
                bundle.getValue(KEY_STATES, possibleStates);

                m_delegate->onGetState(currentState, possibleStates, ret);
            }

            void RemoteAutomaticDocumentFeederResource::onSet(PropertyBundle bundle,
                    ResultCode ret)
            {
            }

            void RemoteAutomaticDocumentFeederResource::onObserve(PropertyBundle bundle,
                    const ObserveResponse obsType,
                    ResultCode ret)
            {
            }
        }
    }
}
