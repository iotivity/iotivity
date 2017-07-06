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
 * RemoteAudioResource.
 */

#ifndef SMARTHOME_API_CLIENT_REMOTEAUDIO_H_
#define SMARTHOME_API_CLIENT_REMOTEAUDIO_H_
#include <SHBaseRemoteResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /**
             * This class contains a set of functions for callback of
             * RemoteAudioResource class.
             */
            class RemoteAudioResourceDelegate
            {
            public:
                virtual ~RemoteAudioResourceDelegate () {}

                /**
                 * API to handle a response about volume change request.
                 *
                 * @param[in] volume audio volume
                 * @param[in] mute true if mute
                 * @ResultCode ret true if success
                 */
                virtual void onChangeVolume(int volume, bool mute, ResultCode ret) = 0;

                /**
                 * API to handle a response about volume get request.
                 *
                 * @param[in] ret sources media sources
                 * @param[in] mute true if mute
                 * @ResultCode ret true if success
                 */
                virtual void onGetVolume(int volume, bool mute, ResultCode ret) = 0;
            };

            /**
             * This class contains a set of functions to control RemoteAudioResource.
             */
            class RemoteAudioResource: public SHBaseRemoteResource,
                    public SHBaseRemoteResourceDelegate
            {
            public:
                virtual ~RemoteAudioResource();
                /**
                 * API to set delegate of RemoteAudioResource.
                 *
                 * @param[in] delegate instance of RemoteAudioResourceDelegate
                 */
                void setDelegate(RemoteAudioResourceDelegate *delegate);

                /**
                 * API to change volume.
                 *
                 * @param[in] volume audio volume
                 * @param[in] mute true if mute
                 */
                void changeVolume(int volume, bool mute);

                /**
                 * API to get audio volume.
                 */
                void getVolume();

            protected:
                RemoteAudioResource();

            private:
                virtual void onGet(PropertyBundle bundle, ResultCode ret);
                virtual void onSet(PropertyBundle bundle, ResultCode ret);

            private:
                RemoteAudioResourceDelegate *m_delegate;
            };
        }
    }
}
#endif /* SMARTHOME_API_CLIENT_REMOTEAUDIO_H_ */
