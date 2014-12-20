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

#ifndef OICRESOURCE_H_
#define OICRESOURCE_H_

#include "list"
#include "string"


using namespace std;

/**
 * @class   Resource
 * @brief   This class defines methods to serialize and/or deserialize contents of XSD complex/list type TEMPLATE_SERIALIZABLE_NAME
 */


class OICResource
{
    public:

        /**
         * NO default constructor, A parameterized constructor is provided as a design choice.
         */

        OICResource(const std::string &resourceType, const Resource_Property &resourceProperty);
        /**
         * Default virtual destructor of Resource class.
         */
        virtual ~OICResource();

        static void invokeAction ();// TODO: What should the function signature look like??

        int addChildResources();//TODO; Decide the return types
        int removeChildResources();//TODO:Decide the return typeis
        virtual int addProperty(const std::string &key, const std::string &value) = 0; // This shuld in theory build up the OCRepresentation
        virtual int removeProperty(const std::string &key) = 0; //Since there is no way ti remove an element from the OCRepresentation, we have to what to do;

    private:
        list<OICResource> childResources;

};

#endif /* RESOURCE_H_ */
