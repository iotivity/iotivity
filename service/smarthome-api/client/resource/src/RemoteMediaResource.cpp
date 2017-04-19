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
#include <RemoteMediaResource.h>
#include <CommonException.h>
#include <PropertyBundle.h>
#include "logger.h"

#define TAG "OIC_SH_CLIENT_REMOTEMEDIA"

const static std::string KEY_MEDIA = "media";
const static std::string KEY_URL = "url";
const static std::string KEY_SDP = "sdp";

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            RemoteMediaResource::RemoteMediaResource() :
                    m_delegate(NULL)
            {
            }

            RemoteMediaResource::~RemoteMediaResource()
            {
            }

            void RemoteMediaResource::setRemoteMediaResourceDelegate(
                    RemoteMediaResourceDelegate *delegate)
            {
                setDelegate(this);

                this->m_delegate = delegate;
            }

            std::list<MediaInfo> convertBundleToMediaInfo(std::list<PropertyBundle> &bundleList)
            {
                if (0 == bundleList.size())
                {
                    throw CommonException("can't find any Media Info.");
                }

                std::list<PropertyBundle>::iterator iter;
                std::list<MediaInfo> mediaInfo;
                for(iter = bundleList.begin(); iter != bundleList.end(); iter++)
                {
                    PropertyBundle bundle = (*iter);
                    MediaInfo info;
                    bundle.getValue(KEY_URL, info.url);
                    bundle.getValue(KEY_SDP, info.sdp);

                    mediaInfo.push_back(info);
                }

                return mediaInfo;
            }

            void RemoteMediaResource::onGet(PropertyBundle bundle, ResultCode ret)
            {
                std::list<PropertyBundle> media;

                if (NULL != m_delegate)
                {
                    bundle.getValue(KEY_MEDIA, media);
                    std::list<MediaInfo> mediaInfo = convertBundleToMediaInfo(media);

                    m_delegate->onGetMedia(mediaInfo, ret);
                }
            }

            void RemoteMediaResource::onSet(PropertyBundle bundle, ResultCode ret)
            {
                throw CommonException("Set request is not supported!");
                return;
            }

            void RemoteMediaResource::onObserve(PropertyBundle bundle,
                        const ObserveResponse obsType,
                        ResultCode ret)
            {
                OIC_LOG(DEBUG, TAG, "Entered onObserve");
                return;
            }

            void RemoteMediaResource::getMedia()
            {
                OIC_LOG(DEBUG, TAG, "[RemoteMediaResource] getState Requested..");
                getPropertyBundle();
            }
        }
    }
}
