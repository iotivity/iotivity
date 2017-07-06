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
 * This file contains the declaration of classes and its members related to MediaResource.
 */

#ifndef SMARTHOME_API_SERVER_MEDIA_H_
#define SMARTHOME_API_SERVER_MEDIA_H_

#include <SHBaseResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /**
             * Data structure for media elements.
             *  - url  Url for the media instance.
             *  - sdp  Array of strings, one per SDP line.
             *         SDP specification: http://tools.ietf.org/html/rfc4566
             */
            typedef struct
            {
                std::string url;
                std::list<std::string> sdp;
            }MediaInfo;

            /**
             * This class contains a set of callback functions handling events and requests
             * for MediaResource class
             */
            class MediaResourceDelegate
            {
            public:
                /**
                 * Virtual destructor
                 */
                virtual ~MediaResourceDelegate() {}
            };

            /**
             * This class contains a set of functions to change the properties of
             * Media
             */
            class MediaResource: public SHBaseResource, public SHBaseResourceDelegate
            {
            public:
                MediaResource();
                virtual ~MediaResource();

                /**
                 * API to get stored media informations.
                 *
                 * @return :: std::list<MediaInfo>  list of medif informations.
                 */
                std::list<MediaInfo> getMediaInfoList() const;

                /**
                 * API to add media information.
                 *
                 * @param[in] MediaInfo media information to be added.
                 * @return :: true if add media information suceess.
                 */
                void addMediaInfo(MediaInfo mediaInfo);

                /**
                 * API to set delegate of MediaResource
                 *
                 * @param[in] MediaDelegate Instance that inherit MediaResourceDelegate
                 * class
                 */
                void setMediaResourceDelegate(MediaResourceDelegate *mediaDelegate);

            private:
                virtual ResultCode onGet(RequestId requestId, const ResourceQuery &query);
                virtual ResultCode onSet(RequestId requestId, const PropertyBundle &bundle,
                                         const ResourceQuery &query);

                MediaResourceDelegate *m_userDelegate;  //TODO need??
            };
        }
    }
}

#endif /* SMARTHOME_API_SERVER_MEDIA_H_ */
