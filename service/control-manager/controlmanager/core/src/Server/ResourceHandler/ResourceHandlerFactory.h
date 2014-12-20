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



#if !defined(_RESOURCEHANDLERFACTORY_H)
#define _RESOURCEHANDLERFACTORY_H

#include "CMConfig.h"
#include "string"
#include "map"
#include "list"
#include "IContext.h"

//Forward declarations..
namespace OC
{
    namespace Cm
    {
        namespace Server
        {
            namespace ResourceHandler
            {
                class IResourceHandler ;
            }
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
                 * @interface   ResourceHandlerFactory
                 * @brief       Defines methods required to instantiate a ResourceHandler object
                 *
                 * @since       1.0
                 *
                 * This interface defines methods required to instantiate ResourceHandler objects
                 *
                 * Control Manager-SDK generates a empty resource handler stubs which extends OC::Cm::Server::ResourceHandler::IResourceHandler
                 * class. Developers can use these empty resource handler stubs to implement Resource handlers
                 *
                 * Control Manager-SDK also generates a class which extends ResourceHandlerFactory class. This class can instantiate the generated
                 * resource handler objects. Developers must configure an instance of this generated ResourceHandlerFactory class with framework
                 * using OC::Cm::Configuration::setResourceHandlerFactory()
                 *
                 */
                class CMAPI ResourceHandlerFactory
                {
                    public:
                        ResourceHandlerFactory()
                        {
                            p_context = NULL;
                        }
                        /**
                         * Default destructor of this class
                         */
                        virtual ~ResourceHandlerFactory() {}

                        /**
                         * returns instance of a resource handler class which is related to specified resource.
                         *
                         * @param[in] resourceType  Represents the type of the resource
                         *
                         * @return instance of a resource handler
                         */
                        virtual OC::Cm::Server::ResourceHandler::IResourceHandler *createResourceHandler(
                            int resourceType) = 0;

                        /**
                         * registers a resourcetype
                         *
                         * @param[in] resourceType type of the resource
                         * @param[in] path path of the resource
                         *
                         * @return  returns @c true true in case of success @n
                         *                  @c false in case of any error
                         */
                        bool registerResourceHandler(int resourceType, std::string path) ;

                        /**
                         * unregisters a resourcetype
                         *
                         * @param[in] resourceType type of the resource
                         *
                         */
                        void unRegisterResourceHandler( int resourceType ) ;

                        /**
                         * determines resource type of the specified path
                         *
                         * @param[in] path resource path
                         * @param[out] templateParams template parameters of the specified resource path
                         *
                         * @return  returns type of the resource
                         */
                        virtual int getResourceType( std::string path,
                                                     std::map<std::string, std::string> &templateParams ) ;

                        /**
                         * configures context
                         *
                         * @param[in] context context to which this object belongs
                         */
                        virtual void setContext(IContext *context);

                    private:

                        std::map< int, std::list<std::string> > resourceTypeMap ;

                    protected:

                        OC::Cm::IContext
                        *p_context;    /**< Represents the instance of the Control Manager Context where Control Manager has started */
                };
                /** @} */
            }
        }
    }
}

#endif  //_RESOURCEHANDLERFACTORY_H
