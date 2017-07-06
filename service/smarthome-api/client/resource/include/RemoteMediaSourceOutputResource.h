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
 * RemoteMediaSourceOutputResource.
 */

#ifndef SMARTHOME_API_CLIENT_REMOTEMEDIASOURCEOUTPUT_H_
#define SMARTHOME_API_CLIENT_REMOTEMEDIASOURCEOUTPUT_H_
#include <RemoteMediaSourceResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /**
             * This class contains a set of functions for callback of
             * RemoteMediaSourceOutputResource class.
             */
            class RemoteMediaSourceOutputResourceDelegate: public RemoteMediaSourceResourceDelegate
            {
            public:
                virtual ~RemoteMediaSourceOutputResourceDelegate() {}

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
             * This class contains a set of functions to control RemoteMediaSourceOutputResource.
             */
            class RemoteMediaSourceOutputResource: public RemoteMediaSourceResource
            {
            public:
                virtual ~RemoteMediaSourceOutputResource();

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
                 * API to set delegate of RemoteMediaSourceOutputResource.
                 *
                 * @param[in] delegate instance of RemoteMediaSourceOutputResourceDelegate
                 */
                void setDelegate(RemoteMediaSourceOutputResourceDelegate *delegate);

            protected:
                RemoteMediaSourceOutputResource();
            };
        }
    }
}
#endif /* SMARTHOME_API_CLIENT_REMOTEMEDIASOURCEOUTPUT_H_ */
