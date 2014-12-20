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

#ifndef LIGHTRESOURCEHANDLER_H_
#define LIGHTRESOURCEHANDLER_H_

#include "IContext.h"
#include "ISerializable.h"
#include "SyncResourceHandler.h"
#include "Elements.h"
#include "ServerSession.h"

/**
 * @class   LightResourceHandler
 * @brief   Defines methods required to handle requests received
 *
 * This class defines methods to handle requests received corresponding to a Light resource.
 */
class LightResourceHandler : public OC::Cm::Server::ResourceHandler::SyncResourceHandler
{

    public:

        /**
         * Default constructor of Light class.
         *
         * @param[in] pContext Context to which this handler belongs
         */
        LightResourceHandler(OC::Cm::IContext *pContext);

        /**
         * Default virtual destructor of Light class.
         */
        virtual ~LightResourceHandler();



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
        bool handleSyncResource(std::string methodId,
                                int &statusCode,
                                OC::Cm::Serialization::ISerializable *request,
                                OC::Cm::Serialization::ISerializable *response);


        /**
         * This method will be invoked to handle a GET request.
         *
         * @param[out] statusCode   Http status code to be returned to the client
         * @param[out] respData Humidity object to be returned to the server through the serializer. This object should not be re-initialized by a user.
         *
         * @return  @c True If the request is properly handled @n
         *          @c False In case of any error occurred
        */
        bool onGET(  int &statusCode, ::Light *respData);

        /**
         * This method will be invoked to handle a PUT request.
         *
         * @param[out] statusCode   Http status code to be returned to the client
         * @param[in] reqData   Received Humidity object through the deserializer. This object should not be re-initialized by a user.
         *
         * @return  @c True If the request is properly handled @n
         *          @c False In case of any error occurred
        */
        bool onPUT(  int &statusCode, ::Light *reqData);

};

#endif /* LIGHTRESOURCEHANDLER_H_ */
