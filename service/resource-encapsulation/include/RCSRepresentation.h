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

            std::string getUri() const;

            void setUri(const std::string& uri);
            void setUri(std::string&& uri);

            const std::vector< std::string >& getInterfaces() const;

            void addInterface(const std::string& interface);
            void addInterface(std::string&& interface);

            void clearInterfaces();

            const std::vector< std::string >& getResourceTypes() const;

            void addResourceType(const std::string& resourceType);
            void addResourceType(std::string&& resourceType);

            void clearResourceTypes();

            const RCSResourceAttributes& getAttributes() const;
            RCSResourceAttributes& getAttributes();

            void setAttributes(const RCSResourceAttributes& attrs);
            void setAttributes(RCSResourceAttributes&& attrs);

            const std::vector< RCSRepresentation >& getChildren() const;

            void addChild(const RCSRepresentation&);
            void addChild(RCSRepresentation&&);

            void setChildren(const std::vector< RCSRepresentation >&);
            void setChildren(std::vector< RCSRepresentation >&&);

            void clearChildren();

            static RCSRepresentation fromOCRepresentation(const OC::OCRepresentation&);

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
