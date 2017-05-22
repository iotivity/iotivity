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
#include <RemoteAudioResource.h>
#include <PropertyBundle.h>
#include "logger.h"

#define TAG "SH_CLIENT_AUDIO_RESOURCE"

const static std::string KEY_VOLUME = "volume";
const static std::string KEY_MUTE = "mute";

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            RemoteAudioResource::RemoteAudioResource() :
                    m_delegate(NULL)
            {
            }

            RemoteAudioResource::~RemoteAudioResource()
            {
            }

            void RemoteAudioResource::setDelegate(RemoteAudioResourceDelegate *delegate)
            {
                OIC_LOG(DEBUG, TAG, "Entered setDelegate");
                SHBaseRemoteResource::setDelegate(this);
                this->m_delegate = delegate;
            }

            void RemoteAudioResource::onGet(PropertyBundle bundle, ResultCode ret)
            {
                OIC_LOG(DEBUG, TAG, "Entered onGet");
                if (!this->m_delegate)
                {
                    OIC_LOG(DEBUG, TAG, "m_delegate is NULL");
                    return;
                }

                int volume;
                bool mute;
                bundle.getValue(KEY_VOLUME, volume);
                bundle.getValue(KEY_MUTE, mute);

                // Call application callback.
                m_delegate->onGetVolume(volume, mute, ret);
            }

            void RemoteAudioResource::onSet(PropertyBundle bundle, ResultCode ret)
            {
                OIC_LOG(DEBUG, TAG, "Entered onSet");
                if (!this->m_delegate)
                {
                    OIC_LOG(DEBUG, TAG, "m_delegate is NULL");
                    return;
                }

                int volume;
                bool mute;
                bundle.getValue(KEY_VOLUME, volume);
                bundle.getValue(KEY_MUTE, mute);

                // Call application callback.
                m_delegate->onChangeVolume(volume, mute, ret);
            }

            void RemoteAudioResource::changeVolume(int volume, bool mute)
            {
                OIC_LOG(DEBUG, TAG, "Entered changeVolume");
                PropertyBundle bundle;
                bundle.setValue(KEY_VOLUME, volume);
                bundle.setValue(KEY_MUTE, mute);
                setPropertyBundle(bundle);
            }

            void RemoteAudioResource::getVolume()
            {
                OIC_LOG(DEBUG, TAG, "Entered getVolume");
                getPropertyBundle();
            }
        }
    }
}
