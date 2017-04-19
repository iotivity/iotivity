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
#include <MediaResource.h>
#include <PropertyBundle.h>
#include <ResourceQuery.h>
#include "logger.h"

#define TAG "OIC_SH_SERVER_MEDIA"

const static std::string URI_MEDIA = "/media";
const static std::string KEY_MEDIA = "media";
const static std::string KEY_URL = "url";
const static std::string KEY_SDP = "sdp";

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            MediaResource::MediaResource() :
                m_userDelegate(NULL), SHBaseResource(URI_MEDIA, RESOURCE_TYPE::MEDIA,
                            RESOURCE_PROPERTY::DISCOVERABLE | RESOURCE_PROPERTY::OBSERVABLE)
            {
            }

            MediaResource::~MediaResource()
            {
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

                    mediaInfo.push_back(info);
                }

                return mediaInfo;
            }

            PropertyBundle convertMediaInfoToBundle(MediaInfo &mediaInfo)
            {
                PropertyBundle bundle;
                bundle.setValue(KEY_URL, mediaInfo.url);
                bundle.setValue(KEY_SDP, mediaInfo.sdp);

                return bundle;
            }

            std::list<MediaInfo> MediaResource::getMediaInfoList() const
            {
                PropertyBundle storedBundle = getPropertyBundle();

                std::list<PropertyBundle> mediaBundle;
                storedBundle.getValue(KEY_MEDIA, mediaBundle);
                return convertBundleToMediaInfo(mediaBundle);
            }

            void MediaResource::addMediaInfo(MediaInfo mediaInfo)
            {
                if (mediaInfo.url.empty() || mediaInfo.sdp.empty())
                {
                    throw CommonException("invalid parameter!");
                }

                PropertyBundle bundle = convertMediaInfoToBundle(mediaInfo);

                //TODO
                //if (0 == bundle.size())
                //{
                    //return false;
                //}

                PropertyBundle storedBundle = getPropertyBundle();

                std::list<PropertyBundle> mediaBundle;
                storedBundle.getValue(KEY_MEDIA, mediaBundle);
                mediaBundle.push_back(bundle);

                storedBundle.setValue(KEY_MEDIA, mediaBundle);
                setPropertyBundle(storedBundle);
            }

            void MediaResource::setMediaResourceDelegate(
                    MediaResourceDelegate *mediaDelegate)
            {
                OIC_LOG(DEBUG, TAG, "[MediaResource] setMediaResourceDelegate");
                m_userDelegate = mediaDelegate;
                setDelegate(this);
            }

            ResultCode MediaResource::onGet(RequestId requestId, const ResourceQuery &query)
            {
                OIC_LOG(DEBUG, TAG, "[MediaResource] onGet");
                return SUCCESS;
            }

            ResultCode MediaResource::onSet(RequestId requestId, const PropertyBundle &bundle,
                    const ResourceQuery &query)
            {
                OIC_LOG(ERROR, TAG, "[MediaResource] onSet is not supported!");
                return FAIL;
            }
        }
    }
}
