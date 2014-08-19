//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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

/// @file OCRepresentation.h

/// @brief  This file contains the declaration of classes and its members 
///         related to OCRepresentation

#ifndef __OCREPRESENTATION_H
#define __OCREPRESENTATION_H


#include <OCApi.h>
#include <OCResource.h>

namespace OC
{
    class OCRepresentation
    {

    private:

        std::string m_uri;
        AttributeMap m_attributeMap;
        std::vector<std::string> m_resourceTypes;
        std::vector<std::string> m_resourceInterfaces;
        bool m_observable; // TODO : do we need this here???
        int errorCode;

        std::vector<OCRepresentation> m_children;

    public:

        OCRepresentation() {}

        std::string getUri(void) const
        {
            return m_uri;
        }

        void setUri(std::string uri)
        {
            m_uri = uri;
        }

        std::vector<OCRepresentation> getChildren(void) const
        {
            return m_children;
        }

        void setChildren(std::vector<OCRepresentation> children)
        {
            m_children = children;
        }

        OCResource* getResource() const
        {
            // TODO Needs to be implemented
            OCResource* res = NULL;

            return res;
        }

        AttributeMap getAttributeMap() const 
        {
            return m_attributeMap;
        }

        void setAttributeMap(AttributeMap map)
        {
            m_attributeMap = map;
        }

        std::vector<std::string> getResourceTypes() const
        {
            return m_resourceTypes;
        }

        void setResourceTypes(std::vector<std::string> resourceTypes)
        {
            m_resourceTypes = resourceTypes;
        }

        std::vector<std::string> getResourceInterfaces(void) const
        {
            return m_resourceInterfaces;
        }

        void setResourceInterfaces(std::vector<std::string> resourceInterfaces)
        {
            m_resourceInterfaces = resourceInterfaces;
        }
    };

} // namespace OC

#endif //__OCREPRESENTATION_H
