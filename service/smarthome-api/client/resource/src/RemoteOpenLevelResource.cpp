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
#include <RemoteOpenLevelResource.h>
#include <CommonException.h>
#include <PropertyBundle.h>
#include "logger.h"

#define TAG "SH_CLIENT_OPENLEVEL_RESOURCE"

const static std::string KEY_OPENLEVEL = "openLevel";
const static std::string KEY_INCREMENT = "increment";
const static std::string KEY_RANGE = "range";

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            RemoteOpenLevelResource::RemoteOpenLevelResource()
            : m_delegate(NULL)
            {
            }

            RemoteOpenLevelResource::~RemoteOpenLevelResource()
            {
            }

            void RemoteOpenLevelResource::getLevel()
            {
                OIC_LOG(DEBUG, TAG, "Entered getLevel");
                getPropertyBundle();
            }

            void RemoteOpenLevelResource::changeLevel(int level)
            {
                OIC_LOG(DEBUG, TAG, "Entered changeLevel");
                PropertyBundle bundle;
                bundle.setValue(KEY_OPENLEVEL, level);
                setPropertyBundle(bundle);
            }

            void RemoteOpenLevelResource::setDelegate(RemoteOpenLevelResourceDelegate* delegate)
            {
                OIC_LOG(DEBUG, TAG, "Entered setDelegate");
                SHBaseRemoteResource::setDelegate(this);
                m_delegate = delegate;
            }

            void RemoteOpenLevelResource::onGet(PropertyBundle bundle, ResultCode ret)
            {
                OIC_LOG(DEBUG, TAG, "Entered onGet");

                if (!m_delegate)
                {
                    OIC_LOG(DEBUG, TAG, "m_delegate is NULL");
                    return;
                }

                if (bundle.contains(KEY_OPENLEVEL)
                    && bundle.contains(KEY_INCREMENT)
                    && bundle.contains(KEY_RANGE))
                {
                    int level = 0;
                    int increment = 0;
                    std::list<int> range;
                    bundle.getValue(KEY_OPENLEVEL, level);
                    bundle.getValue(KEY_INCREMENT, increment);
                    bundle.getValue(KEY_RANGE, range);

                    int min = *(range.begin());
                    int max = *(range.rbegin());
                    m_delegate->onGetLevel(level, increment, min, max, ret);
                }
                else
                {
                    throw CommonException("Exception on get openLevel");
                }
            }

            void RemoteOpenLevelResource::onSet(PropertyBundle bundle, ResultCode ret)
            {
                OIC_LOG(DEBUG, TAG, "Entered onSet");

                if (!m_delegate)
                {
                    OIC_LOG(DEBUG, TAG, "m_delegate is NULL");
                    return;
                }

                if (bundle.contains(KEY_OPENLEVEL)
                    && bundle.contains(KEY_INCREMENT)
                    && bundle.contains(KEY_RANGE))
                {
                    int level = 0;
                    int increment = 0;
                    std::list<int> range;
                    bundle.getValue(KEY_OPENLEVEL, level);
                    bundle.getValue(KEY_INCREMENT, increment);
                    bundle.getValue(KEY_RANGE, range);

                    int min = *(range.begin());
                    int max = *(range.rbegin());
                    m_delegate->onChangeLevel(level, increment, min, max, ret);
                }
                else
                {
                    throw CommonException("Exception on set openLevel");
                }
            }

            void RemoteOpenLevelResource::onObserve(PropertyBundle bundle,
                                                    const ObserveResponse obsType,
                                                    ResultCode ret)
            {
                OIC_LOG(DEBUG, TAG, "Entered onObserve");
                return;
            }
        }
    }
}
