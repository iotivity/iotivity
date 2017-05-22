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
#include <RemoteMediaSourceInputResource.h>
#include <PropertyBundle.h>
#include "logger.h"

#define TAG "SH_CLIENT_MEDIASOURCEINPUT_RESOURCE"

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            RemoteMediaSourceInputResource::RemoteMediaSourceInputResource()
            {
            }

            RemoteMediaSourceInputResource::~RemoteMediaSourceInputResource()
            {
            }

            void RemoteMediaSourceInputResource::setDelegate(
                                            RemoteMediaSourceInputResourceDelegate *delegate)
            {
                RemoteMediaSourceResource::setDelegate(delegate);
            }

            void RemoteMediaSourceInputResource::getMediaSources()
            {
                OIC_LOG(DEBUG, TAG, "Entered getMediaSources");
                RemoteMediaSourceResource::getMediaSources();
            }

            void RemoteMediaSourceInputResource::changeMediaSources(std::list<MediaSource> sources)
            {
                OIC_LOG(DEBUG, TAG, "Entered changeMediaSources");
                RemoteMediaSourceResource::changeMediaSources(sources);
            }
        }
    }
}
