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
#include "NSSyncInfo.h"
#include "NSMessage.h"
#include "NSUtils.h"
#include "NSTopicsList.h"

namespace OIC
{
    namespace Service
    {
        /**
         * @class   NSProvider
         * @brief   This class provides a set of Notification service Provider representation APIs.
         */
        class NSProvider
        {
            public:

                /**
                     * Invoked when the provider state is changed
                     * @param[in] providerState  NSProviderState of the notification resource
                     */
                typedef void (* ProviderStateCallback)(NSProviderState);

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
                NSProvider(): m_topicList(new NSTopicsList()), m_state(NSProviderState::DENY),
                    m_subscribedState(NSProviderSubscribedState::DENY),
                    m_stateCb(NULL), m_messageCb(NULL), m_syncInfoCb(NULL)
                {
                }

                /**
                      * Constructor of NSProvider.
                      *
                      * @param providerId - providerId of the Notification.
                      */
                NSProvider(const std::string &providerId) : m_providerId(providerId),
                    m_topicList(new NSTopicsList()), m_state(NSProviderState::DENY),
                    m_subscribedState(NSProviderSubscribedState::DENY),
                    m_stateCb(NULL), m_messageCb(NULL), m_syncInfoCb(NULL)
                {
                }

                /**
                      * Constructor of NSProvider.
                      *
                      * @param providerId - providerId of the Notification.
                      * @param topicList - NSTopicsList of interested Topics.
                      */
                NSProvider(const std::string &providerId, NSTopicsList *topicList) : m_providerId(
                        providerId), m_topicList(topicList), m_state(NSProviderState::DENY),
                    m_subscribedState(NSProviderSubscribedState::DENY),
                    m_stateCb(NULL), m_messageCb(NULL), m_syncInfoCb(NULL)
                {
                }

                /**
                      * Constructor of NSProvider.
                      *
                      * @param provider - of type NSProvider.
                      */
                NSProvider(::NSProvider *provider);

                /**
                     * Copy Constructor of NSProvider.
                     *
                     * @param provider - NSProvider to initialize.
                     */
                NSProvider(const NSProvider &provider);

                /**
                     * Copy assignment operator of NSProvider.
                     *
                     * @param provider -  NSProvider to initialize.
                     * @return NSProvider object reference
                     */
                NSProvider &operator=(const NSProvider &provider);

                /**
                      * Destructor of NSProvider.
                      */
                ~NSProvider();

                /**
                      * This method is for getting ProviderId from the Notification service provider.
                      *
                      * @return ProviderId as string.
                      */
                std::string getProviderId() const;

                /**
                      * This method is for getting NSTopic List from the Notification service provider.
                      *
                      * @return NSTopicsList  as pointer.
                      */
                NSTopicsList *getTopicList() const;

                /**
                     * Update Topic list that is wanted to be subscribed from provider
                     *
                     * @param topicList - NSTopicsList of interested Topics.
                     * @return NSResult
                     */
                NSResult updateTopicList(NSTopicsList *topicList);

                /**
                      * This method is for getting ProviderState from the Notification service provider.
                      *
                      * @return ProviderState as NSProviderState.
                      */
                NSProviderState getProviderState() const;

                /**
                      * This method is for getting SubscribedState from the Notification service provider.
                      *
                      * @return subscribedState as NSProviderSubscribedState.
                      */
                NSProviderSubscribedState getProviderSubscribedState() const;

                /**
                      * This method is for requesting subscription of Notification service.
                      *
                      */
                void subscribe();

                /**
                      * This method is for requesting subscription status from Provider of Notification service.
                      *
                      */
                bool isSubscribed();

                /**
                      * This method is for Sending SyncInfo of Notification service.
                      *
                      * @param messageId - id of type message.
                      * @param type - NSSyncType of Notification service.
                      */
                void sendSyncInfo(uint64_t messageId, NSSyncInfo::NSSyncType type);

                /**
                      * This method is for registering for listeners of Notification .
                      *
                      * @param stateHandle - ProviderStateCallback callback.
                      * @param messageHandle - MessageReceivedCallback callback.
                      * @param syncHandle - SyncInfoReceivedCallback callback
                      */
                void setListener(ProviderStateCallback stateHandle,
                                 MessageReceivedCallback messageHandle,
                                 SyncInfoReceivedCallback syncHandle);

                /**
                      * This method is for getting the registered cb of Provider State received.
                      *
                      * @return stateCb - ProviderStateCallback .
                      */
                ProviderStateCallback getProviderStateReceivedCb() const;

                /**
                      * This method is for getting the registered cb of Notification message received.
                      *
                      * @return messageCb - MessageReceivedCallback .
                      */
                MessageReceivedCallback getMessageReceivedCb() const;

                /**
                      * This method is for getting the registered cb of Notification SyncInfo received.
                      *
                      * @return syncInfoCb - SyncInfoReceivedCallback .
                      */
                SyncInfoReceivedCallback getSyncInfoReceivedCb() const;

                /**
                      * This method is for setting NSTopic List for the Notification service provider.
                      *
                      * @param topicsList  as NSTopicsList pointer.
                      */
                void setTopicList(NSTopicsList *topicsList);

                /**
                     * This method is for setting ProviderState for the Notification service provider.
                     *
                     * @param providerState as NSProviderState.
                     */
                void setProviderState(const NSProviderState &providerState);

                /**
                     * This method is for setting subscribedState for the Notification service provider.
                     *
                     * @param subscribedState as NSProviderSubscribedState.
                     */
                void setProviderSubscribedState(const NSProviderSubscribedState &subscribedState);

            private:
                ::NSProvider *getNSProvider();

            private:
                std::string m_providerId;
                NSTopicsList *m_topicList;
                NSProviderState m_state;
                NSProviderSubscribedState m_subscribedState;

                ProviderStateCallback m_stateCb;
                MessageReceivedCallback m_messageCb;
                SyncInfoReceivedCallback m_syncInfoCb;
        };
    }
}
#endif /* _NS_PROVIDER_H_ */
