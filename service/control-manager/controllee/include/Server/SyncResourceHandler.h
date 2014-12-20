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

#if !defined(_SYNCRESOURCEHANDLER_H)
#define _SYNCRESOURCEHANDLER_H

#include "IResourceHandler.h"

#include "string"
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
                 * @class       SyncResourceHandler
                 * @brief       Defines methods required to handle SHP requests synchronously
                 *
                 * @since       1.0
                 *
                 * This interface defines methods to handle SHP requests synchronously
                 *
                 */
                class SyncResourceHandler: public IResourceHandler
                {
                    public:

                        /**
                         * Default Constructor of this class
                         *
                         * @param[in] context context to which this handler belongs
                         */
                        SyncResourceHandler(IContext *context);

                        /**
                         * Default destructor of this class
                         */
                        virtual ~SyncResourceHandler();

                        /**
                         * This method will be invoked to initialize the resource handler
                         *
                         * @return  returns @c true true in case of success @n
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
                        virtual bool handleResource(std::string methodId, OC::Cm::Serialization::ISerializable *request,
                                                    OC::Cm::Serialization::ISerializable *response, int &statusCode);

                        /**
                         * This method will be invoked to handle a request synchronously
                         *
                         * @param[out] statusCode status code
                         * @param[in] request   request object
                         * @param[out] response response object
                         *
                         * @return  returns @c true if request processed successfully @n
                         *                  @c false in case of any error
                         *
                         */
                        virtual bool handleSyncResource(std::string methodId, int &statusCode,
                                                        OC::Cm::Serialization::ISerializable *request,
                                                        OC::Cm::Serialization::ISerializable *response) = 0;
                        /**
                         * Validates the representation by matching element name. this api determines whether the
                         * serializable object represented by data argument is of type specified by argument elementName
                         *
                         * @param[in] data serializable object
                         * @param[in] elementName name of the element
                         *
                         * @return  returns @c true if matches @n
                         *                  @c false in case of any error
                         */
                        virtual bool validateRepresentation(
                            OC::Cm::Serialization::ISerializable *data,
                            const std::string elementName);

                    protected:
                        IContext *p_context; /**< Represents the instance of the SHP Context where SHP has started */

                };
            }
        }
    }
}

#endif  //_SYNCRESOURCEHANDLER_H
