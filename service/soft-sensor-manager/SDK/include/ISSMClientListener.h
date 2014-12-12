/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
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
 * @file    ISSMClientListener.h
 * @brief   This file contains client listener interfaces.
 */

#ifndef ISSMCLIENTLISTENER_H_
#define ISSMCLIENTLISTENER_H_
#include <string>
#include "OCPlatform.h"

/**
 * @brief This enum defines Soft Sensor return types
 */
typedef enum
{
    SSM_SUCCESS, SSM_ERROR, SSM_ERROR_QUERY_PARSING, SSM_ERROR_NO_QUERY, SSM_ERROR_NETWORK
} SSMReturn;

using namespace OC;
/**
 *  @brief  ISSMClientListener is a listener interface from which application is derived to get callback from SoftSensorManager service
 */
class ISSMClientListener
{
    public:
        /**
         * @brief   onRegisterQuery is a pure virtual operation which should be implemented in applications to get callback messages.
         * @param [in] jsonData - A data map in which SoftSensorManager service sends sensor data with cqid.
         * @param [in] eCode - The address of listener class.  When an application which inherits the ISSMClientListener calls this operation, it sends its address for the listener so that
         *                          SSMClient can callback message to the appication.
         * @param [out] cqid - A query id generated from SoftSensorManager service for the queryString request.
         * @return  SSMReturn

         */
        virtual void onRegisterQuery(const std::string &jsonData, SSMReturn &eCode) = 0;
        virtual ~ISSMClientListener()
        {
        }
};

#endif /* ISSMCLIENTLISTENER_H_ */
