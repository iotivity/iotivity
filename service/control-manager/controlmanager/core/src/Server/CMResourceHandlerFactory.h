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



#if !defined(_CMResourceHandlerFactory_H)
#define _CMResourceHandlerFactory_H

#include "CMServerResourceHandler.h"
#include "string"

/**
*  @addtogroup Server
*  @{
*/

/**
 * @class   CMResourceHandlerFactory
 * @brief   This class defines methods required to instantiate a ResourceHandler object
 *
 *@n
 * Control Manager-SDK generates a empty resource handler stubs which extends OC::Cm::Server::ResourceHandler::IResourceHandler
 * class. Developers can use these empty resource handler stubs to implement Resource handlers@n
 *@n
 * Control Manager-SDK also generates a class which extends ResourceHandlerFactory class. This class can instantiate the generated
 * resource handler objects. @n@n Developers must configure an instance of this generated CMResourceHandlerFactory class with framework
 * using OC::Cm::Configuration::setResourceHandlerFactory()
 */
class CMResourceHandlerFactory : public OC::Cm::Server::ResourceHandler::ResourceHandlerFactory
{

    public:

        /**
         * Default constructor of CMResourceHandlerFactory class.
         */
        CMResourceHandlerFactory();

        /**
         * Default destructor of CMResourceHandlerFactory class.
         */
        ~CMResourceHandlerFactory();

        /**
         * Returns instance of a resource handler class which is related to specified resource.
         *
         * @param[in] resourceType Type of the resource (ResourceTypeEnum)
         *
         * @return Instance of a resource handler
         */
        virtual OC::Cm::Server::ResourceHandler::IResourceHandler *createResourceHandler(int resourceType);

};
/** @} */
#endif  //_CMResourceHandlerFactory_H
