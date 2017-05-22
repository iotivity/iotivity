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
#include <AudioResource.h>
#include <PropertyBundle.h>
#include <ResourceQuery.h>
#include "logger.h"

#define TAG "SH_SERVER_AUDIO_RESOURCE"

const static std::string URI_AUDIO = "/audio";
const static std::string KEY_VOLUME = "volume";
const static std::string KEY_MUTE = "mute";

const static int MINIMUM_VOLUME = 0;
const static int MAXIMUM_VOLUME = 100;

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            AudioResource::AudioResource() :
                    m_delegate(NULL), SHBaseResource(URI_AUDIO, RESOURCE_TYPE::AUDIO,
                        RESOURCE_PROPERTY::DISCOVERABLE | RESOURCE_PROPERTY::OBSERVABLE)
            {
            }

            AudioResource::~AudioResource()
            {
            }

            int AudioResource::getVolume()
            {
                OIC_LOG(DEBUG, TAG, "Entered getVolume");

                // Get "volume" property value from bundle.
                int volume;
                getPropertyBundle().getValue(KEY_VOLUME, volume);
                return volume;
            }

            bool AudioResource::setVolume(int volume)
            {
                OIC_LOG(DEBUG, TAG, "Entered setVolume");

                if (volume < MINIMUM_VOLUME || MAXIMUM_VOLUME < volume)
                {
                    OIC_LOG(ERROR, TAG, "Invalid volume. Possible values are: [0-100]");
                    return false;
                }

                PropertyBundle bundle = getPropertyBundle();
                bundle.setValue(KEY_VOLUME, volume);
                setPropertyBundle(bundle);
                return true;
            }

            bool AudioResource::isMute()
            {
                OIC_LOG(DEBUG, TAG, "Entered isMute");

                // Get "mute" property value from bundle.
                bool mute;
                getPropertyBundle().getValue(KEY_MUTE, mute);
                return mute;
            }

            void AudioResource::setMute(bool on)
            {
                OIC_LOG(DEBUG, TAG, "Entered setMute");

                PropertyBundle bundle = getPropertyBundle();
                bundle.setValue(KEY_MUTE, on);
                setPropertyBundle(bundle);
            }

            void AudioResource::setDelegate(AudioResourceDelegate *delegate)
            {
                OIC_LOG(DEBUG, TAG, "Entered setDelegate");

                m_delegate = delegate;
                SHBaseResource::setDelegate(this);
            }

            ResultCode AudioResource::onGet(RequestId requestId, const ResourceQuery& query)
            {
                OIC_LOG(DEBUG, TAG, "Entered onGet");
                return SUCCESS;
            }

            ResultCode AudioResource::onSet(RequestId requestId, const PropertyBundle& bundle,
                                            const ResourceQuery& query)
            {
                OIC_LOG(DEBUG, TAG, "Entered onSet");

                if (!this->m_delegate)
                {
                    OIC_LOG(DEBUG, TAG, "m_delegate is NULL");
                    return FAIL;
                }

                if (!bundle.contains(KEY_VOLUME))
                {
                    OIC_LOG(ERROR, TAG, "volume is mandatory property");
                    return FAIL;
                }

                if (!bundle.contains(KEY_MUTE))
                {
                    OIC_LOG(ERROR, TAG, "mute is mandatory property");
                    return FAIL;
                }

                int volume;
                bool mute;
                bundle.getValue(KEY_VOLUME, volume);
                bundle.getValue(KEY_MUTE, mute);

                // Call application callback.
                ResultCode retCode = this->m_delegate->onChangeVolume(volume, mute);
                if (retCode == SUCCESS)
                {
                    // Update property with the requested value.
                    PropertyBundle storedBundle = getPropertyBundle();
                    storedBundle.setValue(KEY_VOLUME, volume);
                    storedBundle.setValue(KEY_MUTE, mute);
                    setPropertyBundle(storedBundle);
                }
                return retCode;
            }
        }
    }
}
