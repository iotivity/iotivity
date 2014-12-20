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

#ifndef ACCESSPOINTRESOURCEHANDLER_H_
#define ACCESSPOINTRESOURCEHANDLER_H_

#include "IContext.h"
#include "ISerializable.h"
#include "SyncResourceHandler.h"
#include "Elements.h"
#include "ServerSession.h"

/**
 * @class   AccessPointResourceHandler
 * @brief   Defines methods required to handle requests received
 *
 * This class defines methods to handle requests received corresponding to a AccessPoint resource.
 */
class AccessPointResourceHandler: public OC::Cm::Server::ResourceHandler::SyncResourceHandler
{

    public:

        /**
         * AccessPointResourceHandler - Default constructor of AccessPoint class.
         *
         * @param pContext Context to which this handler belongs
         * @return -none
         */
        AccessPointResourceHandler(OC::Cm::IContext *pContext);

        /**
         * ~AccessPointResourceHandler - Default virtual destructor of AccessPoint class.
         * @param -none
         * @return -none
         */
        virtual ~AccessPointResourceHandler();

        /**
         * handleSyncResource - Resource handler logic
         *
         * @param- methodID - The kind of HTTP call
         * @param- statusCode - The status code of the output(output).
         * @param- request - The request
         * @param- response - The output. (output)
         * @return -type:bool - True if the handling was successful, else false
         */
        bool handleSyncResource(std::string methodId, int &statusCode,
                                OC::Cm::Serialization::ISerializable *request,
                                OC::Cm::Serialization::ISerializable *response);

        /**
         * onGET - Handle get
         *
         * @param- statusCode - The status code of the output(output).
         * @param- respData - The output (output)
         * @return -type:bool - True if the handling was successful, else false
         */
        bool onGET(int &statusCode, ::AccessPoint *respData);
};

#endif /* ACCESSPOINTRESOURCEHANDLER_H_ */
