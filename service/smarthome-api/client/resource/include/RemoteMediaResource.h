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
 * RemoteMediaResource.
 */

#ifndef SMARTHOME_API_CLIENT_REMOTEMEDIA_H_
#define SMARTHOME_API_CLIENT_REMOTEMEDIA_H_

#include <SHBaseRemoteResource.h>

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
             * This class contains a set of functions for callback of
             * RemoteMediaResource class.
             */
            class RemoteMediaResourceDelegate
            {
            public:
                /**
                 * Virtual destructor
                 */
                virtual ~RemoteMediaResourceDelegate() {}

                /**
                 * API to handle a response to get media resource.
                 *
                 * @param[in] mediaInfo  list of media informations.
                 * @param[in] ret        ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onGetMedia(std::list<MediaInfo> mediaInfo, ResultCode ret) = 0;
            };

            /**
             * This class contains a set of functions to control Media
             */
            class RemoteMediaResource: public SHBaseRemoteResource,
                    public SHBaseRemoteResourceDelegate
            {
            friend class SHBaseRemoteResourceBuilder;
            public:

                virtual ~RemoteMediaResource();

                /**
                 * API to set delegate of RemoteMediaResource
                 *
                 * @param[in] delegate  Instance that inherit RemoteMediaResourceDelegate
                 * class
                 */
                void setRemoteMediaResourceDelegate(
                        RemoteMediaResourceDelegate *delegate);


                /**
                 * API to get the media informations.
                 */
                void getMedia();

            protected:
                RemoteMediaResource();

            private:
                virtual void onGet(PropertyBundle bundle, ResultCode ret);
                virtual void onSet(PropertyBundle bundle, ResultCode ret);
                virtual void onObserve(PropertyBundle bundle, const ObserveResponse obsType,
                        ResultCode ret);

            private:
                RemoteMediaResourceDelegate *m_delegate;
            };
        }
    }
}

#endif /* SMARTHOME_API_CLIENT_REMOTEMEDIA_H_ */
