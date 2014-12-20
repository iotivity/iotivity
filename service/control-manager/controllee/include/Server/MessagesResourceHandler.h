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

#ifndef MESSAGESRESOURCEHANDLER_H_
#define MESSAGESRESOURCEHANDLER_H_

#include "IContext.h"
#include "ISerializable.h"
#include "SyncResourceHandler.h"
#include "Elements.h"
/**
 * @class   MessagesResourceHandler
 * @brief   Defines methods required to handle requests received
 *
 * This class defines methods to handle requests received corresponding to a Messages resource.
 */
class MessagesResourceHandler: public OC::Cm::Server::ResourceHandler::SyncResourceHandler
{

    public:

        /**
         * Default constructor of Messages class.
         *
         * @param[in] pContext Context to which this handler belongs
         */
        MessagesResourceHandler(OC::Cm::IContext *pContext);

        /**
         * Default virtual destructor of Messages class.
         */
        virtual ~MessagesResourceHandler();

        /**
         * This method will be invoked to distinguish type of http request. If required, validation also will be performed.
         *
         * @param[in] methodId  Http method type which needs to be executed (GET, PUT, POST, DELETE)
         * @param[out] statusCode   Http status code to be returned to the client
         * @param[in] request   Serializable request object
         * @param[out] response Serializable response object
         *
         * @return  @c True If the request is properly handled @n
         *          @c False In case of any error occurred
         */
        bool handleSyncResource(std::string methodId, int &statusCode,
                                OC::Cm::Serialization::ISerializable *request,
                                OC::Cm::Serialization::ISerializable *response);

        /**
         * This method will be invoked to handle a GET request.
         *
         * @param[out] statusCode   Http status code to be returned to the client
         * @param[out] respData Messages object to be returned to the server through the serializer. This object should not be re-initialized by a user.
         *
         * @return  @c True If the request is properly handled @n
         *          @c False In case of any error occurred
         */
        bool onGET(int &statusCode, ::Messages *respData);

        /**
         * This method will be invoked to handle a PUT request.
         *
         * @param[out] statusCode   Http status code to be returned to the client
         * @param[in] reqData   Received Messages object through the deserializer. This object should not be re-initialized by a user.
         *
         * @return  @c True If the request is properly handled @n
         *          @c False In case of any error occurred
         */
        bool onPUT(int &statusCode, ::Messages *reqData);

        /**
         * This method will be invoked to handle a POST request.
         *
         * @param[out] statusCode   Http status code to be returned to the client
         * @param[in] reqData   Received Messages object through the deserializer. This object should not be re-initialized by a user.
         * @param[out] location Contains http location header field information to provide the location of a newly created resource or ask a client to load a different URL
         *
         * @return  @c True If the request is properly handled @n
         *          @c False In case of any error occurred
         */
        bool onPOST(int &statusCode, ::Message *reqData, std::string &location);

        /**
         * This method will be invoked to handle a DELETE request.
         *
         * @param[out] statusCode   Http status code to be returned to the client
         *
         * @return  @c True If the request is properly handled @n
         *          @c False In case of any error occurred
         */
        bool onDELETE(int &statusCode);

};

#endif /* MESSAGESRESOURCEHANDLER_H_ */
