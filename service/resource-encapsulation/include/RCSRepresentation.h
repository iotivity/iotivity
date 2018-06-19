//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

#ifndef RES_ENCAPSULATION_RCSREPRESENTATION_H_
#define RES_ENCAPSULATION_RCSREPRESENTATION_H_

#include "RCSResourceAttributes.h"

namespace OC
{
    class OCRepresentation;
}

namespace OIC
{
    namespace Service
    {

        /**
         * This class describes a resource representation.
         *
         * @see RCSResourceObject
         * @see RCRemoteResourceObject
         */
        class RCSRepresentation
        {
        public:
            RCSRepresentation();

            explicit RCSRepresentation(const std::string& uri);
            explicit RCSRepresentation(const RCSResourceAttributes& attrs);

            RCSRepresentation(const std::string& uri, const std::vector< std::string >& interfaces,
                    const std::vector< std::string >& resourceTypes);

            RCSRepresentation(const std::string& uri, const std::vector< std::string >& interfaces,
                    const std::vector< std::string >& resourceTypes,
                    const RCSResourceAttributes& attrs);

            /**
             * Gets the resource URI.
             *
             * @return Resource URI.
             */
            std::string getUri() const;

            /**
             * Sets the URI of this representation.
             *
             * @param uri Resource URI to be set.
             */
            void setUri(std::string uri);

            /**
             * Gets all interfaces added.
             *
             * @return Vector containing resource interfaces.
             */
            const std::vector< std::string >& getInterfaces() const;

            /**
             * Adds an interface.
             *
             * @param interface an interface to add.
             */
            void addInterface(std::string interface);

            /**
             * Removes all interfaces added.
             */
            void clearInterfaces();

            /**
             * Gets all resource types added.
             *
             * @return Vector containing resource types.
             */
            const std::vector< std::string >& getResourceTypes() const;


            /**
             * Adds a resource type.
             *
             * @param resourceType Resource type to add.
             */
            void addResourceType(std::string resourceType);

            /**
             * Removes all resource types.
             */
            void clearResourceTypes();

            /**
             * Gets attributes set in this representation.
             *
             * @return Resource attributes.
             *
             * @see RCSResourceAttributes
             */
            const RCSResourceAttributes& getAttributes() const;

            /**
             * @overload
             */
            RCSResourceAttributes& getAttributes();

            /**
             * Overwrite attributes.
             *
             * @param attrs new attributes.
             */
            void setAttributes(const RCSResourceAttributes& attrs);

            /**
             * @overload
             */
            void setAttributes(RCSResourceAttributes&& attrs);

            /**
             * Gets children of this representation.
             *
             * @return children of this representation.
             */
            const std::vector< RCSRepresentation >& getChildren() const;

            /**
             * Adds a child to this representation.
             *
             * @param child a representation to be attached.
             */
            void addChild(RCSRepresentation child);

            /**
             * Sets children of this representation.
             *
             * @param children new children.
             */
            void setChildren(std::vector< RCSRepresentation > children);

            /**
             * Removse all children
             */
            void clearChildren();

            /**
             * Converts OCRepresentation into RCSRepresentation.
             *
             * @return Converted RCSRepresentation.
             *
             * @see toOCRepresentation
             */
            static RCSRepresentation fromOCRepresentation(const OC::OCRepresentation&);


            /**
             * Converts RCSRepresentation into OCRepresentation.
             *
             * @return Converted OCRepresentation.
             *
             * @see fromOCRepresentation
             */
            static OC::OCRepresentation toOCRepresentation(const RCSRepresentation&);
            static OC::OCRepresentation toOCRepresentation(RCSRepresentation&&);

        private:
            std::string m_uri;

            std::vector< std::string > m_interfaces;
            std::vector< std::string > m_resourceTypes;

            RCSResourceAttributes m_attributes;

            std::vector< RCSRepresentation > m_children;
        };

    }
}

#endif /* RES_ENCAPSULATION_RCSREPRESENTATION_H_ */
