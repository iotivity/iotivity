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
 * This file provides C++ Wrapper APIs of Notification Service for Consumer.
 */

#ifndef _NS_CONSUMER_SERVICE_H_
#define _NS_CONSUMER_SERVICE_H_

#include <list>
#include <algorithm>
#include "NSProvider.h"
#include "NSUtils.h"


namespace OIC
{
    namespace Service
    {
        /**
         * @class   NSConsumerService
         * @brief   This class provides a set of C++APIs for Notification Consumer.
         */
        class NSConsumerService
        {

            public :
                /**
                     * Consumer uses this callback function to receive the discovered providers
                     * @param[in] provider        Provider who has the notification resource
                     */
                typedef void (*ProviderDiscoveredCallback)(NSProvider *);

                /**
                     * Invoked when the provider state is changed
                     * @param[in] provider  Provider which has the notification resource
                     * @param[in] response  Response which has the provider state
                     */
                typedef void (* ProviderChangedCallback)(NSProvider *, NSResponse);

                /**
                     * @struct   ConsumerConfig
                     * @brief Consumer sets this callback function configuration for registering callbacks
                     *
                     */
                typedef struct
                {
                    /** m_discoverCb - ProviderDiscoveredCallback callback listener.*/
                    ProviderDiscoveredCallback m_discoverCb;
                    /** m_changedCb - ProviderChangedCallback callback listener.*/
                    ProviderChangedCallback m_changedCb;
                } ConsumerConfig;

                /**
                      * API for getting the Instance of NSConsumerService class
                      *
                      * @return Instance of the "NSConsumerService" class
                      */
                static NSConsumerService *getInstance();

                /**
                      * Initialize notification service for consumer
                      * @param[in]  config   ConsumerConfig Callback function pointers to onDiscoverd,
                      * OnAccepted, OnMessageReceived and OnSyncInfoReceived functions
                      */
                void Start(ConsumerConfig config);

                /**
                      * Terminate notification service for consumer
                      */
                void Stop();

                /**
                     * Request to discover to remote address as parameter.
                     * @param[in]  server address combined with IP address and port number using delimiter :
                     * @return  NSResult code of Consumer Service
                     */
                NSResult EnableRemoteService(const std::string &serverAddress);

                /**
                      * Request discovery manually
                      */
                void RescanProvider();

                /**
                      *  request to get NSProvider pointer
                      * @param id -id as string
                      *
                      * @return pointer to NSProvider
                      */
                NSProvider *getProvider(const std::string &id);


                /**
                     * Request NSMessage that is matched by message id
                     * @param[in]  messageId  the id of message that user wants to get
                     * @return NSMessage
                     */
                NSMessage *getMessage(uint64_t messageId);

                /**
                      *  get consumer config values
                      * @return ConsumerConfig callbaks set
                      */
                ConsumerConfig getConsumerConfig();

                /**
                      *  get list of providers acceted.
                      * @return m_acceptedProviders -list of accepted providers
                      */
                std::list<NSProvider *> getAcceptedProviders();

            private :
                ConsumerConfig m_config;
                std::list<NSProvider *> m_acceptedProviders;

            private :
                NSConsumerService();
                ~NSConsumerService();
                NSConsumerService(const NSConsumerService & ) = delete;
                NSConsumerService &operator=(const NSConsumerService & ) = delete;
                NSConsumerService(const NSConsumerService &&) = delete;
                NSConsumerService &operator=(const NSConsumerService && ) = delete;

        };
    }
}

#endif //_NS_CONSUMER_SERVICE_H_
