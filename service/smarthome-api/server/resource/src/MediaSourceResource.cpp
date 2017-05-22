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
#include <MediaSourceResource.h>
#include <PropertyBundle.h>
#include <ResourceQuery.h>
#include "logger.h"

#define TAG "SH_SERVER_MEDIASOURCE_RESOURCE"

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
            MediaSourceResource::MediaSourceResource(std::string uri) :
                    m_delegate(NULL), SHBaseResource(uri, RESOURCE_TYPE::MEDIA_INPUT,
                        RESOURCE_PROPERTY::DISCOVERABLE | RESOURCE_PROPERTY::OBSERVABLE)
            {
            }

            MediaSourceResource::~MediaSourceResource()
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

            std::list<MediaSource> MediaSourceResource::getMediaSources() const
            {
                OIC_LOG(DEBUG, TAG, "Entered getMediaSources");

                PropertyBundle storedBundle = getPropertyBundle();
                std::list<PropertyBundle> bundles;
                storedBundle.getValue(KEY_SOURCES, bundles);
                return convertBundleToMediaSource(bundles);
            }

            bool MediaSourceResource::addMediaSource(const MediaSource& source)
            {
                OIC_LOG(DEBUG, TAG, "Entered addMediaSource");

                PropertyBundle storedBundle = getPropertyBundle();
                std::list<PropertyBundle> bundles;
                storedBundle.getValue(KEY_SOURCES, bundles);

                std::list<PropertyBundle>::iterator iter;
                for (iter = bundles.begin(); iter != bundles.end(); iter++)
                {
                    PropertyBundle bundle = (*iter);
                    int number;
                    bundle.getValue(KEY_SOURCE_NUMBER, number);

                    if (number == source.number)
                    {
                        OIC_LOG(DEBUG, TAG, "Found duplicated sourceNumber");
                        return false;
                    }
                }

                bundles.push_back(convertMediaSourceToBundle(source));
                storedBundle.setValue(KEY_SOURCES, bundles);
                setPropertyBundle(storedBundle);
                return true;
            }

            bool MediaSourceResource::removeMediaSource(const MediaSource& source)
            {
                OIC_LOG(DEBUG, TAG, "Entered removeMediaSource");

                PropertyBundle storedBundle = getPropertyBundle();
                std::list<PropertyBundle> bundles;
                storedBundle.getValue(KEY_SOURCES, bundles);

                std::list<PropertyBundle>::iterator iter;
                for (iter = bundles.begin(); iter != bundles.end(); iter++)
                {
                    PropertyBundle bundle = (*iter);
                    int number;
                    bundle.getValue(KEY_SOURCE_NUMBER, number);

                    if (number == source.number)
                    {
                        OIC_LOG(DEBUG, TAG, "Found MediaSource");
                        bundles.erase(iter);
                        storedBundle.setValue(KEY_SOURCES, bundles);
                        setPropertyBundle(storedBundle);
                        return true;
                    }
                }
                return false;
            }

            bool MediaSourceResource::updateMediaSource(const MediaSource& source)
            {
                OIC_LOG(DEBUG, TAG, "Entered updateMediaSource");

                PropertyBundle storedBundle = getPropertyBundle();
                std::list<PropertyBundle> bundles;
                storedBundle.getValue(KEY_SOURCES, bundles);

                std::list<PropertyBundle>::iterator iter;
                for (iter = bundles.begin(); iter != bundles.end(); iter++)
                {
                    PropertyBundle bundle = (*iter);
                    int number;
                    bundle.getValue(KEY_SOURCE_NUMBER, number);

                    if (number == source.number)
                    {
                        OIC_LOG(DEBUG, TAG, "Found MediaSource");
                        bundles.erase(iter);
                        bundles.push_back(convertMediaSourceToBundle(source));
                        storedBundle.setValue(KEY_SOURCES, bundles);
                        setPropertyBundle(storedBundle);
                        return true;
                    }
                }
                return false;
            }

            void MediaSourceResource::setDelegate(MediaSourceResourceDelegate *delegate)
            {
                OIC_LOG(DEBUG, TAG, "Entered setDelegate");
                this->m_delegate = delegate;
                SHBaseResource::setDelegate(this);
            }

            ResultCode MediaSourceResource::onGet(RequestId requestId, const ResourceQuery& query)
            {
                OIC_LOG(DEBUG, TAG, "Entered onGet");
                return SUCCESS;
            }

            ResultCode MediaSourceResource::onSet(RequestId requestId, const PropertyBundle& bundle,
                                                  const ResourceQuery& query)
            {
                OIC_LOG(DEBUG, TAG, "Entered onSet");

                if (!this->m_delegate)
                {
                    OIC_LOG(DEBUG, TAG, "m_delegate is NULL");
                    return FAIL;
                }

                if (!bundle.contains(KEY_SOURCES))
                {
                    OIC_LOG(ERROR, TAG, "sources is mandatory property");
                    return FAIL;
                }

                std::list<PropertyBundle> receivedSources;
                bundle.getValue(KEY_SOURCES, receivedSources);
                // Call application callback.
                std::list<MediaSource> sources = convertBundleToMediaSource(receivedSources);
                ResultCode retCode = this->m_delegate->onChangeMediaSources(sources);
                if (retCode == SUCCESS)
                {
                    // Update property with the requested value.
                    std::list<MediaSource>::iterator iter;
                    for (iter = sources.begin(); iter != sources.end(); iter++)
                    {
                        MediaSourceResource::updateMediaSource(*iter);
                    }
                }
                return retCode;
            }
        }
    }
}
