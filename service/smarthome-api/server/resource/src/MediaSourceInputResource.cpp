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
#include <MediaSourceInputResource.h>
#include <ResourceQuery.h>
#include "logger.h"

#define TAG "SH_SERVER_MEDIASOURCEINPUT_RESOURCE"

const static std::string URI_MEDIA_INPUT = "/mediaInput";

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            MediaSourceInputResource::MediaSourceInputResource()
            : MediaSourceResource(URI_MEDIA_INPUT)
            {
            }

            MediaSourceInputResource::~MediaSourceInputResource()
            {
            }

            void MediaSourceInputResource::setDelegate(MediaSourceInputResourceDelegate *delegate)
            {
                MediaSourceResource::setDelegate(delegate);
            }

            std::list<MediaSource> MediaSourceInputResource::getMediaSources() const
            {
                OIC_LOG(DEBUG, TAG, "Entered getMediaSources");
                return MediaSourceResource::getMediaSources();
            }

            bool MediaSourceInputResource::addMediaSource(const MediaSource& source)
            {
                OIC_LOG(DEBUG, TAG, "Entered addMediaSource");
                return MediaSourceResource::addMediaSource(source);
            }

            bool MediaSourceInputResource::removeMediaSource(const MediaSource& source)
            {
                OIC_LOG(DEBUG, TAG, "Entered removeMediaSource");
                return MediaSourceResource::removeMediaSource(source);
            }

            bool MediaSourceInputResource::updateMediaSource(const MediaSource& source)
            {
                OIC_LOG(DEBUG, TAG, "Entered updateMediaSource");
                return MediaSourceResource::updateMediaSource(source);
            }
        }
    }
}
