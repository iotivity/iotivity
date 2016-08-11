//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/**
 * @file
 *
 * This file contains Notification service Provider representation.
 */

#ifndef _NS_PROVIDER_H_
#define _NS_PROVIDER_H_


#include <string>
#include <cstring>
#include "oic_string.h"
#include "NSSyncInfo.h"
#include "NSConstants.h"

namespace OIC
{
    namespace Service
    {
        class NSMessage;
        class NSSyncInfo;
        /**
         * @class   NSProvider
         * @brief   This class provides a set of Notification service Provider representation APIs.
         */
        class NSProvider
        {
            public:
                /**
                     * Consumer use this callback function to receive notification message from provider
                     * synchronization
                     * @param[in] message     Notification message
                     */
                typedef void (*MessageReceivedCallback)(NSMessage *);

                /**
                     * Provider and consumer use this callback function to receive the status of the message
                     * synchronization
                     * @param[in] sync        Synchronization information of the notification message
                     */
                typedef void (*SyncInfoReceivedCallback)(NSSyncInfo *);

                /**
                      * Constructor of NSProvider.
                      */
                NSProvider(): m_messageCb(NULL), m_syncInfoCb(NULL) {}

                /**
                      * Constructor of NSProvider.
                      *
                      * @param providerId - providerId of the Notification.
                      */
                NSProvider(const std::string &providerId) : m_providerId(providerId),
                    m_messageCb(NULL), m_syncInfoCb(NULL) {}

                /**
                      * Constructor of NSProvider.
                      *
                      * @param provider - of type NSProvider.
                      */
                NSProvider(::NSProvider *provider);


                /**
                      * Destructor of NSProvider.
                      */
                ~NSProvider() = default;

                /**
                      * This method is for getting ProviderId from the Notification service provider.
                      *
                      * @return ProviderId as string.
                      */
                std::string getProviderId() const;

                /**
                      * This method is for requesting subscription of Notification service.
                      *
                      */
                void subscribe();

                /**
                      * This method is for requesting unsubscription of Notification service.
                      *
                      */
                void unSubscribe();

                /**
                      * This method is for Sending SyncInfo of Notification service.
                      *
                      * @param messageId - id of type message.
                      * @param type - NSSyncType of Notification service.
                      */
                void SendSyncInfo(uint64_t messageId, NSSyncInfo::NSSyncType type);

                /**
                      * This method is for registering for listeners of Notification .
                      *
                      * @param messageHandle - MessageReceivedCallback callback.
                      * @param syncHandle - SyncInfoReceivedCallback callback
                      */
                void setListener(MessageReceivedCallback messageHandle,
                                 SyncInfoReceivedCallback syncHandle);

                /**
                      * This method is for getting the registered cb of Notification message received.
                      *
                      * @return messageCb - MessageReceivedCallback .
                      */
                MessageReceivedCallback getMessageReceivedCb();

                /**
                      * This method is for getting the registered cb of Notification SyncInfo received.
                      *
                      * @return syncInfoCb - SyncInfoReceivedCallback .
                      */
                SyncInfoReceivedCallback getSyncInfoReceivedCb();

            private:
                ::NSProvider *getNSProvider();

            private:
                std::string m_providerId;
                MessageReceivedCallback m_messageCb;
                SyncInfoReceivedCallback m_syncInfoCb;

        };
    }
}
#endif /* _NS_PROVIDER_H_ */
