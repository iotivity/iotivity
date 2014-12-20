//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

#ifndef CDSERVER_H_
#define CDSERVER_H_

#include "CDSerializableDataFactory.h"
#include "CDCoreResourceHandlerFactory.h"
#include "CDServerListener.h"
#include "IContext.h"
#include "OCPlatform.h"
#include "Configuration.h"

#define OIC_PAYLOAD_TYPE "application/json; charset=utf-8"

namespace OC
{

    /**
     * @class   CDServer
     * @brief   This class is the core class for communication with underlying layers<bridge class>.
     */
    class CDServer: CDServerListener
    {

        public:
            /**
             * CDServer - Parameterized constructor
             *
             * @param -m_ocPlatform - A pointer to the OC platform instance.
             * @param - p_context - A context object to this instance of the run.
             * @return -none
             */
            CDServer(OC::Cm::IContext *p_context);

            /**
             * ~CDServer - Parameterized constructor
             *
             * @param -none
             * @return -none
             */
            virtual ~CDServer();

            /**
             * initialize - Initialize the object. This has to be done before using it
             *
             * @param -configuration - The framework configuration.
             * @return - type:bool - true if the initialize succeeded, else false.
             */
            bool initialize(OC::Cm::Configuration *configuration);

            /**
             * initialize - De-initialize the object. Do this before the object is deleted
             *                  or goes out of scope.
             * @param -none
             * @return - type:bool - true if the de-initialize succeeded, else false.
             */
            bool deinit();

            /**
             * start - Here for backward compatibility. Deprecated - DONOT USE
             *
             * @param -none
             * @return - type:bool - true if start succeeded, else false.
             */
            bool start();

            /**
             * stop - Here for backward compatibility. Deprecated - DONOT USE
             *
             * @param -none
             * @return - type:bool - true if stop succeeded, else false.
             */
            bool stop();

            /**
             * handleOCRequest - This is currently the call back entry from OC to SHP
             *                      DONOT call or tinker with this.
             *
             * @param - request - Input request
             * @param - response - Response as output.
             * @return - type:bool - true if stop succeeded, else false.
             */
            static OCEntityHandlerResult handleOCRequest(
                const std::shared_ptr<OC::OCResourceRequest> request /**,
            std::shared_ptr<OC::OCResourceResponse> response**/);

            /**
             * handleRequest - Ideally should be private helper. Close and make private
             *
             * @param - ocRequest - Input request
             * @param - ocResponse - Response as output.
             * @return - type:bool - true if stop succeeded, else false.
             */
            void handleRequest(const std::shared_ptr<OC::OCResourceRequest> ocRequest,
                               std::shared_ptr<OC::OCResourceResponse> ocResponse);
            OC::Cm::Serialization::SerializableDataFactory *getSerializableDataFactory();

            /** sendClientRequest - Send message to a remote client
             *
             * @param - method - Type of request . eg. GET, PUT.
             * @param - ocResponse - Response as output.
             * @return - type:bool - true if stop succeeded, else false.
             */
            void sendClientRequest(std::string method, std::string deviceAddress,
                                   std::string relativeURI,
                                   OC::Cm::Serialization::ISerializable *data);

            /** onResponse -Placeholder to keep the sendResponse satisfied
             *
             * @param - headerOptions - Options in the response.
             * @param - attributeMap - Attribute map tht would be received in the response
             * @param - eCode - Error code if any.
             * @return -none
             */
            static void onResponse(const HeaderOptions &headerOptions,
                                   const OC::OCRepresentation &attributeMap, const int eCode);

            /** onOCDelete -Placeholder to keep the sendResponse satisfied
             *
             * @param - headerOptions - Options in the response.
             * @param - eCode - Error code if any.
             * @return -none
             */
            static void onOCDelete(const HeaderOptions &headerOptions, const int eCode);

        private:

            /** registerOCResource - Helper class to register resources
             *
             * @param - resourceURI - The URI to assign to the resource being registered
             * @param - resourceName - Name assigned to the resource.
             * @param - isDiscoverable - If the device is discoverable or not.
             * @return - type:bool - true if stop succeeded, else false.
             */
            bool registerOCResource(std::string resourceURI, std::string resourceName,
                                    bool isDiscoverable);

            /** registerOCResource - Helper class to register resources.
             *                          A default capabilty resource is registered
             *
             * @return - type:bool - true if stop succeeded, else false.
             */
            bool registerOCResources();

            /** serialize - Helper class to convert from OC Representation to ISerializable
             *
             * @param - data - The data in ISerializable form (input)
             * @param - method - The type of request
             * @param - attr - Map of the attributes
             * @param - payLoad - Payload (output)
             *
             * @param - payloadLen - Length of the payload
             * @param - errorMessage - Error message if any
             *
             * @return - type:bool - true if serialize succeeded, else false
             */
            bool serialize(OC::Cm::Serialization::ISerializable *data,
                           std::string &method, std::map<std::string, std::string> *attr,
                           std::string &contentType, std::string &payLoad, int &payloadLen,
                           std::string &prettyPrintOutput, std::string &errorMessage);

            /** deSerialize - Helper class to convert from  OC Representation to ISerializable
             *
             * @param - contentType - The type to deserialize from
             * @param - payLoad - Payload input in JSON(OC Rep for now)
             * @param - payloadLen - Length of the payload
             * @param - method - The type of request
             * @param - queryParams - Map of the attributes
             * @param - data - The data in ISerializable form (output)
             * @param - errorMessage - Error message if any
             * @return - type:bool - true if serialize succeeded, else false
             */

            int deSerialize(std::string &contentType, std::string &payLoad,
                            int payloadLen, std::string &method,
                            std::map<std::string, std::string> &queryParams,
                            OC::Cm::Serialization::ISerializable *data,
                            std::string &errorMessage);

            /** simple_deserialize -A wrapper over deSerialize
             *
             * @param - ocRequest - The tincoming request
             * @param - ocRequest - SHP representation(ISerializable)
             * @return - type:int- error code. 0 if succeess.
             */
            int simple_deserialize(
                const std::shared_ptr<OC::OCResourceRequest> &ocRequest,
                OC::Cm::Serialization::ISerializable *m_pRequestPayload);

            static OC::Cm::IContext *_context;/**< Context of the this device.*/
            CDSerializableDataFactory *_serializableDataFactory;/**< The data factory for serialization*/
            CDCoreResourceHandlerFactory *_resourceHandlerFactory;/**< The data factory for resource handler factory*/
            static CDServerListener *_listener;/**< The object to the parent class */

    };
}

#endif /* CDSERVER_H_ */
