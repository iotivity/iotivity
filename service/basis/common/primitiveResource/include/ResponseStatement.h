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

#ifndef __RESPONSESTATEMENT_H
#define __RESPONSESTATEMENT_H

#include <ResourceAttributes.h>

namespace OIC
{
    namespace Service
    {

        /**
         * TODO : design for future flexibility
         */
        class ResponseStatement
        {
        public:
            static ResponseStatement create(ResourceAttributes&& attrs)
            {
                return ResponseStatement(attrs);
            }

            explicit ResponseStatement(const ResourceAttributes& attrs) :
                    m_attrs{ attrs }
            {
            }
            explicit ResponseStatement(ResourceAttributes&& attrs) :
                    m_attrs{ std::move(attrs) }
            {
            }

            ResponseStatement(ResponseStatement&&) = default;

            ResponseStatement& operator=(ResponseStatement&&) = default;

        //	std::string getUri() const;
        //	std::vector<std::string> getResourceTypes() const;
        //	std::vector<std::string> getResourceInterfaces() const;

            const ResourceAttributes& getAttributes() const
            {
                return m_attrs;
            }

        private:
            ResourceAttributes m_attrs;
        };

    }
}

#endif // __RESPONSESTATEMENT_H
