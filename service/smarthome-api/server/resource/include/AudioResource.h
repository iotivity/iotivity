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

/**
 * This file contains the declaration of classes and its members related to
 * AudioResource.
 */

#ifndef SMARTHOME_API_SERVER_AUDIO_H_
#define SMARTHOME_API_SERVER_AUDIO_H_
#include <SHBaseResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /**
             * This class contains a set of functions to change the properties of AudioResource.
             */
            class AudioResourceDelegate
            {
            public:
                virtual ~AudioResourceDelegate() {}
                virtual ResultCode onChangeVolume(int volume, bool mute) = 0;
            };

            class AudioResource: public SHBaseResource, public SHBaseResourceDelegate
            {
            public:
                AudioResource();
                virtual ~AudioResource();

                /**
                 * API to get the volume.
                 *
                 * @return volume
                 */
                int getVolume();

                /**
                 * API to set the volume.
                 *
                 * @param[in] volume desired volume
                 * @return true if success
                 */
                bool setVolume(int volume);

                /**
                 * API to know whether mute state is on or not.
                 *
                 * @return true if success
                 */
                bool isMute();

                /**
                 * API to set the mute.
                 *
                 * @param[in] on disired mute state
                 */
                void setMute(bool on);

                /**
                 * API to set delegate function.
                 *
                 * @param[in] delegate instance of delegate
                 */
                void setDelegate(AudioResourceDelegate *delegate);

            private:
                virtual ResultCode onGet(RequestId requestId, const ResourceQuery& query);
                virtual ResultCode onSet(RequestId requestId, const PropertyBundle& bundle,
                                         const ResourceQuery& query);
                AudioResourceDelegate *m_delegate;
            };
        }
    }
}
#endif /* SMARTHOME_API_SERVER_AUDIO_H_ */
