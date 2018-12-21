/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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
 * @file
 *
 * This file contains Notification service Utils.
 */

#ifndef _NS_UTILS_H_
#define _NS_UTILS_H_

/** NS_UTILS_UUID_STRING_SIZE - UUID string size*/
#define NS_UTILS_UUID_STRING_SIZE 37

namespace OIC
{
    namespace Service
    {
        /** NSResult - enumeration for NS service Result*/
        enum class NSResult
        {
            OK = 100,      /**< success result */
            ERROR = 200,   /**< error result */
            SUCCESS = 300, /**< success result*/
            FAIL = 400,    /**< failure result */
        };

        /**
         * NSProviderState of notification service
         */
        enum class NSProviderState
        {
            ALLOW = 1,   /**< allow state */
            DENY = 2,    /**< deny state */
            TOPIC = 3,   /**< topic */
            STOPPED = 12 /**< stopped state */
        };

        /**
         * NSProviderSubscribedState of notification service
         */
        enum class NSProviderSubscribedState
        {
            DENY = 0,       /**< deny state */
            DISCOVERED = 1, /**< discovered state */
            SUBSCRIBED = 2  /**< subscribed state */
        };
    }
}
#endif /* _NS_UTILS_H_ */
