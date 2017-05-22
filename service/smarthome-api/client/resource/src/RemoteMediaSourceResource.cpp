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
#include <RemoteMediaSourceResource.h>
#include <PropertyBundle.h>
#include <CommonException.h>
#include "logger.h"

#define TAG "SH_CLIENT_MEDIASOURCE_RESOURCE"

const static std::string KEY_SOURCES = "sources";
const static std::string KEY_SOURCE_NAME = "sourceName";
const static std::string KEY_SOURCE_NUMBER = "sourceNumber";
const static std::string KEY_SOURCE_TYPE = "sourceType";
const static std::string KEY_STATUS = "status";

const static std::string AUDIO_ONLY = "audioOnly";
const static std::string VIDEO_ONLY = "videoOnly";
const static std::string AUDIO_PLUS_VIDEO = "audioPlusVideo";

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            RemoteMediaSourceResource::RemoteMediaSourceResource() :
                    m_delegate(NULL)
            {
            }

            RemoteMediaSourceResource::~RemoteMediaSourceResource()
            {
            }

            MediaSourceType convertStringToMediaSourceType(std::string type)
            {
                MediaSourceType sourceType;
                if (type.compare(AUDIO_ONLY) == 0)
                {
                    sourceType = AUDIO;
                }
                else if (type.compare(VIDEO_ONLY) == 0)
                {
                    sourceType = VIDEO;
                }
                else if (type.compare(AUDIO_PLUS_VIDEO) == 0)
                {
                    sourceType = AUDIO_VIDEO;
                }
                else
                {
                    throw CommonException("Invalid MediaSourceType");
                }
                return sourceType;
            }

            std::string convertMediaSourceTypeToString(MediaSourceType type)
            {
                std::string sourceType;
                if (AUDIO == type)
                {
                    sourceType = AUDIO_ONLY;
                }
                else if (VIDEO == type)
                {
                    sourceType = VIDEO_ONLY;
                }
                else if (AUDIO_VIDEO == type)
                {
                    sourceType = AUDIO_PLUS_VIDEO;
                }
                else
                {
                   throw CommonException("Invalid MediaSourceType");
                }
                return sourceType;
            }

            std::list<MediaSource> convertBundleToMediaSource(std::list<PropertyBundle> &bundles)
            {
                if (0 == bundles.size())
                {
                    throw CommonException("bundles size is 0.");
                }

                std::list<PropertyBundle>::iterator iter;
                std::list<MediaSource> inputSources;
                for (iter = bundles.begin(); iter != bundles.end(); iter++)
                {
                    PropertyBundle bundle = (*iter);
                    MediaSource source;
                    bundle.getValue(KEY_SOURCE_NAME, source.name);
                    bundle.getValue(KEY_SOURCE_NUMBER, source.number);
                    bundle.getValue(KEY_STATUS, source.status);

                    std::string sourceType;
                    bundle.getValue(KEY_SOURCE_TYPE, sourceType);
                    source.type = convertStringToMediaSourceType(sourceType);

                    inputSources.push_back(source);
                }

                return inputSources;
            }

            PropertyBundle convertMediaSourceToBundle(const MediaSource& source)
            {
                PropertyBundle bundle;
                std::string type = convertMediaSourceTypeToString(source.type);
                bundle.setValue(KEY_SOURCE_TYPE, type);
                bundle.setValue(KEY_SOURCE_NAME, source.name);
                bundle.setValue(KEY_SOURCE_NUMBER, source.number);
                bundle.setValue(KEY_STATUS, source.status);
                return bundle;
            }

            void RemoteMediaSourceResource::setDelegate(
                                            RemoteMediaSourceResourceDelegate *delegate)
            {
                OIC_LOG(DEBUG, TAG, "Entered setDelegate");
                SHBaseRemoteResource::setDelegate(this);
                this->m_delegate = delegate;
            }

            void RemoteMediaSourceResource::getMediaSources()
            {
                OIC_LOG(DEBUG, TAG, "Entered getMediaSources");
                getPropertyBundle();
            }

            void RemoteMediaSourceResource::changeMediaSources(std::list<MediaSource> sources)
            {
                OIC_LOG(DEBUG, TAG, "Entered changeMediaSources");
                PropertyBundle bundle;
                bundle.setValue(KEY_SOURCES, sources);
                setPropertyBundle(bundle);
            }

            void RemoteMediaSourceResource::onGet(PropertyBundle bundle, ResultCode ret)
            {
                OIC_LOG(DEBUG, TAG, "Entered onGet");

                if (!m_delegate)
                {
                    OIC_LOG(DEBUG, TAG, "m_delegate is NULL");
                    return;
                }

                if (bundle.contains(KEY_SOURCES))
                {
                    std::list<PropertyBundle> receivedSources;
                    bundle.getValue(KEY_SOURCES, receivedSources);
                    std::list<MediaSource> sources = convertBundleToMediaSource(receivedSources);
                    m_delegate->onGetMediaSource(sources);
                }
            }

            void RemoteMediaSourceResource::onSet(PropertyBundle bundle, ResultCode ret)
            {
                OIC_LOG(DEBUG, TAG, "Entered onSet");

                if (!m_delegate)
                {
                    OIC_LOG(DEBUG, TAG, "m_delegate is NULL");
                    return;
                }

                if (bundle.contains(KEY_SOURCES))
                {
                    std::list<PropertyBundle> receivedSources;
                    bundle.getValue(KEY_SOURCES, receivedSources);
                    std::list<MediaSource> sources = convertBundleToMediaSource(receivedSources);
                    m_delegate->onChangeMediaSource(sources);
                }
            }
        }
    }
}
