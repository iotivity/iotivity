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
 * This file provides C++ Wrapper APIs of Notification Service for Provider.
 */

#ifndef _NS_PROVIDER_SERVICE_H_
#define _NS_PROVIDER_SERVICE_H_

#include "NSCommon.h"
#include "NSProviderInterface.h"
#include "NSMessage.h"
#include "oic_string.h"
#include "NSConsumer.h"
#include "NSSyncInfo.h"
#include "NSConstants.h"
#include <cstring>

namespace OIC
{
    namespace Service
    {
        /**
         * @class   NSProviderService
         * @brief   This class provides a set of C++APIs for Notification Provider.
         */
        class NSProviderService
        {

            public:
                /**
                      * Provider uses this callback function to receive subscription request of consumer
                      * @param[in] consumer        Consumer who subscribes the resource
                      */
                typedef void (*ConsumerSubscribedCallback)(NSConsumer *);

                /**
                      * Provider use this callback function to receive the status of the message
                      * synchronization
                      * @param[in] sync        Synchronization information of the notification message
                      */
                typedef void (*MessageSynchronizedCallback)(NSSyncInfo *);


                /**
                      * @struct   ProviderConfig
                      * @brief Provider sets this callback function configuration for registering callbacks
                      *
                      */
                typedef struct
                {
                    /** m_subscribeRequestCb - ConsumerSubscribedCallback callback listener.*/
                    ConsumerSubscribedCallback m_subscribeRequestCb;
                    /** m_syncInfoCb - MessageSynchronizedCallback callback listener.*/
                    MessageSynchronizedCallback m_syncInfoCb;
                } ProviderConfig;

                /**
                     * Access policy exchanged between provider and consumer during subscription process
                     */
                enum class NSAccessPolicy
                {
                    NS_ACCESS_ALLOW = 0,
                    NS_ACCESS_DENY = 1,
                };

                /**
                      * API for starting the NS Provider
                      *
                      * @return NSProviderService Pointer to singleton instance created
                      */
                static NSProviderService *getInstance();

                /**
                      * Initialize notification service for provider
                      * @param[in]  policy   Accepter
                      * @param[in]  config   ProviderConfig Callback function pointers to onConsumerSubscribed,
                      * and onMessageSynchronized function listeners
                      * @return :: result code of NSResult
                      */
                NSResult Start(NSAccessPolicy policy, ProviderConfig config);

                /**
                      * Terminate notification service for provider
                      * @return :: result code of NSResult
                      */
                NSResult Stop();

                /**
                      * Request to publish resource to cloud server
                      * @param[in]  server address combined with IP address and port number using delimiter :
                      * @return  result code of NSResult
                      */
                NSResult EnableRemoteService(const std::string &serverAddress);

                /**
                      * Request to cancel remote service using cloud server
                      * @param[in]  server address combined with IP address and port number using delimiter :
                      * @return  result code of NSResult
                      */
                NSResult DisableRemoteService(const std::string &serverAddress);

                /**
                      * Send notification message to all subscribers
                      * @param[in]  msg  Notification message including id, title, contentText
                      * @return :: result code of NSResult
                      */
                NSResult SendMessage(NSMessage *msg);


                /**
                      * Send read-check to provider in order to synchronize notification status with other consumers
                      * @param[in]  messageId  Notification message to synchronize the status
                      * @param[in]  type  NotificationSyncType of the SyncInfo message
                      * @return :: result code of NSResult
                      */
                void SendSyncInfo(uint64_t messageId, NSSyncInfo::NSSyncType type);

                /**
                      *  get Provider config values
                      * @return ProviderConfig callbaks set
                      */
                ProviderConfig getProviderConfig();

            private :
                ProviderConfig m_config;

            private:
                NSProviderService()
                {
                    m_config.m_subscribeRequestCb = NULL;
                    m_config.m_syncInfoCb = NULL;
                }
                ~NSProviderService() = default;
                NSProviderService(const NSProviderService &) = delete;
                NSProviderService &operator=(const NSProviderService &) = delete;
                NSProviderService(const NSProviderService &&) = delete;
                NSProviderService &operator=(const NSProviderService && ) = delete;

                ::NSMessage *getNSMessage(NSMessage *msg);
        };
    }
}
#endif /* _NS_PROVIDER_SERVICE_H_ */
