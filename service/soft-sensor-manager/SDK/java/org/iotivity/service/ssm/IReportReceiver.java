/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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
 * @file    IReportReceiver.java
 *
 * @brief    This file provides a class for a set of APIs to handle response on receiving
 * message
 *
 */

package org.iotivity.service.ssm;

/**
 * @class IReportReceiver
 * @brief This class provides an API to handle the report Message.
 *      The application should implement the pure virtual function OnMessageReceived.
 *
 */
public abstract class IReportReceiver
{
        /**
         * Handle response on receiving message
         *       This abstract method needs to be implemeted by application
         *        as a precondition for query engine implementation.
         *
         * @param tag - tag from SSMCore
         *
         * @param msg - received message
         *
         * @return void
         * 
         */
        public abstract void onMessageReceived(String tag, String msg);
}