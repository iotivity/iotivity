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



#if !defined(_ASYNCRESOURCEHANDLER_H)
#define _ASYNCRESOURCEHANDLER_H

#include "CMConfig.h"
#include "IResourceHandler.h"
#include "IContext.h"

//Forward declarations
namespace OC
{
    namespace Cm
    {
        namespace Connector
        {
            namespace Server
            {
                class ServerSession;
            }
        }
        namespace Serialization
        {
            class ISerializable;
        }
    }
}


namespace OC
{
    namespace Cm
    {
        namespace Server
        {
            namespace ResourceHandler
            {

                /**
                *  @addtogroup ResourceHandler
                *  @{
                */

                /**
                 * @class       AsyncResourceHandler
                 * @brief       Defines methods required to handle Control Manager requests asynchronously
                 *
                 * @since       1.0
                 *
                 * This interface defines methods to handle Control Manager requests asynchronously
                 *
                 */
                class CMAPI AsyncResourceHandler : public IResourceHandler
                {

                    public:

                        /**
                         * Default Constructor of this class
                         *
                         * @param[in] context context to which this handler belongs
                         */
                        AsyncResourceHandler(IContext *context) ;

                        /**
                         * Default destructor of this class
                         */
                        virtual ~AsyncResourceHandler() {} ;

                        /**
                         * This method will be invoked to initialize the resource handler
                         *
                         * @return  returns @c true in case of success @n
                         *                  @c false in case of any error
                         */
                        virtual bool onInit();

                        /**
                         * This method will be invoked to deinitialize the resource handler
                         *
                         * @return  returns @c true in case of success @n
                         *                  @c false in case of any error
                         */
                        virtual bool onDeinit();

                        /**
                         * This method will be invoked to handle a request
                         *
                         * @param[in] session   server session to which the request belongs
                         * @param[in] methodId  method id which needs to be executed
                         * @param[in] request   request object
                         * @param[out] response response object
                         *
                         * @return  returns @c true if request is accepted for handling @n
                         *                  @c false in case of any error
                         */
                        bool handleResource( OC::Cm::Connector::Server::ServerSession &session,
                                             std::string methodId,
                                             OC::Cm::Serialization::ISerializable *request,
                                             OC::Cm::Serialization::ISerializable *response);

                        /**
                         * sends the response of request.
                         *
                         * @param[in] statusCode status code of the response
                         * @param[in] session server session to which the request belongs
                         * @param[in] response response object
                         *
                         * @return  returns @c true if request is accepted for handling @n
                         *                  @c false in case of any error
                         */
                        bool sendResponse( int statusCode,
                                           OC::Cm::Connector::Server::ServerSession &session,
                                           OC::Cm::Serialization::ISerializable  *response);

                        /**
                         * This method will be invoked to handle a request asynchronously
                         *
                         * @param[in] session   server session to which the request belongs
                         * @param[in] methodId  method id which needs to be executed
                         * @param[in] request   request object
                         * @param[out] response response object
                         *
                         * @return  returns @c true if request is accepted for asynchronous handling @n
                         *                  @c false in case of any error
                         *
                         */
                        virtual bool handleAsyncResource( OC::Cm::Connector::Server::ServerSession &session,
                                                          std::string methodId,
                                                          OC::Cm::Serialization::ISerializable *request,
                                                          OC::Cm::Serialization::ISerializable *response) = 0;

                    private:
                        IContext *p_context ;
                };
                /** @} */
            }
        }
    }
}

#endif  //_ASYNCRESOURCEHANDLER_H
