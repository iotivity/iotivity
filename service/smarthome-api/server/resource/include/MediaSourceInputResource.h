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
 * MediaSourceInputResource.
 */

#ifndef SMARTHOME_API_SERVER_MEDIASOURCEINPUT_H_
#define SMARTHOME_API_SERVER_MEDIASOURCEINPUT_H_
#include <MediaSourceResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /**
             * This class contains a set of callback functions handling events and requests
             * for MediaSourceInputResource class
             */
            class MediaSourceInputResourceDelegate: public MediaSourceResourceDelegate
            {
            public:
                virtual ~MediaSourceInputResourceDelegate() {}
                virtual ResultCode onChangeMediaSources(std::list<MediaSource> sources) = 0;
            };

            /**
             * This class contains a set of functions to change the properties of
             * MediaSourceInputResource
             */
            class MediaSourceInputResource: public MediaSourceResource
            {
            public:
                MediaSourceInputResource();
                virtual ~MediaSourceInputResource();

                /**
                 * API to get media source.
                 *
                 * @return :: std::list<MediaInfo>  list of medif source.
                 */
                std::list<MediaSource> getMediaSources() const;

                /**
                 * API to add media source.
                 *
                 * @param source media source that will be added
                 * @return true if success
                 */
                bool addMediaSource(const MediaSource& source);

                /**
                 * API to remove media source.
                 *
                 * @param source media source that will be removed
                 * @return true if success
                 */
                bool removeMediaSource(const MediaSource& source);

                /**
                 * API to update media source.
                 *
                 * @param source media source that will be update
                 * @return true if success
                 */
                bool updateMediaSource(const MediaSource& source);

                /**
                 * API to set delegate.
                 *
                 * @param delegate instance of delegate
                 */
                void setDelegate(MediaSourceInputResourceDelegate *delegate);
            };
        }
    }
}
#endif /* SMARTHOME_API_SERVER_MEDIASOURCEINPUT_H_ */
