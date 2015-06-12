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

/** @file   jni_things_manager_callbacks.h
  *
  *  @brief  This file contains the declaration of ThingsManagerCallbacks class
  *         and its members related to ThingsManagerCallbacks.
  */

#ifndef __JNI_THINGS_MANAGER_CALLBACKS_H_
#define __JNI_THINGS_MANAGER_CALLBACKS_H_

#include <string>

#include "ThingsManager.h"

/**
 * @class   ThingsManagerCallbacks
 * @brief   This class provides a set of callback functions for group management,
 *          synchronization of group, configuration of things, and diagnostics about things.
 *
 */
class ThingsManagerCallbacks
{

    public:
        ThingsManagerCallbacks() {}
        virtual ~ThingsManagerCallbacks() {}

        /**
         * This callback method is called when resources are discovered in network.
         *
         * @param resourceVector - List of resources discovered in the network
         *
         * @return void
         */
        static void onFoundCandidateResource(std::vector< std::shared_ptr<OC::OCResource > >
                                             resourceVector);

        /**
         * This callback method is called to notify whether group is found or not.
         *
         * @param resource - Resource URI
         *
         * @return void
         */
        static void onFoundGroup(std::shared_ptr<OC::OCResource> resource);

        /**
         * This callback method is called for child resource presence status.
         *
         * @param resource - URI of resource.
         * @param result - error code.
         *
         * @return void
         */
        static void onSubscribePresence(std::string resource, OCStackResult result);

        /**
         * This callback method is called when a response for the updateConfigurations request
         * just arrives.
         *
         * @param headerOptions - It comprises of optionID and optionData as members.
         * @param rep - Configuration parameters are carried as a pair of attribute key and value
         *                      in a form of OCRepresentation instance.
         * @param eCode - error code.
         *
         * @return void
         */
        static void onUpdateConfigurationsResponse(const OC::HeaderOptions &headerOptions,
                const OC::OCRepresentation &rep, const int eCode);

        /**
         * This callback method is called when a response for the getConfigurations request
         * just arrives.
         *
         * @param headerOptions - It comprises of optionID and optionData as members.
         * @param rep - Configuration parameters are carried as a pair of attribute key and value
         *                      in a form of OCRepresentation instance.
         * @param eCode - error code.
         *
         * @return void
         */
        static void onGetConfigurationsResponse(const OC::HeaderOptions &headerOptions,
                                                const OC::OCRepresentation &rep, const int eCode);

        /**
         * This callback method is called when a response for the doBootstrap request
         * just arrives.
         *
         * @param headerOptions - It comprises of optionID and optionData as members.
         * @param rep - Configuration parameters are carried as a pair of attribute key and value
         *                      in a form of OCRepresentation instance.
         * @param eCode - error code.
         *
         * @return void
         */
        static void onBootStrapResponse(const OC::HeaderOptions &headerOptions,
                                        const OC::OCRepresentation &rep,
                                        const int eCode);

        /**
         * This callback method is called when a response for the reboot request
         * just arrives.
         *
         * @param headerOptions - It comprises of optionID and optionData as members.
         * @param rep - Configuration parameters are carried as a pair of attribute key and value
         *                      in a form of OCRepresentation instance.
         * @param eCode - error code.
         *
         * @return void
         */
        static void onRebootResponse(const OC::HeaderOptions &headerOptions,
                                     const OC::OCRepresentation &rep,
                                     const int eCode);

        /**
         * This callback method is called when a response for the factoryReset request
         * just arrives.
         *
         * @param headerOptions - It comprises of optionID and optionData as members.
         * @param rep - Configuration parameters are carried as a pair of attribute key and value
         *                      in a form of OCRepresentation instance.
         * @param eCode - error code.
         *
         * @return void
         */
        static void onFactoryResetResponse(const OC::HeaderOptions &headerOptions,
                                           const OC::OCRepresentation &rep,
                                           const int eCode);

        /**
         * This callback method is called when a response for the executeActionSet
         * or deleteActionSet request just arrives.
         *
         * @param headerOptions - It comprises of optionID and optionData as members.
         * @param rep - Configuration parameters are carried as a pair of attribute key and value
         *                      in a form of OCRepresentation instance.
         * @param eCode - error code.
         *
         * @return void
         */
        static void onPostResponse(const OC::HeaderOptions &headerOptions,
                                   const OC::OCRepresentation &rep, const int eCode);

        /**
         * This callback method is called when a response for the addActionSet request
         * just arrives.
         *
         * @param headerOptions - It comprises of optionID and optionData as members.
         * @param rep - Configuration parameters are carried as a pair of attribute key and value
         *                      in a form of OCRepresentation instance.
         * @param eCode - error code.
         *
         * @return void
         */
        static void onPutResponse(const OC::HeaderOptions &headerOptions,
                                  const OC::OCRepresentation &rep, const int eCode);

        /**
         * This callback method is called when a response for the getActionSet request
         * just arrives.
         *
         * @param headerOptions - It comprises of optionID and optionData as members.
         * @param rep - Configuration parameters are carried as a pair of attribute key and value
         *                      in a form of OCRepresentation instance.
         * @param eCode - error code.
         *
         * @return void
         */
        static void onGetResponse(const OC::HeaderOptions &headerOptions,
                                  const OC::OCRepresentation &rep, const int eCode);

        /**
         * This method invokes the Callback function with particular name and signature.
         *
         * @param headerOptions - It comprises of optionID and optionData as members.
         * @param rep - Configuration parameters are carried as a pair of attribute key and value
         *                      in a form of OCRepresentation instance.
         * @param eCode - error code.
         * @param callbackName - callbackName to be invoked.
         * @param signature - Signature of the callback method to be called.
         *
         * @return void
         */
        static void invokeCallback(const OC::HeaderOptions &headerOptions,
                                   const OC::OCRepresentation &rep, const int eCode,
                                   const char *callbackName, const char *signature);

};
#endif  //__JNI_THINGS_MANAGER_CALLBACKS_H_