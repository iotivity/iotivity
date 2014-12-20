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



#ifndef _CMCLIENTRESOURCE_H
#define _CMCLIENTRESOURCE_H

#include "Client/Resource/IResourceResponseListener.h"
#include "Client/Resource/Resource.h"

namespace OC
{
    namespace Cm
    {
        namespace Client
        {
            /**
             * @namespace   OC::Cm::Client::Resource
             * @brief   This namespace contains classes and interfaces which are required to control Control Manager resources.
             * @since   1.0
             * @remarks @b Header @b file: @b \#include @b <CMClientResource.h> @n
             *          @b Library : @b Shp
             *
             * This namespace contains classes and interfaces which are required to control Control Manager resources.
             *
             * @section OverViewResource The Resource Namespace
             *
             * The Resource namespace defines interfaces and classes that let the application control Control Manager resources.
             * This namespace provides generic controller classes using which any resource can be accessed.
             *
             * To use this namespace, application developer must have the knowledge of Control Manager protocol. Application
             * developers must have the knowledge of input and output data format of Control Manager requests. Then only
             * they can use proper data classes with these classes.
             *
             * Control ManagerSDK generates resource specific controller classes which are derived from class. Resource specific
             * classes can be used to access particular resource only. Usage of these classes does not require Control Manager
             * protocol knowledge
             *
             */
            namespace Resource
            {

            }
        }
    }
}



#endif /* _CMCLIENTRESOURCE_H */
