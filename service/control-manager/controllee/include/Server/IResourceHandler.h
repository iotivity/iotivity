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

#if !defined(_IRESOURCEHANDLER_H)
#define _IRESOURCEHANDLER_H

#include "string"

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
                 * @interface   IResourceHandler
                 * @brief       Defines methods required to handle requests received
                 *
                 * @since       1.0
                 *
                 * This interface defines methods to handle requests received.
                 *
                 */
                class IResourceHandler
                {

                    public:

                        /**
                         * Default destrcutor of this class
                         */
                        virtual ~IResourceHandler()
                        {
                        }

                        /**
                         * This method will be invoked to initialize the resource handler
                         *
                         * @return  returns @c true true in case of success @n
                         *                  @c false in case of any error
                         */
                        virtual bool onInit() = 0;

                        /**
                         * This method will be invoked to deinitialize the resource handler
                         *
                         * @return  returns @c true true in case of success @n
                         *                  @c false in case of any error
                         */
                        virtual bool onDeinit() = 0;

                        /**
                         * This method will be invoked to handle a request
                         *
                         * @param[in] methodId  method id which needs to be executed
                         * @param[in] request   request object
                         * @param[out] response response object
                         * @param[out] status   status
                         *
                         * @return  returns @c true if request is accepted for handling @n
                         *                  @c false in case of any error
                         */
                        virtual bool handleResource(std::string methodId,
                                                    OC::Cm::Serialization::ISerializable *request,
                                                    OC::Cm::Serialization::ISerializable *response, int &status) = 0;

                };
            }
        }
    }
}

#endif  //_IRESOURCEHANDLER_H
