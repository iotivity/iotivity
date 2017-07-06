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
 * RemoteMediaSourceResource.
 */

#ifndef SMARTHOME_API_CLIENT_REMOTEMEDIASOURCE_H_
#define SMARTHOME_API_CLIENT_REMOTEMEDIASOURCE_H_
#include <SHBaseRemoteResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            enum MediaSourceType
            {
                AUDIO, VIDEO, AUDIO_VIDEO
            };

            typedef struct
            {
                std::string name;
                int number;
                MediaSourceType type;
                bool status;
            } MediaSource;

            /**
             * This class contains a set of functions for callback of
             * RemoteMediaSourceResource class.
             */
            class RemoteMediaSourceResourceDelegate
            {
            public:
                virtual ~RemoteMediaSourceResourceDelegate () {}

                /**
                 * API to handle a response about media source get request.
                 *
                 * @param[in] sources media sources
                 */
                virtual void onGetMediaSource(std::list<MediaSource> sources) = 0;

                /**
                 * API to handle a response about media source change request.
                 *
                 * @param[in] ret sources media sources
                 */
                virtual void onChangeMediaSource(std::list<MediaSource> sources) = 0;
            };

            /**
             * This class contains a set of functions to control RemoteMediaSourceResource.
             */
            class RemoteMediaSourceResource: public SHBaseRemoteResource,
                    public SHBaseRemoteResourceDelegate
            {
            public:
                virtual ~RemoteMediaSourceResource();

                /**
                 * API to get media sources.
                 */
                void getMediaSources();

                /**
                 * API to change media sources.
                 *
                 * @param[in] sources media sources
                 */
                void changeMediaSources(std::list<MediaSource> sources);

                /**
                 * API to set delegate of RemoteMediaSourceResource.
                 *
                 * @param[in] delegate instance of RemoteMediaSourceResourceDelegate
                 */
                void setDelegate(RemoteMediaSourceResourceDelegate *delegate);

            protected:
                RemoteMediaSourceResource();

            private:
                virtual void onGet(PropertyBundle bundle, ResultCode ret);
                virtual void onSet(PropertyBundle bundle, ResultCode ret);

            private:
                RemoteMediaSourceResourceDelegate *m_delegate;
            };
        }
    }
}
#endif /* SMARTHOME_API_CLIENT_REMOTEMEDIASOURCE_H_ */
